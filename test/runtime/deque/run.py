#!/usr/bin/env python3
"""Run the stdlib deque tests: python3 test/runtime/deque/run.py."""
from pathlib import Path
import re
import shutil
import subprocess
import tempfile


HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]


def main():
    with tempfile.TemporaryDirectory(prefix="aflat-deque-") as temporary:
        project = Path(temporary)
        (project / "src").mkdir()
        (project / "bin").mkdir()
        (project / "aflat.cfg").write_text("[build]\nmain = main\n")
        for suite in ["main.af", "specializations.af"]:
            shutil.copyfile(HERE / suite, project / "src/main.af")
            subprocess.run([ROOT / "bin/aflat", "--no-cache", "build"],
                           cwd=project, check=True, timeout=300)
            result = subprocess.run([project / "bin/a.out"], cwd=project,
                                    text=True, capture_output=True, timeout=60)
            print(result.stdout, end="")
            print(result.stderr, end="")
            result.check_returncode()
            # ATest reports failures without changing the program's exit status.
            summary = re.search(r"Summary: (\d+) passed, (\d+) failed", result.stdout)
            assert summary and int(summary[1]) > 0 and int(summary[2]) == 0, result.stdout


if __name__ == "__main__":
    main()
