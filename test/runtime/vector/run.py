#!/usr/bin/env python3
"""Build and run vector storage regressions against the local standard library."""
from pathlib import Path
import os
import resource
import shutil
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]


def limits():
    # Keep regression builds sequential and bounded on small development hosts.
    resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
    resource.setrlimit(resource.RLIMIT_AS, (2 * 1024**3, 2 * 1024**3))
    resource.setrlimit(resource.RLIMIT_CPU, (120, 120))
    os.nice(10)


def main():
    with tempfile.TemporaryDirectory(prefix="aflat-vector-") as temporary:
        project = Path(temporary)
        (project / "src").mkdir()
        (project / "bin").mkdir()
        (project / "aflat.cfg").write_text("[build]\nmain = main\n")
        for suite in ["shared.af"]:
            shutil.copyfile(HERE / suite, project / "src/main.af")
            subprocess.run([ROOT / "bin/aflat", "--no-cache", "build"],
                           cwd=project, check=True, timeout=180, preexec_fn=limits)
            subprocess.run([project / "bin/a.out"], cwd=project,
                           check=True, timeout=60, preexec_fn=limits)
            print(f"vector {suite}: passed", flush=True)

        (project / "src/main.af").write_text(
            '.needs <std>\nimport vector from "Collections/Vector";\n'
            'unique class Owned { int value = 1; };\n'
            'fn main() -> int {\n'
            '    let source = new vector::<Owned>();\n'
            '    let destination = new vector::<Owned>();\n'
            '    destination.stitch(source);\n'
            '    return 0;\n};\n')
        rejected = subprocess.run([ROOT / "bin/aflat", "--no-cache", "build"],
                                  cwd=project, text=True, capture_output=True,
                                  timeout=180, preexec_fn=limits)
        assert rejected.returncode != 0, "borrowed move-only stitch was accepted"
        assert "stitch" in rejected.stdout + rejected.stderr, rejected
        print("vector borrowed move-only stitch: rejected", flush=True)


if __name__ == "__main__":
    main()
