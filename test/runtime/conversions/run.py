#!/usr/bin/env python3
"""Runtime checks for the integer conversion rules.

Pass programs assert exact values after conversions at declaration
and assignment boundaries. Fail programs must be rejected at compile
time with diagnostics naming the source, the destination, and the
reason. Run from anywhere:

    python3 test/runtime/conversions/run.py [name-filter ...]

With no arguments every program runs; with arguments only programs
whose name contains one of the arguments run.
"""
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
AFLAT = ROOT / "bin" / "aflat"
SUITE = Path(__file__).resolve().parent

PASS_PROGRAMS = [
    "decl_sign_ext",
    "decl_sign_lit",
    "decl_unsigned",
    "decl_loaded",
    "assign_values",
    "assign_fields",
    "literal_bounds",
    "arg_values",
    "return_values",
    "mixed_values",
    "mod_values",
    "compare_values",
    "cast_values",
    "cast_checked2",
    "cast_wrap",
    "cast_extra",
]

FAIL_PROGRAMS = {
    "decl_byte_300": (
        "const byte b = 300;",
        ["does not fit in byte", "0..255"],
    ),
    "decl_byte_negative": (
        "const byte b = -1;",
        ["does not fit in byte"],
    ),
    "decl_u32_negative": (
        "const u32 x = -1;",
        ["does not fit in u32"],
    ),
    "decl_i8_over": (
        "const i8 x = 128;",
        ["does not fit in i8", "-128..127"],
    ),
    "decl_i8_under": (
        "const i8 x = -129;",
        ["does not fit in i8"],
    ),
    "decl_typed_int_to_usize": (
        "const int n = 5;\n    const usize m = n;",
        ["cannot implicitly convert", "cannot represent negative"],
    ),
    "decl_typed_narrowing": (
        "const long n = #5;\n    const int m = n;",
        ["cannot implicitly convert"],
    ),
    "decl_typed_sign_change": (
        "const i8 n = -1;\n    const u8 m = n;",
        ["cannot implicitly convert"],
    ),
    "decl_let_oversized": (
        "let x = 5000000000;",
        ["does not fit in i32", "5000000000"],
    ),
    "assign_byte_300": (
        "mutable byte b = 0;\n    b = 300;",
        ["does not fit in byte"],
    ),
    "assign_typed_int_to_usize": (
        "const int n = 5;\n    mutable usize m = 0;\n    m = n;",
        ["cannot implicitly convert"],
    ),
}

GLOBAL_FAIL_PROGRAMS = {
    "global_byte_300": (
        "const byte G = 300;",
        ["does not fit in byte"],
    ),
}

