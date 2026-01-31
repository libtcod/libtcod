#!/usr/bin/env python3
"""Print the libtcod version number."""

import re
from pathlib import Path
import argparse

THIS_DIR = Path(__file__).parent.resolve(strict=True)
VERSION_HEADER = Path(THIS_DIR, "../../src/libtcod/version.h").resolve(strict=True)

RE_MAJOR = r".*#define TCOD_MAJOR_VERSION\s+([0-9]+)"
RE_MINOR = r".*#define TCOD_MINOR_VERSION\s+([0-9]+)"
RE_PATCH = r".*#define TCOD_PATCHLEVEL\s+([0-9]+)"
RE_VERSION = re.compile(RE_MAJOR + RE_MINOR + RE_PATCH, flags=re.DOTALL)

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument(
    "--soversion",
    "--so",
    action="store_true",
    help="print the shared object version number instead",
)


def main() -> None:
    """Fetch and print the requested version number."""
    args = parser.parse_args()
    version_match = RE_VERSION.match(VERSION_HEADER.read_text(encoding="utf-8"))
    assert version_match
    major, minor, patch = version_match.groups()
    if args.soversion:
        print(f"{major}:{minor}")  # libtcod versions are not ABI compatible
        return
    print(f"{major}.{minor}.{patch}")


if __name__ == "__main__":
    main()
