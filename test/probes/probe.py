
import os, pathlib, subprocess, sys, tempfile

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
        if name == "oswalk_s":
            hits = []
            for dirpath, _dirs, files in os.walk(work):
                hits += [f for f in files if f.endswith(".s")]
            return 0 if hits else 1
        if name == "direct_main_s":
            return 0 if (work / ".cache" / "main.s").exists() else 1
        if name == "direct_src_main_s":
            return 0 if (work / ".cache" / "src" / "main.s").exists() else 1
        if name == "oswalk_any":
            hits = []
            for dirpath, _dirs, files in os.walk(work):
                hits += files
            return 0 if len(hits) > 3 else 1
    return 1

if __name__ == "__main__":
    raise SystemExit(main())
