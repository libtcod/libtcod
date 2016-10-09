[TOC]

## Introduction ##

libtcod is a free, fast, portable and uncomplicated API for roguelike developers providing an advanced true color console, input, and lots of other utilities frequently used in roguelikes.

1.6: [![Build status](https://ci.appveyor.com/api/projects/status/6jh07hq205iy0mlh/branch/default?svg=true)](https://ci.appveyor.com/project/rmtew/libtcod/branch/default)

1.5: COMING.

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

### Compiling from source ###

These are the recommended places to obtain the source code from:

* The latest source code binary on the [downloads page](https://bitbucket.org/libtcod/libtcod/downloads).
* Clone the repository with Mercurial and checkout the tag for the latest version in the default branch.
* Clone the repository and checkout the bleeding edge from the tip in the default branch. 

All compilation methods, whether Windows or autotools, are located within the `build/` subdirectory.

Windows users can either runn the top-level 'build.bat' script, or locate and open the provided solution directly using Visual Studio.  The solution is currently for Visual Studio 2015, and the [free community edition](https://www.visualstudio.com/vs/community/) is highly recommended.

For all other platforms, you need to make use of the `autotools` support.  This is a standard approach used for decades, and if you are not familiar with it, you can Bing (or Google) the topic and learn more. For example, to build a production library :

    cd build/autotools
    autoreconf -i # if building from hg
    ./configure CFLAGS='-O2'
    make

#### SDL2 ####

[SDL2](http://hg.libsdl.org/SDL) is the latest version of SDL.  While the latest code for libtcod retains the support for SDL, it is not recommended you use it.  The SDL support will eventually be removed, and if you choose to use it, you will be incurring extra work for you further down the road.

The SDL2 revision e12c38730512 is the only one that is supported by this project at this time.  If you are using any other revision, try the supported revision before reporting problems.  This is to ensure that we are all dealing with the same SDL2 bugs, otherwise your problems may not be reproducible when reported.

## Getting Started ##

libtcod comes with a sample application, implementations of which are provided in each of C ([samples_c.c](https://bitbucket.org/libtcod/libtcod/src/tip/samples/samples_c.c?at=default)), C++ ([samples_cpp.cpp](https://bitbucket.org/libtcod/libtcod/src/tip/samples/samples_cpp.cpp?at=default)) and Python ([samples_py.py](https://bitbucket.org/libtcod/libtcod/src/tip/python/samples_py.py?at=default)).  This provides a decent overview of the basic features, in an interactive fashion.  Each should be identical, so if you are using Windows, downloading the precompiled binaries and running the included `samples.exe` which is compiled from the C source code, should be representative of the other versions.

The popular [Python tutorial](http://www.roguebasin.com/index.php?title=Complete_Roguelike_Tutorial,_using_python%2Blibtcod) is a good way to both build up a simple roguelike, and get familiar with the basics of libtcod.