# Complete programs that must be rejected at compile time (slice 3:
# call-argument and return boundaries).
FULL_FAIL_PROGRAMS = {
    "arg_typed_int_to_usize": (
        ".needs <std>\n"
        "fn take(const usize v) -> usize { return v; };\n"
        "fn main() -> int {\n"
        "    const int n = 5;\n"
        "    take(n);\n"
        "    return 0;\n"
        "};\n",
        ["cannot implicitly convert"],
    ),
    "arg_literal_byte_300": (
        ".needs <std>\n"
        "fn take(const byte v) -> void {};\n"
        "fn main() -> int {\n"
        "    take(300);\n"
        "    return 0;\n"
        "};\n",
        ["does not fit in byte"],
    ),
    "arg_literal_u32_negative": (
        ".needs <std>\n"
        "fn take(const u32 v) -> void {};\n"
        "fn main() -> int {\n"
        "    take(-1);\n"
        "    return 0;\n"
        "};\n",
        ["does not fit in u32"],
    ),
    "arg_typed_narrowing": (
        ".needs <std>\n"
        "fn take(const i32 v) -> void {};\n"
        "fn main() -> int {\n"
        "    const i64 n = #5;\n"
        "    take(n);\n"
        "    return 0;\n"
        "};\n",
        ["cannot implicitly convert"],
    ),
    "return_typed_int_to_usize": (
        ".needs <std>\n"
        "fn f() -> usize {\n"
        "    const int n = 5;\n"
        "    return n;\n"
        "};\n"
        "fn main() -> int { return 0; };\n",
        ["cannot implicitly convert"],
    ),
    "return_literal_byte_300": (
        ".needs <std>\n"
        "fn f() -> byte { return 300; };\n"
        "fn main() -> int { return 0; };\n",
        ["does not fit in byte"],
    ),
    "return_typed_narrowing": (
        ".needs <std>\n"
        "fn f() -> i32 {\n"
        "    const long v = #5;\n"
        "    return v;\n"
        "};\n"
        "fn main() -> int { return 0; };\n",
        ["cannot implicitly convert"],
    ),
    "mixed_no_common_i64_u64": (
        ".needs <std>\n"
        "fn main() -> int {\n"
        "    const i64 a = #1;\n"
        "    const u64 b = #2;\n"
        "    const i64 c = a + b;\n"
        "    return 0;\n"
        "};\n",
        ["no integer type can represent"],
    ),
    "mixed_no_common_isize_usize": (
        ".needs <std>\n"
        "fn main() -> int {\n"
        "    const isize a = #1;\n"
        "    const usize b = 2;\n"
        "    const isize c = a + b;\n"
        "    return 0;\n"
        "};\n",
        ["no integer type can represent"],
    ),
    "mixed_no_common_i64_usize": (
        ".needs <std>\n"
        "fn main() -> int {\n"
        "    const i64 a = #1;\n"
        "    const usize b = 2;\n"
        "    const i64 c = a - b;\n"
        "    return 0;\n"
        "};\n",
        ["no integer type can represent"],
    ),
    "cmp_no_common_u64_negative": (
        ".needs <std>\n"
        "fn main() -> int {\n"
        "    const u64 b = #2;\n"
        "    if b == -1 { return 1; };\n"
        "    return 0;\n"
        "};\n",
        ["no integer type can represent"],
    ),
    "cast_constant_negative_to_unsigned": (
        ".needs <std>\n"
        "fn main() -> int {\n"
        "    let n = -10 as u32;\n"
        "    return 0;\n"
        "};\n",
        ["checked conversion of constant", "always fails"],
    ),
    "cast_constant_too_big_for_byte": (
        ".needs <std>\n"
        "fn main() -> int {\n"
        "    let n = 300 as byte;\n"
        "    return 0;\n"
        "};\n",
        ["checked conversion of constant", "always fails"],
    ),
}


def annotate(title: str, text: str) -> None:
    """Surface failure details as GitHub workflow annotations.

    Actions logs are not readable in every environment; lines printed
    in ::error:: workflow-command form become check-run annotations,
    which are. Keep each failure to a handful of short lines.
    """
    def esc(s: str) -> str:
        return (s.replace("%", "%25").replace("\r", "%0D")
                 .replace("\n", "%0A"))
    def esc_prop(s: str) -> str:
        return esc(s).replace(":", "%3A").replace(",", "%2C")
    lines = [ln for ln in text.splitlines() if ln.strip()][:10]
    if not lines:
        lines = ["(no output)"]
    for ln in lines:
        print(f"::error title={esc_prop(title)}::{esc(ln[:400])}",
              flush=True)


def write_project(project: Path, source: str) -> None:
    # Same project shape as the other runtime suites (deque, vector,
    # format_sized): an INI config and a pre-created bin directory.
    (project / "src").mkdir(parents=True)
    (project / "bin").mkdir(parents=True)
    (project / "aflat.cfg").write_text("[build]\nmain = main\n")
    (project / "src" / "main.af").write_text(source)


def build(project: Path) -> subprocess.CompletedProcess:
    return subprocess.run(
        [str(AFLAT), "--no-cache", "build"],
        cwd=project,
        capture_output=True,
        text=True,
    )


