#!/usr/bin/env python
"""
This script will automatically update the copyright banner on all of libtcod's
source files using LIBTCOD-LICENSE.txt as a reference.
"""
import os

import datetime
import io
import re

LICENSE_FILE = 'LIBTCOD-LICENSE.txt'
RE_SOURCE_FILE = re.compile(r'.*\.[ch].?.?$')

# Change to the root libtcod directory.
os.chdir(os.path.join(os.path.dirname(__file__), '..'))

def get_copyright():
    """Update the year and return the copyright as a string."""
    with io.open(LICENSE_FILE, 'r', encoding='utf-8') as f:
        copyright = f.read()
    copyright = re.sub(
        pattern=r'(?<=Copyright © \d\d\d\d-)\d+',
        repl=str(datetime.date.today().year),
        string=copyright,
    )
    with io.open(LICENSE_FILE, 'w', encoding='utf-8') as f:
        f.write(copyright)
    return copyright


def update_file(path, copyright):
    """Adds or replaces the copyright banner for a source file.

    `path` is the file path.
    `copyright` is the up-to-date copyright banner.
    """
    with io.open(path, 'r', encoding='utf-8') as f:
        source = f.read()
    match = re.match(
        pattern=r'(/\*.*?libtcod.*?Copyright.*?\*/\n?)?\n*(.*)',
        string=source,
        flags=re.DOTALL | re.MULTILINE,
    )
    old_copyright, source = match.groups()
    if old_copyright == copyright:
        return # Banner is already correct.
    elif old_copyright is None:
        print('Adding missing banner to %r' % (path,))
    else:
        print('Updating banner for %r' % (path,))

    with io.open(path, 'w', encoding='utf-8') as f:
        f.write(copyright)
        f.write(source)


def main():
    copyright = get_copyright()
    # This avoids touching vendor files.
    for file in os.listdir('src/'):
        if RE_SOURCE_FILE.match(file):
            update_file(os.path.join('src/', file), copyright)
    for dirpath, _, filenames in os.walk('src/libtcod'):
        for file in filenames:
            if RE_SOURCE_FILE.match(file):
                update_file(os.path.join(dirpath, file), copyright)


if __name__ == '__main__':
    main()
