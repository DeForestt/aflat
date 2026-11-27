
import pathlib, subprocess, sys, tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
PROBES = {
  'long_ge': 'fn main() -> int {\n  const long a = 0 - 5;\n  if a >= 0 { return 9; };\n  return 0;\n};\n',
  'long_lt': 'fn main() -> int {\n  const long a = 0 - 5;\n  if a < 0 { return 0; };\n  return 9;\n  return 0;\n};\n',
  'long_div': 'fn main() -> int {\n  const long a = 0 - 5;\n  if a / 2 != -2 { return 10; };\n  return 0;\n};\n',
  'long_mod': 'fn main() -> int {\n  const long w = #100;\n  if (w % 3) != 1 { return 1; };\n  return 0;\n};\n',
  'long_mod_small': 'fn main() -> int {\n  mutable long w = #100;\n  w = w % 3;\n  if w != 1 { return 1; };\n  return 0;\n};\n',
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
