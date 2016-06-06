Building Libtcod 1.6 on Linux with SDL2
=======================================

Due to limited resources, libtcod 1.6 is only going to support SDL2. In addition, the last verified as stable version is the SDL2 commit `704a0bfecf75`. To build libtcod, we recommend you build this version of SDL2.

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
Download the supported SDL2 revision, build and install it:

    $ curl -o sdl.tar.gz http://hg.libsdl.org/SDL/archive/704a0bfecf75.tar.gz
    $ tar -xf sdl.tar.gz
    $ cd SDL-704a0bfecf75/
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
    $ ./configure CFLAGS='-O2'
    $ make

This will place the libraries in the top level of the libtcod checkout directory.

Note that the same makefile is used for 32 and 64 bit distributions.
