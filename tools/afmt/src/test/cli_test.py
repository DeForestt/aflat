"""Run with `python3 src/test/cli_test.py` after `aflat build`."""

from pathlib import Path
import os
import subprocess
import tempfile
import unittest


BINARY = Path(__file__).resolve().parents[2] / "bin" / "afmt"
SOURCE = b'fn main() {\nitems = [\n1,\n2\n];\n};\n'
FORMATTED = b'fn main() {\n    items = [\n        1,\n        2\n    ];\n};\n'


class OutputModeTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory(prefix="afmt-cli-")
        self.addCleanup(self.directory.cleanup)
        self.path = Path(self.directory.name) / "input file.af"
        self.path.write_bytes(SOURCE)

    def run_cli(self, *arguments):
        return subprocess.run(
            [str(BINARY), *map(str, arguments)],
            capture_output=True,
            timeout=10,
        )

    def test_default_prints_without_modifying_input(self):
        result = self.run_cli(self.path)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(result.stdout, FORMATTED)
        self.assertEqual(self.path.read_bytes(), SOURCE)

    def test_both_inplace_flags_before_and_after_path(self):
        for flag in ("--inplace", "-i"):
            for flag_first in (True, False):
                with self.subTest(flag=flag, flag_first=flag_first):
                    self.path.write_bytes(SOURCE)
                    arguments = (flag, self.path) if flag_first else (self.path, flag)
                    result = self.run_cli(*arguments)
                    self.assertEqual(result.returncode, 0, result.stderr)
                    self.assertEqual(result.stdout, b"")
                    self.assertEqual(self.path.read_bytes(), FORMATTED)
                    again = self.run_cli(*arguments)
                    self.assertEqual(again.returncode, 0, again.stderr)
                    self.assertEqual(again.stdout, b"")
                    self.assertEqual(self.path.read_bytes(), FORMATTED)

    def test_inplace_truncates_old_contents(self):
        self.path.write_bytes(b"                    value\n")
        result = self.run_cli("-i", self.path)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(result.stdout, b"")
        self.assertEqual(self.path.read_bytes(), b"value\n")

    def test_empty_files_fail_without_changes(self):
        self.path.write_bytes(b"")
        for flags in ((), ("--inplace",), ("-i",)):
            with self.subTest(flags=flags):
                result = self.run_cli(*flags, self.path)
                self.assertGreater(result.returncode, 0, result.stderr)
                self.assertIn(b"Failed to read from file", result.stdout + result.stderr)
                self.assertEqual(self.path.read_bytes(), b"")

    def test_help_does_not_modify_input(self):
        result = self.run_cli("--help", "--inplace", self.path)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn(b"--inplace", result.stdout)
        self.assertEqual(self.path.read_bytes(), SOURCE)

    def test_missing_path_fails_without_creating_a_file(self):
        missing = Path(self.directory.name) / "missing.af"
        result = self.run_cli("--inplace", missing)
        self.assertNotEqual(result.returncode, 0)
        self.assertFalse(missing.exists())

    @unittest.skipIf(os.geteuid() == 0, "root can write read-only files")
    def test_unwritable_input_fails_without_changing_it(self):
        self.path.chmod(0o444)
        self.addCleanup(self.path.chmod, 0o644)
        result = self.run_cli("--inplace", self.path)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn(b"Failed to", result.stdout + result.stderr)
        self.assertEqual(self.path.read_bytes(), SOURCE)


if __name__ == "__main__":
    unittest.main()
