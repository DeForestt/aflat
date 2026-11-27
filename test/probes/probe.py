
import pathlib, re, subprocess, sys, tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]

CASES = {
  # name: (source, mode, pattern)  mode: "has" or "lacks"
  "asm_exists": ("""fn main() -> int { return 0; };
""", "has", r"idiv|ret"),
  "i64mod_cqto": ("""fn main() -> int {
  const i64 wide = #6442450944;
  if (wide % 2) != 0 { return 8; };
  return 0;
};
""", "has", r"cqto"),
  "i64mod_cltd": ("""fn main() -> int {
  const i64 wide = #6442450944;
  if (wide % 2) != 0 { return 8; };
  return 0;
};
""", "lacks", r"cltd"),
  "i64mod_movrdx": ("""fn main() -> int {
  const i64 wide = #6442450944;
  if (wide % 2) != 0 { return 8; };
  return 0;
};
""", "has", r"movq\s+%rdx, %rax"),
  "u64mod_xor": ("""fn main() -> int {
  const u64 zero64 = 0;
  const u64 under64 = zero64 - 1;
  if (under64 % 2) != 1 { return 12; };
  return 0;
};
""", "has", r"xor\s+%rdx, %rdx"),
  "isize_div_cqto": ("""fn main() -> int {
  const isize negSize = 0 - 5;
  if negSize / 2 != -2 { return 10; };
  return 0;
};
""", "has", r"cqto"),
  "isize_cmp_setge": ("""fn main() -> int {
  const isize negSize = 0 - 5;
  if negSize >= 0 { return 9; };
  return 0;
};
""", "has", r"setge"),
  "isize_cmp_setae": ("""fn main() -> int {
  const isize negSize = 0 - 5;
  if negSize >= 0 { return 9; };
  return 0;
};
""", "lacks", r"setae"),
}

def main():
    name = sys.argv[1]
    source, mode, pattern = CASES[name]
    aflat = ROOT / "bin" / "aflat"
    with tempfile.TemporaryDirectory() as tmp:
        work = pathlib.Path(tmp)
        (work / "src").mkdir()
        (work / "bin").mkdir()
        (work / "aflat.cfg").write_text("[build]\nmain = main\n")
        (work / "src" / "main.af").write_text(source)
        subprocess.run([str(aflat), "--no-cache", "build"], cwd=work, check=True)
        asm = ""
        for p in work.rglob("*.s"):
            asm += p.read_text(errors="replace")
        found = re.search(pattern, asm) is not None
        ok = found if mode == "has" else not found
        print(f"asm probe {name}: pattern={pattern!r} found={found} mode={mode}")
        return 0 if ok else 1

if __name__ == "__main__":
    raise SystemExit(main())
