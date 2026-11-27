import pathlib
import subprocess
import sys
import tempfile


ROOT = pathlib.Path(__file__).resolve().parents[3]
TESTS = ["main.af"]


def run(command, cwd):
    print("+", " ".join(str(part) for part in command), flush=True)
    subprocess.run(command, cwd=cwd, check=True)


def build_and_run(aflat, project_dir, source_name):
    source = pathlib.Path(__file__).with_name(source_name)
    (project_dir / "main.af").write_text(source.read_text())
    run([aflat, "--no-cache", "build"], project_dir)
    # The suite uses plain assert(), which aborts with a nonzero exit
    # code on failure, so a clean run is the pass signal.
    run([project_dir / "bin" / "a.out"], project_dir)


def main():
    aflat = ROOT / "bin" / "aflat"
    if not aflat.exists():
        print(f"missing built compiler: {aflat}", file=sys.stderr)
        return 1

    with tempfile.TemporaryDirectory() as tmp:
        work = pathlib.Path(tmp)
        (work / "aflat.cfg").write_text("[build]\nmain = main\n")
        for name in TESTS:
            build_and_run(aflat, work, name)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
