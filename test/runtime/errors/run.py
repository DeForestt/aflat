#!/usr/bin/env python3
"""Build and run owned error regressions against the local standard library."""
from pathlib import Path
import os
import resource
import shutil
import subprocess
import sys
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
    with tempfile.TemporaryDirectory(prefix="aflat-errors-") as temporary:
        project = Path(temporary)
        (project / "src").mkdir()
        (project / "bin").mkdir()
        (project / "aflat.cfg").write_text("[build]\nmain = main\n")
        for suite in sys.argv[1:] or ["main.af", "derived.af"]:
            shutil.copyfile(HERE / suite, project / "src/main.af")
            subprocess.run([ROOT / "bin/aflat", "--no-cache", "build"],
                           cwd=project, check=True, timeout=180, preexec_fn=limits)
            subprocess.run([project / "bin/a.out"], cwd=project,
                           check=True, timeout=60, preexec_fn=limits)
            print(f"errors {suite}: passed", flush=True)


if __name__ == "__main__":
    main()
