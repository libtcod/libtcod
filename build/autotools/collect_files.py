#!/usr/bin/env python3
"""
Writes out the source and include files needed for AutoTools.

This script will update the collected_files.md file.
"""

import os

import re

BANNER = '# This file is automatically updated by running collect_files.py'

def collect():
    """Return an iterator of AutoMake commands.

    This might be run on Windows, but must return Unix line separators.
    """
    source_files = [
        '../../src/vendor/glad.c',
        '../../src/vendor/lodepng.cpp',
        '../../src/vendor/stb.c',
        '../../src/vendor/utf8proc/utf8proc.c',
    ]
    for curpath, dirs, files in os.walk('../../src/libtcod'):
        curpath = curpath.replace('\\', '/')
        relative_dir = os.path.relpath(curpath, '../../src')
        include_name = (relative_dir.replace('/', '_').replace('\\', '_')
                        + '_include')
        include_dir = '/'.join(['$(includedir)', relative_dir])
        include_files = ['/'.join([curpath, f]) for f in files
                         if f.endswith('.h') or f.endswith('.hpp')]
        source_files.extend('/'.join([curpath, f]) for f in files
                            if f.endswith('.c') or f.endswith('.cpp'))
        if '/.' in curpath[5:]:
            continue # Skip hidden directories.
        if not include_files:
            continue
        yield('%sdir = %s' % (include_name, include_dir))
        yield('%s_HEADERS = \\\n\t%s' % (include_name,
                                   ' \\\n\t'.join(include_files)))
        yield('')
    yield('libtcod_la_SOURCES = \\\n\t%s' % (' \\\n\t'.join(source_files),))

def main():
    with open('collected_files.am', 'w') as file:
        file.write(BANNER)
        file.write('\n\n')
        file.write('\n'.join(collect()))
        file.write('\n')

if __name__ == '__main__':
    main()
