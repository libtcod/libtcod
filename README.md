# Introduction

libtcod is a free, fast, portable and uncomplicated API for roguelike developers providing a true color console, pathfinding, field-of-view, and a few other utilities frequently used in roguelikes.

## Status

[![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/libtcod/libtcod/builds.yml)](https://github.com/libtcod/libtcod/actions)
[![Documentation Status](https://readthedocs.org/projects/libtcod/badge/?version=latest)](https://libtcod.readthedocs.io/en/latest/?badge=latest)
[![codecov](https://codecov.io/gh/libtcod/libtcod/branch/main/graph/badge.svg?token=pmHy3jXemj)](https://codecov.io/gh/libtcod/libtcod)
[![GitHub commits since latest release (by date)](https://img.shields.io/github/commits-since/libtcod/libtcod/latest)](https://github.com/libtcod/libtcod/blob/main/CHANGELOG.md)
[![Packaging status](https://repology.org/badge/tiny-repos/libtcod.svg)](https://repology.org/project/libtcod/versions)

# How do I get set up?

## Using Vcpkg

This is the easiest way to get the latest stable version of libtcod for any project.

Libtcod is included as a port in [Vcpkg](https://github.com/microsoft/vcpkg).
You can install libtcod via Vcpkg and then link the library using a [CMake](https://cmake.org/) script as you normally do for that package manager.

## As a submodule

This is the best option for testing the development versions of libtcod.

You can include libtcod in a project by adding the libtcod repository as a [submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules) and then adding that directory to a [CMake](https://cmake.org/) script.
You will want to fork [this template project](https://github.com/HexDecimal/libtcod-vcpkg-template) if you plan on starting a project with this setup.

It is expected that Vcpkg will be used, but libtcod's CMake script can be configured to compile without using Vcpkg for dependencies.
See [CONTRIBUTING.md](CONTRIBUTING.md) for details on configuring dependencies.

## Using one of our downloads

This is not recommend as these releases are unwieldy, are more difficult to update, and are less cross-platform.
*Do not upload binary files to your projects source repository.*

If you are programming in C or C++ then
Windows and MacOS binaries are available from the
[GitHub Releases page](https://github.com/libtcod/libtcod/releases).

Various C/C++ sample projects are included within
the repository which can be used as examples of various features.

Keep in mind that as a C++ library, you may need to distribute the
appropriate runtime with your program such as the
[Visual Studio 2015 runtimes](https://www.microsoft.com/en-us/download/details.aspx?id=53587)
or else the program will fail to run.

For those who wish to program in Python you can install python-tcod using
[this installation guide](https://python-tcod.readthedocs.io/en/latest/installation.html).
Once installed you can follow
[the Python 3 tutorial](http://rogueliketutorials.com/)
or you can find Python example scripts on the
[python-tcod repository](https://github.com/libtcod/python-tcod).

A alternative version of the Python library (libtcodpy) exists in the binary
downloads, but you should avoid using this version as it is not as well
maintained.

## Compiling from source

This is only recommended for libtcod developers.
See [CONTRIBUTING.md](CONTRIBUTING.md).

# Getting Started

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

# Contact / Community

For reporting bugs or requesting features you should use the [GitHub Issues page](https://github.com/libtcod/libtcod/issues).
For smaller questions or help with a tutorial or project you can join ``#libtcod`` on the [Roguelikes Discord](https://discord.gg/jEgZtqB) or on the [Libera.​Chat](https://libera.chat/) IRC.

# Sponsors

[Sponsors donating $25 or more per month](https://github.com/sponsors/HexDecimal) will have their names or logos listed here.

* [q00u](https://github.com/q00u)
* [amaya30](https://github.com/amaya30)
