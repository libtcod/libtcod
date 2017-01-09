Building Libtcod 1.6 on Linux with SDL2
=======================================

The following instructions have been tested on 32 and 64-bit versions of Ubuntu 14.04 and Fedora 22.

Dependencies
------------

For Ubuntu 14.04, install these dependencies:

    $ sudo apt-get install curl build-essential make cmake autoconf automake libtool mercurial libasound2-dev libpulse-dev libaudio-dev libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxi-dev libxinerama-dev libxxf86vm-dev libxss-dev libgl1-mesa-dev libesd0-dev libdbus-1-dev libudev-dev libgles1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev

For Fedora 22:

    $ sudo dnf -v groupinstall "C Development Tools and Libraries"
    $ sudo dnf install mercurial alsa-lib-devel audiofile-devel mesa-libGL-devel mesa-libGLU-devel mesa-libEGL-devel mesa-libGLES-devel libXext-devel libX11-devel libXi-devel libXrandr-devel libXrender-devel dbus-devel libXScrnSaver-devel libusb-devel pulseaudio-libs-devel libXinerama-devel libXcursor-devel systemd-devel


Building SDL2
-------------

It is recommended strongly that you install SDL2 using your package manager.  However, if you are unable to work out the package name, then you can take the harder route and build it yourself.

Download the supported SDL2 revision, build and install it if you must:

    $ curl -o sdl.tar.gz http://hg.libsdl.org/SDL/archive/007dfe83abf8.tar.gz
    $ tar -xf sdl.tar.gz
    $ cd SDL-007dfe83abf8/
    $ mkdir -p build
    $ cd build
    $ ../configure
    $ make
    $ sudo make install

This will place the libraries at `/usr/local/lib/` and the development headers at `/usr/local/include/SDL2/`.


Building Libtcod 1.6
--------------------
Download the latest libtcod version, build it and install it:

    $ hg clone https://bitbucket.org/libtcod/libtcod
    $ cd libtcod/build/autotools
	$ autoreconf -i # if building from hg
    $ ./configure CFLAGS='-O2'
    $ make

This will place the libraries in the top level of the libtcod checkout directory.

Note that the same makefile is used for 32 and 64 bit distributions.
