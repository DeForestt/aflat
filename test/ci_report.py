#!/usr/bin/env python3
"""Echo interesting lines of a test log as ::error:: annotations.

Usage: ci_report.py LOGFILE TITLE

GitHub turns ::error:: lines printed by a step into check-run
annotations, which stay readable via the API in environments where
Actions logs are not. Only lines mentioning FAILED or the Catch2
summary are echoed, capped at a handful.
"""
import sys


def esc(s: str) -> str:
    return (s.replace("%", "%25").replace("\r", "%0D")
             .replace("\n", "%0A"))


def main() -> int:
    path, title = sys.argv[1], sys.argv[2]
    try:
        lines = open(path, errors="replace").read().splitlines()
    except OSError as exc:
        print(f"::error title={title}::cannot read log: {exc}")
        return 0
    picked = [ln for ln in lines
              if "FAILED" in ln or "test cases:" in ln][:12]
    if not picked:
        picked = [ln for ln in lines if ln.strip()][-6:]
    for ln in picked:
        print(f"::error title={title}::{esc(ln[:400])}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
