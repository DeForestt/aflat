#!/usr/bin/env python3
"""Run the imported stdlib raw_buffer tests: python3 test/runtime/raw_buffer/run.py."""
from pathlib import Path
import re
import shutil
import subprocess
import tempfile


HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]


def main():
    with tempfile.TemporaryDirectory(prefix="aflat-raw-buffer-") as temporary:
        project = Path(temporary)
        (project / "src").mkdir()
        (project / "bin").mkdir()
        shutil.copyfile(HERE / "main.af", project / "src/main.af")
        (project / "aflat.cfg").write_text("[build]\nmain = main\n")
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
        for capacity, diagnostic in [
            (-1, "Buffer capacity must not be negative"),
            (2147483647, "Buffer capacity exceeds byte-size limit"),
        ]:
            (project / "src/main.af").write_text(
                '.needs <std>\nimport raw_buffer from "Collections/raw_buffer";\n'
                'fn main() -> int {\n'
                f'    let buffer = raw_buffer::<int>({capacity});\n'
                '    return 0;\n};\n')
            subprocess.run([ROOT / "bin/aflat", "--no-cache", "build"],
                           cwd=project, check=True, timeout=60)
            invalid = subprocess.run([project / "bin/a.out"], cwd=project,
                                     text=True, capture_output=True, timeout=10)
            assert invalid.returncode != 0, "invalid capacity was accepted"
            assert diagnostic in invalid.stdout + invalid.stderr, invalid
        print("Constructor checks: negative and overflowing capacities rejected")


if __name__ == "__main__":
    main()
