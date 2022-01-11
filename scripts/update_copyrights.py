#!/usr/bin/env python3
"""
This script will automatically update the copyright banner on all of libtcod's
source files using LICENSE.txt as a reference.
"""
from __future__ import annotations

import argparse
import datetime
import os
import re
from pathlib import Path

parser = argparse.ArgumentParser(description="Updates the copyright banners in this project.")
parser.add_argument("-n", "--dry-run", action="store_true", help="Don't modify files.")
parser.add_argument("-v", "--verbose", action="store_true", help="Print debug information.")

PROJECT_DIR = Path(__file__, "../..")  # Project directory relative to this script.
LICENSE_FILE = PROJECT_DIR / "LICENSE.txt"
RE_SOURCE_FILE = re.compile(r".*\.[ch].?.?$")


def get_copyright(args: argparse.Namespace) -> str:
    """Update the year and return the copyright as a banner."""
    copyright = LICENSE_FILE.read_text(encoding="utf-8")
    copyright = re.sub(
        pattern=r"(?<=Copyright © \d\d\d\d-)\d+",
        repl=str(datetime.date.today().year),
        string=copyright,
    )
    if not args.dry_run:
        LICENSE_FILE.write_text(copyright, encoding="utf-8")

    banner = "\n * ".join(copyright.strip().split("\n"))
    banner = "\n".join(line.rstrip() for line in banner.split("\n"))
    return "/* " + banner + "\n */\n"


def update_file(path: Path, copyright: str, args: argparse.Namespace):
    """Adds or replaces the copyright banner for a source file.

    `path` is the file path.
    `copyright` is the up-to-date copyright banner.
    """
    path = path.resolve()
    source = path.read_text(encoding="utf-8")
    match = re.match(
        pattern=r"(/\*.*?Copyright ©.*?\*/\n?)?\n*(.*)",
        string=source,
        flags=re.DOTALL | re.MULTILINE,
    )
    assert match
    old_copyright, source = match.groups()
    if old_copyright == copyright:
        if args.verbose:
            print(f"Banner is up-to-date {path}")
        return
    elif old_copyright is None:
        print(f"Adding missing banner to {path}")
    else:
        print(f"Updating banner for {path}")

    if not args.dry_run:
        path.write_text(copyright + source, encoding="utf-8")


def main() -> None:
    args = parser.parse_args()

    copyright = get_copyright(args)
    # This avoids touching vendor files.
    for file in os.listdir(PROJECT_DIR / "src"):
        if RE_SOURCE_FILE.match(file):
            update_file(PROJECT_DIR / "src" / file, copyright, args)
    for dirpath, _, filenames in os.walk(PROJECT_DIR / "src/libtcod"):
        for file in filenames:
            if RE_SOURCE_FILE.match(file):
                update_file(Path(dirpath, file), copyright, args)


if __name__ == "__main__":
    main()
