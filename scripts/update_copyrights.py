#!/usr/bin/env python3
"""
This script will automatically update the copyright banner on all of libtcod's
source files using LICENSE.txt as a reference.
"""
import os
import sys

import argparse
import datetime
import re
from typing import Any

parser = argparse.ArgumentParser(
    description='Updates the copyright banners in this project.',
)

parser.add_argument(
    '-n', '--dry-run',
    action='store_true',
    help="Don't modify files.",
)

parser.add_argument(
    '-v', '--verbose',
    action='store_true',
    help='Print debug information.',
)

LICENSE_FILE = 'LICENSE.txt'
RE_SOURCE_FILE = re.compile(r'.*\.[ch].?.?$')

# Change to the root libtcod directory.
os.chdir(os.path.join(os.path.dirname(__file__), '..'))

def get_copyright() -> str:
    """Update the year and return the copyright as a banner."""
    with open(LICENSE_FILE, 'r', encoding='utf-8') as f:
        copyright = f.read()
    copyright = re.sub(
        pattern=r'(?<=Copyright © \d\d\d\d-)\d+',
        repl=str(datetime.date.today().year),
        string=copyright,
    )
    with open(LICENSE_FILE, 'w', encoding='utf-8') as f:
        f.write(copyright)

    banner = "\n * ".join(copyright.strip().split("\n"))
    banner = "\n".join(line.rstrip() for line in banner.split("\n"))
    return "/* " + banner + "\n */\n"


def update_file(path: str, copyright: str, args: Any):
    """Adds or replaces the copyright banner for a source file.

    `path` is the file path.
    `copyright` is the up-to-date copyright banner.
    """
    with open(path, 'r', encoding='utf-8') as f:
        source = f.read()
    match = re.match(
        pattern=r'(/\*.*?Copyright ©.*?\*/\n?)?\n*(.*)',
        string=source,
        flags=re.DOTALL | re.MULTILINE,
    )
    assert match
    old_copyright, source = match.groups()
    if old_copyright == copyright:
        if args.verbose:
            print("Banner is up-to-date %r" % (path,))
        return
    elif old_copyright is None:
        print('Adding missing banner to %r' % (path,))
    else:
        print('Updating banner for %r' % (path,))

    if not args.dry_run:
        with open(path, 'w', encoding='utf-8') as f:
            f.write(copyright)
            f.write(source)


def main() -> None:
    args = parser.parse_args()

    copyright = get_copyright()
    # This avoids touching vendor files.
    for file in os.listdir('src/'):
        if RE_SOURCE_FILE.match(file):
            update_file(os.path.join('src/', file), copyright, args)
    for dirpath, _, filenames in os.walk('src/libtcod'):
        for file in filenames:
            if RE_SOURCE_FILE.match(file):
                update_file(os.path.join(dirpath, file), copyright, args)


if __name__ == '__main__':
    main()
