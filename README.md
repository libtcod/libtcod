# Table of contents #

   * [Table of contents](#table-of-contents)
   * [Introduction](#introduction)
   * [What can it do?](#what-can-it-do)
   * [How do I get set up?](#how-do-i-get-set-up)
      * [Using one of our downloads](#using-one-of-our-downloads)
      * [Compiling from source](#compiling-from-source)
   * [Getting Started](#getting-started)

# Introduction #

libtcod is a free, fast, portable and uncomplicated API for roguelike
developers providing an advanced true color console, input, and lots of other
utilities frequently used in roguelikes.

Status:
[![Build status](https://ci.appveyor.com/api/projects/status/pemepxo2221f8heo/branch/master?svg=true)](https://ci.appveyor.com/project/HexDecimal/libtcod-6e1jk/branch/master)
[![Build Status](https://travis-ci.org/libtcod/libtcod.svg?branch=master)](https://travis-ci.org/libtcod/libtcod)
[![Documentation Status](https://readthedocs.org/projects/libtcod/badge/?version=latest)](https://libtcod.readthedocs.io/en/latest/?badge=latest)

# What can it do? #

If you want to get a quick overview of the features that libtcod provides,
check out the [Features](https://bitbucket.org/libtcod/libtcod/wiki/Features)
page.

# How do I get set up? #

## Using one of our downloads ##

If you are programming in C or C++ then
Windows and MacOS binaries are available from the
[GitHub Releases page](https://github.com/libtcod/libtcod/releases).

Various C/C++ sample projects are included within
the repository which can be used as examples of various features.

Keep in mind that as a C++14 library, you will need to distribute the
appropriate runtime with your program such as the
[Visual Studio 2015 runtimes](https://www.microsoft.com/en-us/download/details.aspx?id=53587)
or else the program will fail to run.  This is required even when using Python.

For those who wish to program in Python you can install python-tcod using
[this installation guide](https://python-tcod.readthedocs.io/en/latest/installation.html).
Once installed you can follow
[the Python 3 tutorial](http://rogueliketutorials.com/tutorials/tcod/)
or you can find Python example scripts on the
[python-tcod repository](https://github.com/libtcod/python-tcod).

A alternative version of the Python library (libtcodpy) exists in the binary
downloads, but you should avoid using this version as it is not as well
maintained.

## Compiling from source ##

You can clone with Git via the
[GitHub repository](https://github.com/libtcod/libtcod).
You can then checkout a specific tag or get the in-development version from
the master branch.

All compilation methods, currently including
SCons (Windows, Linux, MacOS) and Autotools (Linux, MacOS), are located within
the `buildsys/` subdirectory.

SCons automatically downloads SDL2 and can be used on all platforms.
Instructions are
[provided here](https://github.com/libtcod/libtcod/tree/master/buildsys/scons).
The current release builds are built using SCons.

Autotools is a common standard on Linux, and can be used for MacOS.
Instructions are
[provided here](https://github.com/libtcod/libtcod/tree/master/buildsys/autotools).

Conan, the first that you should be is install conan: `sudo pip3 install conan`,
the second step is added a remote used for libtcod: 
`conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan`,
the third step is use conan in the project: 
`conan install . -s build_type=Debug --install-folder=cmake-build-debug`,
the fourth step is: `cd cmake-build-debug` and then: `cmake ..`, finally: `make`


# Getting Started #

The latest documentation is [here](https://libtcod.readthedocs.io/en/latest).
Currently it's very incomplete, so most people will want to read the
[1.6.4 documentation](https://libtcod.github.io/docs/index2.html?c=true&cpp=true&cs=false&py=false&lua=false)
instead.
Python users should use the
[python-tcod documentation](http://python-tcod.readthedocs.io).

libtcod comes with a sample application, implementations of which are provided
in each of
C ([samples_c.c](https://github.com/libtcod/libtcod/blob/master/samples/samples_c.c)),
C++ ([samples_cpp.cpp](https://github.com/libtcod/libtcod/blob/master/samples/samples_cpp.cpp)),
and Python ([samples_py.py](https://github.com/libtcod/python-tcod/blob/master/examples/samples_tcod.py)).
This provides a decent overview of the basic features, in an interactive
fashion.
Each should be identical for the most part, so if you are using Windows,
downloading the pre-compiled binaries and running the included `samples.exe`
which is compiled from the C source code, should be representative of the other
versions.
