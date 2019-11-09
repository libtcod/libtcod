Building Libtcod using Autotools
================================

Dependencies
------------

For Ubuntu 16.04, install these dependencies:

    $ sudo apt install build-essential autoconf automake libtool git libsdl2-dev

For Fedora 22:

    $ sudo dnf -v groupinstall "C Development Tools and Libraries"
    $ sudo dnf install mercurial alsa-lib-devel audiofile-devel mesa-libGL-devel mesa-libGLU-devel mesa-libEGL-devel mesa-libGLES-devel libXext-devel libX11-devel libXi-devel libXrandr-devel libXrender-devel dbus-devel libXScrnSaver-devel libusb-devel pulseaudio-libs-devel libXinerama-devel libXcursor-devel systemd-devel

MacOS users with a working Homebrew installation, can install the necessary
dependencies with `brew install autoconf automake libtool pkg-build sdl2`.

Building SDL2
-------------

It is recommended strongly that you install SDL2 using your package manager.
However, if you are unable to work out the package name, then you can take the
harder route and build it yourself.

https://wiki.libsdl.org/Installation

Building Libtcod
----------------
Download the latest libtcod version, build it, and install it:

    $ git clone https://github.com/libtcod/libtcod.git
    $ cd libtcod/build/autotools
    $ autoreconf -i
    $ ./configure
    $ make
    $ sudo make install

This will place libtcod static and shared libraries in the `/usr/local/lib`
directory, and header files in the `/usr/local/include/libtcod` directory.

Once installed you can get the necessary compile flags for a libtcod project
with the `pkg-config libtcod --cflags --libs` command.
