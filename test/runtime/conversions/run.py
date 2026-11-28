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
    "decl_signed",
    "decl_unsigned",
    "decl_loaded",
    "assign_values",
    "assign_fields",
    "literal_bounds",
    "arg_values",
    "return_values",
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
}


def write_project(project: Path, source: str) -> None:
    (project / "src").mkdir(parents=True)
    (project / "aflat.cfg").write_text(' {
        "name": "conversions-test",
        "bin": "bin",
        "src": "src",
        "entry": "main.af",
        "include": []
    }')
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
            return False
        ran = subprocess.run(
            [str(project / "bin" / "a.out")],
            capture_output=True,
            text=True,
        )
        if ran.returncode != 0:
            print(f"FAIL {name}: exited {ran.returncode}\n"
                  f"{ran.stdout}{ran.stderr}")
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
            return False
        missing = [n for n in needles if n not in output]
        if missing:
            print(f"FAIL {name}: diagnostic missing {missing}\n{output}")
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
            return False
        missing = [n for n in needles if n not in output]
        if missing:
            print(f"FAIL {name}: diagnostic missing {missing}\n{output}")
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
    raise SystemExit(main())
