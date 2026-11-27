
import pathlib, subprocess, sys, tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = "fn main() -> int { return 0; };\n"

def build(work):
    (work / "src").mkdir(exist_ok=True)
    (work / "bin").mkdir(exist_ok=True)
    (work / "aflat.cfg").write_text("[build]\nmain = main\n")
    (work / "src" / "main.af").write_text(SRC)
    subprocess.run([str(ROOT / "bin" / "aflat"), "--no-cache", "build"],
                   cwd=work, check=True)

def main():
    name = sys.argv[1]
    with tempfile.TemporaryDirectory() as tmp:
        work = pathlib.Path(tmp)
        build(work)
        if name == "work_cache_dir":
            return 0 if (work / ".cache").exists() else 1
        if name == "work_any_new_file":
            names = [p.name for p in work.rglob("*") if p.is_file()]
            return 0 if any("main" in n and n != "main.af" for n in names) else 1
        if name == "root_cache_main_s":
            return 0 if (ROOT / ".cache" / "main.s").exists() else 1
        if name == "root_cache_dir":
            return 0 if (ROOT / ".cache").exists() else 1
        if name == "work_bin_aout":
            return 0 if (work / "bin" / "a.out").exists() else 1
    return 1

if __name__ == "__main__":
    raise SystemExit(main())
