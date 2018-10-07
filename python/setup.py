#!/usr/bin/env python

import sys
import os

from distutils.command.build import build as orig_build
from distutils.util import get_platform
from distutils import file_util
from setuptools import setup, Command

class build_make(Command):
    description = "run the makefile and include the libraries in the build"

    user_options = [('ignore-errors', 'i',
                     "ignore errors from makefile commands"),
                   ]

    boolean_options = ['ignore-errors']

    def initialize_options(self):
        self.build_lib = None
        self.ignore_errors = False
        self.makefile = None
        self.make_data = None # {package: files}
        place_dll_dir = 'libtcodpy'

        if 'linux' in sys.platform:
            self.makefile = 'makefiles/makefile-linux'
            self.make_data = {place_dll_dir: ['libtcod.so']}
        elif 'haiku' in sys.platform:
            self.makefile = 'makefiles/makefile-haiku'
            self.make_data = {place_dll_dir: ['libtcod.so']}
        elif 'win' in sys.platform:
            self.makefile = 'makefiles/makefile-mingw-sdl2'
            self.make_data = {place_dll_dir: ['libtcod-mingw.dll', 'SDL2.dll']}
        else:
            raise StandardError('No makefile exists for the %s platform' %
                                 sys.platform)

    def finalize_options(self):
        self.set_undefined_options('build',
                                   ('build_lib', 'build_lib'),)

    def run(self):
        cmd = ['make', '-f', self.makefile,'release']
        if self.ignore_errors:
            cmd += ['--ignore-errors']
        if self.dry_run:
            cmd += ['--dry-run']
        self.spawn(cmd)
        for directory, files in self.make_data.items():
            for file in files:
                self.copy_file(os.path.join('.', file),
                               os.path.join(self.build_lib, directory, file))

class build(orig_build):

    def initialize_options(self):
        'add the platform name to the build dir to prevent conflicts'
        orig_build.initialize_options(self)

        plat_name = self.plat_name or get_platform()
        self.build_lib = os.path.join(self.build_base, 'lib.%s' % plat_name)

    # add build_make as a subcommand
    sub_commands = orig_build.sub_commands + [('build_make', None)]

cmdclass={'build': build, 'build_make': build_make}

try:
    from wheel.bdist_wheel import bdist_wheel as orig_bdist_wheel

    class bdist_wheel(orig_bdist_wheel):
        """ctypes libraries are "platform specific" "pure Python" modules.

        There's no way to tell bdist_wheel this without overwriting its methods
        """

        description = "create a ctypes wheel distribution"

        def get_tag(self):
            # modified to add the platform tag to pure libraries
            # no other changes
            impl, abi, plat = orig_bdist_wheel.get_tag(self)

            plat_name = self.plat_name
            if plat_name is None:
                plat_name = get_platform()
            plat_name = plat_name.replace('-', '_').replace('.', '_')

            return (impl, abi, plat_name)

    cmdclass['bdist_wheel'] = bdist_wheel
except ImportError:
    pass

needs_pytest = {'pytest', 'test', 'ptr'}.intersection(sys.argv)
pytest_runner = ['pytest-runner'] if needs_pytest else []

setup(
    # public name, e.g. > pip install libtcod
    name='libtcod',

    # generic first release version
    version = '0.1.0',

    # package named to be compatible with the tutorial
    packages=['libtcodpy'],

    # use added and modified commands
    # disabled at this moment
    #cmdclass=cmdclass,

    # important metadata
    url = 'https://bitbucket.org/libtcod/libtcod',
    maintainer = '',
    maintainer_email = '',

    # used for test command
    setup_requires = pytest_runner,
    tests_require = ['pytest', 'pytest-cov'],

    # optional metadata for pypi
    description = '',
    license = 'Revised BSD License', # 3-clause BSD license
    )