def run_pass(name: str) -> bool:
    source = (SUITE / f"{name}.af").read_text()
    with tempfile.TemporaryDirectory() as tmp:
        project = Path(tmp)
        write_project(project, source)
        built = build(project)
        if built.returncode != 0:
            print(f"FAIL {name}: build failed\n{built.stdout}{built.stderr}")
            annotate(f"conversions {name} build", built.stdout + built.stderr)
            return False
        if not (project / "bin" / "a.out").exists():
            print(f"FAIL {name}: build produced no bin/a.out")
            annotate(f"conversions {name} build",
                     "build exited 0 but produced no bin/a.out")
            return False
        ran = subprocess.run(
            [str(project / "bin" / "a.out")],
            capture_output=True,
            text=True,
        )
        if ran.returncode != 0:
            print(f"FAIL {name}: exited {ran.returncode}\n"
                  f"{ran.stdout}{ran.stderr}")
            annotate(f"conversions {name} run (exit {ran.returncode})",
                     ran.stdout + ran.stderr)
            return False
    print(f"PASS {name}")
    return True


def run_fail(name: str, body: str, needles: list, global_scope: bool) -> bool:
    if global_scope:
        source = f".needs <std>\n{body}\nfn main() -> int {{ return 0; }};\n"
    else:
        source = (".needs <std>\nfn main() -> int {\n    " + body +
                  "\n    return 0;\n};\n")
    with tempfile.TemporaryDirectory() as tmp:
        project = Path(tmp)
        write_project(project, source)
        built = build(project)
        output = built.stdout + built.stderr
        if built.returncode == 0:
            print(f"FAIL {name}: expected a compile error, build succeeded")
            annotate(f"conversions {name}",
                     "expected a compile error, build succeeded")
            return False
        missing = [n for n in needles if n not in output]
        if missing:
            print(f"FAIL {name}: diagnostic missing {missing}\n{output}")
            annotate(f"conversions {name} missing {missing}", output)
            return False
    print(f"PASS {name}")
    return True


def run_full_fail(name: str, source: str, needles: list) -> bool:
    with tempfile.TemporaryDirectory() as tmp:
        project = Path(tmp)
        write_project(project, source)
        built = build(project)
        output = built.stdout + built.stderr
        if built.returncode == 0:
            print(f"FAIL {name}: expected a compile error, build succeeded")
            annotate(f"conversions {name}",
                     "expected a compile error, build succeeded")
            return False
        missing = [n for n in needles if n not in output]
        if missing:
            print(f"FAIL {name}: diagnostic missing {missing}\n{output}")
            annotate(f"conversions {name} missing {missing}", output)
            return False
    print(f"PASS {name}")
    return True


def main() -> int:
    if not AFLAT.exists():
        print(f"aflat binary not found at {AFLAT}")
        return 1
    filters = sys.argv[1:]
    ok = True
    for name in PASS_PROGRAMS:
        if filters and ("fails" in filters or
                        not any(f in name for f in filters)):
            continue
        ok = run_pass(name) and ok
    for name, (body, needles) in FAIL_PROGRAMS.items():
        if filters and "fails" not in filters and \
                not any(f in name for f in filters):
            continue
        ok = run_fail(name, body, needles, False) and ok
    for name, (body, needles) in GLOBAL_FAIL_PROGRAMS.items():
        if filters and "fails" not in filters and \
                not any(f in name for f in filters):
            continue
        ok = run_fail(name, body, needles, True) and ok
    for name, (source, needles) in FULL_FAIL_PROGRAMS.items():
        if filters and "full_fails" not in filters and \
                not any(f in name for f in filters):
            continue
        ok = run_full_fail(name, source, needles) and ok
    return 0 if ok else 1


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except SystemExit:
        raise
    except Exception as exc:  # harness bug: surface it as an annotation
        annotate("conversions harness", f"{type(exc).__name__}: {exc}")
        raise
