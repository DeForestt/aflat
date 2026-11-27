
import pathlib, subprocess, sys, tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = "fn main() -> int { return 0; };\n"

def build(work, no_cache):
    (work / "src").mkdir(exist_ok=True)
    (work / "bin").mkdir(exist_ok=True)
    (work / "aflat.cfg").write_text("[build]\nmain = main\n")
    (work / "src" / "main.af").write_text(SRC)
    cmd = [str(ROOT / "bin" / "aflat")]
    if no_cache:
        cmd.append("--no-cache")
    cmd.append("build")
    subprocess.run(cmd, cwd=work, check=True)

def main():
    name = sys.argv[1]
    aflat_cache = pathlib.Path.home() / ".cache" / "aflat"
    with tempfile.TemporaryDirectory() as tmp:
        work = pathlib.Path(tmp)
        if name == "cache_main_s":
            build(work, True)
            return 0 if (work / ".cache" / "main.s").exists() else 1
        if name == "cache_main_s_cached":
            build(work, False)
            return 0 if (work / ".cache" / "main.s").exists() else 1
        if name == "any_s_in_work":
            build(work, True)
            hits = [p for p in work.rglob("*") if p.suffix == ".s"]
            return 0 if hits else 1
        if name == "any_s_in_work_cached":
            build(work, False)
            hits = [p for p in work.rglob("*") if p.suffix == ".s"]
            return 0 if hits else 1
        if name == "home_cache":
            build(work, True)
            return 0 if aflat_cache.exists() else 1
    return 1

if __name__ == "__main__":
    raise SystemExit(main())
