#!/usr/bin/env python
"""
    This script is used as a workaround to a bug in SCons.

    https://github.com/SCons/scons/issues/3136

    urlretrieve must be called from a seperate Python process on some
    platforms.
"""
import sys

try:
    from urllib.request import urlretrieve
except ImportError: # Python 2
    from urllib import urlretrieve

if __name__ == '__main__':
    urlretrieve(*sys.argv[1:])
