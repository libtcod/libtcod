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

# What can it do? #

If you want to get a quick overview of the features that libtcod provides, check out the [Features](https://bitbucket.org/libtcod/libtcod/wiki/Features) page.

If you want to get a quick overview of games which have used libtcod, check out the [Projects](http://roguecentral.org/doryen/projects-2/) page.

libtcod is also well known for it's [easy to follow tutorial](http://www.roguebasin.com/index.php?title=Complete_Roguelike_Tutorial,_using_python%2Blibtcod), which many people have used to get started developing a roguelike.  Follow the relevant links the tutorial gives, and it will point you to the right locations.

# How do I get set up? #

## Using one of our downloads ##

Windows and MacOS binaries are available from the
[GitHub Releases page](https://github.com/libtcod/libtcod/releases).

If you are programming in C or C++, various sample projects are included within
the repository which can be used as examples of various features.

For those who wish to program in Python which is the only scripting language
support that has been contributed so far, the basics are described in
[part 1](http://www.roguebasin.com/index.php?title=Complete_Roguelike_Tutorial,_using_python%2Blibtcod,_part_1#Setting_it_up)
of the tutorial.
It's currently recommended to install libtcodpy via
[python-tdl](https://github.com/HexDecimal/python-tdl) rather than using the
libtcodpy package included in this repository.

**Warning:** If you download these builds, run `samples.exe` as your very first
action.
If you are unable to and get an error about `vcruntime140.dll` being missing,
you need to install the two
[Visual Studio 2015 runtimes](https://www.microsoft.com/en-us/download/details.aspx?id=53587).
Make sure you install the 32-bit runtime.
And make sure you also install the 64-bit runtime.
Then run `samples.exe` again, and it should now work.
Whether you plan to develop in C, C++, Python or some other language, this is
required.

## Compiling from source ##

You can clone with Mercurial via the
[BitBucket repository](https://bitbucket.org/libtcod/libtcod), or with Git via
the [GitHub repository](https://github.com/libtcod/libtcod).
You can then checkout a specific tag or get the in-development version from
the default or master branches.

All compilation methods, currently including Visual Studio (Windows),
SCons (Windows, Linux, MacOS) and Autotools (Linux, MacOS), are located within
the `build/` subdirectory.

Windows users who prefer to use Visual Studio directly can either run the
top-level 'build.bat' script, or locate and open the provided solution directly
using Visual Studio.
The solution is currently for Visual Studio 2015, and the
[free community edition](https://www.visualstudio.com/vs/community/) is highly
recommended.

SCons automatically downloads SDL2 and can be used on all platforms.
Instructions are
[provided here](https://github.com/libtcod/libtcod/tree/master/build/scons).
The current release builds are built using SCons.

Autotools is a common standard on Linux, and can be used for MacOS.
Instructions are
[provided here](https://github.com/libtcod/libtcod/tree/master/build/autotools).

If you ever need to statically compile libtcod the easiest method would be to
use the `src/libtcod_c.c` and `src/libtcod.cpp` source files, which include all
the other sources.  Further instructions are included in those files.

# Getting Started #

The latest documentation is [here](https://libtcod.readthedocs.io/en/latest).
Currently it's very incomplete, so most people will want to read the
[1.5.1 documentation](http://roguecentral.org/doryen/data/libtcod/doc/1.5.1/index2.html?c=true&cpp=true&cs=false&py=true&lua=false)
instead.

libtcod comes with a sample application, implementations of which are provided
in each of
C ([samples_c.c](https://bitbucket.org/libtcod/libtcod/src/tip/samples/samples_c.c?at=default)),
C++ ([samples_cpp.cpp](https://bitbucket.org/libtcod/libtcod/src/tip/samples/samples_cpp.cpp?at=default)),
and Python ([samples_py.py](https://bitbucket.org/libtcod/libtcod/src/tip/python/samples_py.py?at=default)).
This provides a decent overview of the basic features, in an interactive
fashion.
Each should be identical for the most part, so if you are using Windows,
downloading the pre-compiled binaries and running the included `samples.exe`
which is compiled from the C source code, should be representative of the other
versions.
