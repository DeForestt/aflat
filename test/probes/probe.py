
import pathlib, subprocess, sys, tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
PROBES = {
  'iso_a': 'fn main() -> int {\n  const isize a = 5;\n  if a != 5 { return 1; };\n  return 0;\n};\n',
  'iso_b': 'fn main() -> int {\n  mutable isize a = 0;\n  a = a - 5;\n  if a >= 0 { return 9; };\n  return 0;\n};\n',
  'iso_c': 'fn main() -> int {\n  const isize a = 0 - 5;\n  if a < 0 { return 0; };\n  return 9;\n  return 0;\n};\n',
  'iso_d': 'fn main() -> int {\n  const i64 a = 0 - 5;\n  if a >= 0 { return 9; };\n  return 0;\n};\n',
  'iso_e': 'fn main() -> int {\n  const isize a = 0 - 5;\n  if a == 0 - 5 { return 0; };\n  return 1;\n  return 0;\n};\n',
  'iso_f': 'fn main() -> int {\n  const isize a = 0 - 5;\n  const isize b = 0 - 2;\n  if a / b != 2 { return 1; };\n  return 0;\n};\n',
  'iso_g': 'fn main() -> int {\n  const isize a = 0 - 5;\n  if a / 2 != 0 - 2 { return 10; };\n  return 0;\n};\n',
  'm_a': 'fn main() -> int {\n  const i64 w = #100;\n  if (w % 3) != 1 { return 1; };\n  return 0;\n};\n',
  'm_b': 'fn main() -> int {\n  const i32 w = 100;\n  if (w % 3) != 1 { return 1; };\n  return 0;\n};\n',
  'm_c': 'fn main() -> int {\n  const u64 z = 100;\n  if (z % 3) != 1 { return 1; };\n  return 0;\n};\n',
  'm_d': 'fn main() -> int {\n  const i64 w = #6442450944;\n  const i64 r = w % 2;\n  if r != 0 { return 8; };\n  return 0;\n};\n',
  'm_e': 'fn main() -> int {\n  const i64 w = #6442450944;\n  if (w % 3) != 0 { return 1; };\n  return 0;\n};\n',
}

def main():
    name = sys.argv[1]
    source = PROBES[name]
    aflat = ROOT / "bin" / "aflat"
    with tempfile.TemporaryDirectory() as tmp:
        work = pathlib.Path(tmp)
        (work / "src").mkdir()
        (work / "bin").mkdir()
        (work / "aflat.cfg").write_text("[build]\nmain = main\n")
        (work / "src" / "main.af").write_text(source)
        subprocess.run([str(aflat), "--no-cache", "build"], cwd=work, check=True)
        ran = subprocess.run([str(work / "bin" / "a.out")], cwd=work)
        return 0 if ran.returncode == 0 else 1

if __name__ == "__main__":
    raise SystemExit(main())
