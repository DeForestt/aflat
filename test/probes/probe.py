
import pathlib, subprocess, sys, tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
PROBES = {'u1': 'fn main() -> int {\n  mutable u8 small = 255;\n  small = small + 1;\n  if small != 0 { return 1; };\n  return 0;\n};\n', 'u2': 'fn main() -> int {\n  mutable u16 mid = 65535;\n  mid = mid + 1;\n  if mid != 0 { return 2; };\n  return 0;\n};\n', 'u3': 'fn main() -> int {\n  const u32 two = 2000000000;\n  const u32 big = two + two;\n  if big < two { return 3; };\n  return 0;\n};\n', 'u4': 'fn main() -> int {\n  const u32 two = 2000000000;\n  const u32 big = two + two;\n  if big / 2 != 2000000000 { return 4; };\n  return 0;\n};\n', 'u5': 'fn main() -> int {\n  const u32 two = 2000000000;\n  const u32 big = two + two;\n  if (big % 2) != 0 { return 5; };\n  return 0;\n};\n', 'u6': 'fn main() -> int {\n  const u32 zero32 = 0;\n  const u32 under = zero32 - 1;\n  if under < zero32 { return 6; };\n  return 0;\n};\n', 'u7': 'fn main() -> int {\n  const u32 zero32 = 0;\n  const u32 under = zero32 - 1;\n  if (under >> 1) != 2147483647 { return 7; };\n  return 0;\n};\n', 'u8': 'fn main() -> int {\n  const u32 zero32 = 0;\n  const u32 under = zero32 - 1;\n  if under / 2 != 2147483647 { return 8; };\n  return 0;\n};\n', 'u9': 'fn main() -> int {\n  const u32 zero32 = 0;\n  const u32 under = zero32 - 1;\n  if (under % 2) != 1 { return 9; };\n  return 0;\n};\n', 'u10': 'fn main() -> int {\n  const u64 zero64 = 0;\n  const u64 under64 = zero64 - 1;\n  if under64 < zero64 { return 10; };\n  return 0;\n};\n', 'u11': 'fn main() -> int {\n  const u64 zero64 = 0;\n  const u64 under64 = zero64 - 1;\n  if under64 / 2 != #9223372036854775807 { return 11; };\n  return 0;\n};\n', 'u12': 'fn main() -> int {\n  const u64 zero64 = 0;\n  const u64 under64 = zero64 - 1;\n  if (under64 % 2) != 1 { return 12; };\n  return 0;\n};\n', 'u13': 'fn main() -> int {\n  const usize zeroSize = 0;\n  const usize underSize = zeroSize - 1;\n  if underSize < zeroSize { return 13; };\n  return 0;\n};\n', 's1': 'fn main() -> int {\n  mutable i8 tiny = 127;\n  tiny = tiny + 1;\n  if tiny != -128 { return 1; };\n  return 0;\n};\n', 's2': 'fn main() -> int {\n  mutable i8 tiny = 127;\n  tiny = tiny + 1;\n  if tiny >= 0 { return 2; };\n  return 0;\n};\n', 's3': 'fn main() -> int {\n  mutable i16 neg = -300;\n  neg = neg / 2;\n  if neg != -150 { return 3; };\n  return 0;\n};\n', 's4': 'fn main() -> int {\n  const i32 negOne = 0 - 1;\n  if (negOne >> 1) != -1 { return 4; };\n  return 0;\n};\n', 's5': 'fn main() -> int {\n  const i32 negEight = 0 - 8;\n  if negEight / 2 != -4 { return 5; };\n  return 0;\n};\n', 's6': 'fn main() -> int {\n  const i32 negEight = 0 - 8;\n  if (negEight >> 2) != -2 { return 6; };\n  return 0;\n};\n', 's7': 'fn main() -> int {\n  const i64 wide = #6442450944;\n  if wide / 2 != #3221225472 { return 7; };\n  return 0;\n};\n', 's8': 'fn main() -> int {\n  const i64 wide = #6442450944;\n  if (wide % 2) != 0 { return 8; };\n  return 0;\n};\n', 's9': 'fn main() -> int {\n  const isize negSize = 0 - 5;\n  if negSize >= 0 { return 9; };\n  return 0;\n};\n', 's10': 'fn main() -> int {\n  const isize negSize = 0 - 5;\n  if negSize / 2 != -2 { return 10; };\n  return 0;\n};\n'}

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
        print(f"probe {name}: exit={ran.returncode} expected=0")
        return 0 if ran.returncode == 0 else 1

if __name__ == "__main__":
    raise SystemExit(main())
