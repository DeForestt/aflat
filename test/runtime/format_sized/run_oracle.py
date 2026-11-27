import pathlib
import subprocess
import sys
import tempfile


ROOT = pathlib.Path(__file__).resolve().parents[3]


def main():
    aflat = ROOT / "bin" / "aflat"
    source_name = sys.argv[1]
    expected = int(sys.argv[2])
    source = pathlib.Path(__file__).with_name(source_name)
    with tempfile.TemporaryDirectory() as tmp:
        work = pathlib.Path(tmp)
        (work / "src").mkdir()
        (work / "bin").mkdir()
        (work / "aflat.cfg").write_text("[build]\nmain = main\n")
        (work / "src" / "main.af").write_text(source.read_text())
        subprocess.run([aflat, "--no-cache", "build"], cwd=work, check=True)
        result = subprocess.run([work / "bin" / "a.out"], cwd=work)
        print(f"{source_name} exited {result.returncode}, expected {expected}")
        return 0 if result.returncode == expected else 1


if __name__ == "__main__":
    raise SystemExit(main())
