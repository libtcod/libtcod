#!/usr/bin/env python3
"""Tags and releases the next version of this project."""

from __future__ import annotations

import argparse
import datetime
import json
import os
import re
import subprocess
import sys
from pathlib import Path
from typing import Final

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("tag", help="Semantic version number to use as the tag.")
parser.add_argument("-e", "--edit", action="store_true", help="Force edits of git commits.")
parser.add_argument("-n", "--dry-run", action="store_true", help="Don't modify files.")
parser.add_argument("-v", "--verbose", action="store_true", help="Print debug information.")

PROJECT_DIR = Path(__file__).parent.parent  # Project directory relative to this script.
CHANGELOG_PATH = PROJECT_DIR / "CHANGELOG.md"


def parse_changelog(args: argparse.Namespace) -> tuple[str, str]:
    """Return an updated changelog and and the list of changes."""
    match = re.match(
        pattern=r"(.*?## \[Unreleased]\n)(.+?\n)(\n*## \[.*)",
        string=CHANGELOG_PATH.read_text(encoding="utf-8"),
        flags=re.DOTALL,
    )
    assert match
    header, changes, tail = match.groups()
    tagged = f"\n## [{args.tag}] - {datetime.date.today().isoformat()}\n{changes}"  # noqa: DTZ011
    if args.verbose:
        print(tagged)

    return f"{header}{tagged}{tail}", changes


VERSION_SOURCE = """\
{head}
#define TCOD_MAJOR_VERSION {major}
#define TCOD_MINOR_VERSION {minor}
#define TCOD_PATCHLEVEL {patch}

#define TCOD_STRVERSION "{tag}"
{tail}"""


def update_version_header(args: argparse.Namespace) -> None:
    """Update version.h to use the given tag."""
    version_header_path: Final = Path("src/libtcod/version.h")
    match = re.match(
        pattern=r"(?P<head>.*?)\n#define TCOD_MAJOR_VERSION.*?TCOD_STRVERSION[^\n]*\n(?P<tail>.*)",
        string=version_header_path.read_text(encoding="utf-8"),
        flags=re.DOTALL,
    )
    assert match
    format_args = {"tag": args.tag}
    format_args.update(match.groupdict())

    match = re.match(
        pattern=r"(?P<major>[0-9]+)\.(?P<minor>[0-9]+)\.(?P<patch>[0-9]+)",
        string=args.tag,
    )
    assert match
    format_args.update(match.groupdict())

    source = VERSION_SOURCE.format(**format_args)

    if not args.dry_run:
        version_header_path.write_text(source, encoding="utf-8")


def update_vcpkg_manifest(args: argparse.Namespace) -> None:
    """Update the version on the vcpkg.json manifest."""
    vcpkg_path: Final = Path("vcpkg.json")
    with vcpkg_path.open(encoding="utf-8") as f:
        vcpkg_manifest = json.load(f)
    vcpkg_manifest["version-semver"] = args.tag

    if not args.dry_run:
        with vcpkg_path.open("w", encoding="utf-8") as f:
            json.dump(vcpkg_manifest, f, indent=2)


def replace_unreleased_tags(tag: str, path: Path, *, dry_run: bool) -> None:
    """Replace "unreleased" version tags with the provided tag.

    Such as the following examples:
        ".. versionadded:: unreleased" -> ".. versionadded:: <tag>"
        "@versionadded{Unreleased}" -> "@versionadded{<tag>}"
    """
    match = re.match(r"\d+\.\d+", tag)  # Get "major.minor" version.
    assert match
    short_tag = match.group()
    for directory, _, files in os.walk(path):
        for filename in files:
            file = Path(directory, filename)
            if file.suffix not in {".c", ".cpp", ".h", ".hpp"}:
                continue
            text = file.read_text(encoding="utf-8")
            new_text = re.sub(
                r":: *unreleased",
                rf":: {short_tag}",
                text,
                flags=re.IGNORECASE,
            )
            new_text = re.sub(
                r"@versionadded{ *unreleased *}",
                rf"@versionadded{{{short_tag}}}",
                new_text,
                flags=re.IGNORECASE,
            )
            if text != new_text:
                print(f"Update tags in {file}")
                if not dry_run:
                    file.write_text(new_text, encoding="utf-8")


def main() -> None:
    """Main script."""
    if len(sys.argv) == 1:
        parser.print_help(sys.stderr)
        sys.exit(1)

    args = parser.parse_args()
    if args.verbose:
        print(args)

    update_version_header(args)
    new_changelog, changes = parse_changelog(args)

    update_vcpkg_manifest(args)

    replace_unreleased_tags(args.tag, PROJECT_DIR / "src", dry_run=args.dry_run)

    if not args.dry_run:
        CHANGELOG_PATH.write_text(new_changelog, encoding="utf-8")
        edit = ["-e"] if args.edit else []
        subprocess.check_call(
            ["git", "commit", "-avm", f"Prepare {args.tag} release.", *edit],  # noqa: S603, S607
        )
        subprocess.check_call(
            ["git", "tag", args.tag, "-a", "-m", f"{args.tag}\n\n{changes}", *edit],  # noqa: S603, S607
        )


if __name__ == "__main__":
    main()
