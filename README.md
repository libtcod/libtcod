[TOC]

## Introduction ##

libtcod is a free, fast, portable and uncomplicated API for roguelike developers providing an advanced true color console, input, and lots of other utilities frequently used in roguelikes.

1.6.3 (pre-release): [![Build status](https://ci.appveyor.com/api/projects/status/6jh07hq205iy0mlh/branch/default?svg=true)](https://ci.appveyor.com/project/rmtew/libtcod/branch/default)

## What can it do? ##

If you want to get a quick overview of the features that libtcod provides, check out the [Features](https://bitbucket.org/libtcod/libtcod/wiki/Features) page.

If you want to get a quick overview of games which have used libtcod, check out the [Projects](http://roguecentral.org/doryen/projects-2/) page.

libtcod is also well known for it's [easy to follow tutorial](http://www.roguebasin.com/index.php?title=Complete_Roguelike_Tutorial,_using_python%2Blibtcod), which many people have used to get started developing a roguelike.  Follow the relevant links the tutorial gives, and it will point you to the right locations.

## How do I get set up? ##

### Using one of our downloads ###

Currently, only Windows binaries are available from the Bitbucket [download section](https://bitbucket.org/libtcod/libtcod/downloads) for this project.  If you are programming in C or C++, various sample projects are included within the source code which can be used as examples of various features.  For those who wish to program in Python which is the only scripting language support that has been contributed so far, the basics are described in [part 1](http://www.roguebasin.com/index.php?title=Complete_Roguelike_Tutorial,_using_python%2Blibtcod,_part_1#Setting_it_up) of the tutorial.

Two builds are currently provided for each release:

* 32 bit (Win32, SDL2 only) binaries also including source code.
* 64 bit (x64, SDL2 only) binaries also including source code.

**Warning:** If you download these builds, run `samples.exe` as your very first action.  If you are unable to and get an error about `vcruntime140.dll` being missing, you need to install the two [Visual Studio 2015 runtimes](https://www.microsoft.com/en-us/download/details.aspx?id=53587).  Make sure you install the 32-bit runtime.  And make sure you also install the 64-bit runtime.  Then run `samples.exe` again, and it should now work.  Whether you plan to develop in C, C++, Python or some other language, this is required.

### Compiling from source ###

These are the recommended places to obtain the source code from:

* The latest source code release snapshot on the [downloads page](https://bitbucket.org/libtcod/libtcod/downloads).
* Clone the repository with Mercurial and checkout the tag for the latest version in the `default` branch.
* Clone the repository and checkout the bleeding edge from the tip in the default branch.

All compilation methods, currently including Visual Studio (Windows), SCons (Windows, Linux, MacOS) and autotools (Linux, MacOS), are located within the `build/` subdirectory.

Windows users who prefer to use Visual Studio directly can either run the top-level 'build.bat' script, or locate and open the provided solution directly using Visual Studio.  The solution is currently for Visual Studio 2015, and the [free community edition](https://www.visualstudio.com/vs/community/) is highly recommended.

Autotools is a standard approach used for decades, and if you are not familiar with it, you can Bing (or Google) the topic and learn more.  Explicit instructions for Linux, which should be relevant, are [provided here](README-linux-SDL2.md).  SCons is another popular approach, and explicit instructions are [provided here](https://bitbucket.org/libtcod/libtcod/src/tip/build/scons/?at=default) for those who prefer.

MacOS users with a working Homebrew installation, can install the necessary dependencies with `brew install autoconf automake libtool pkg-build sdl2`. To build from the current development version of libtcod (rather than downloading a source package), `brew install mercurial`. Then follow [the directions](README-linux-SDL2.md) for building libtcod on Linux (just the final section). Note that the actual libraries will be in libtcod/build/autotools/.lib, with several symlinks.

#### SDL2 ####

[SDL2](http://hg.libsdl.org/SDL) is the latest version of SDL.  Release 2.0.5 (changeset 007dfe83abf8) of SDL2, is currently used for the official Windows builds.  If you choose to use a different version of SDL2, please be sure to mention it in any issues you create.  And if you are using the bleeding edge (the latest unreleased/untagged changes), please rule out that this is the cause of any problems you encounter, before creating issues.

## Getting Started ##

The latest documentation is updated with every significant change made to libtcod.  You can find it in [the doc directory](https://bitbucket.org/libtcod/libtcod/src/tip/doc/index2.html?at=default&fileviewer=file-view-default).  Similarly, [the changelog]()https://bitbucket.org/libtcod/libtcod/src/tip/libtcod-CHANGELOG.txt?at=default provides a high level overview of the things you might need to be aware of when later updating libtcod.

libtcod comes with a sample application, implementations of which are provided in each of C ([samples_c.c](https://bitbucket.org/libtcod/libtcod/src/tip/samples/samples_c.c?at=default)), C++ ([samples_cpp.cpp](https://bitbucket.org/libtcod/libtcod/src/tip/samples/samples_cpp.cpp?at=default)) and Python ([samples_py.py](https://bitbucket.org/libtcod/libtcod/src/tip/python/samples_py.py?at=default)).  This provides a decent overview of the basic features, in an interactive fashion.  Each should be identical for the most part, so if you are using Windows, downloading the precompiled binaries and running the included `samples.exe` which is compiled from the C source code, should be representative of the other versions.

The popular [Python tutorial](http://www.roguebasin.com/index.php?title=Complete_Roguelike_Tutorial,_using_python%2Blibtcod) is a good way to both build up a simple roguelike, and get familiar with the basics of libtcod.