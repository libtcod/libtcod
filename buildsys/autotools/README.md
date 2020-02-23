Building Libtcod using Autotools
================================
# Dependencies #

For Ubuntu 16.04, install these dependencies from the command line:

    sudo apt install build-essential autoconf automake libtool git libsdl2-dev

For Fedora 22, run these commands:

    sudo dnf -v groupinstall "C Development Tools and Libraries"
    sudo dnf install alsa-lib-devel audiofile-devel mesa-libGL-devel mesa-libGLU-devel mesa-libEGL-devel mesa-libGLES-devel libXext-devel libX11-devel libXi-devel libXrandr-devel libXrender-devel dbus-devel libXScrnSaver-devel libusb-devel pulseaudio-libs-devel libXinerama-devel libXcursor-devel systemd-devel

MacOS users with a working Homebrew installation, can install the necessary
dependencies with `brew install autoconf automake libtool pkg-build sdl2`.

# Building SDL2 #

It is recommended strongly that you install SDL2 using your package manager.
However, if you are unable to work out the package name, then you can take the
harder route and build it yourself.

https://wiki.libsdl.org/Installation

# Building Libtcod #

Download the latest libtcod version, build it, and install it with the
following commands:

    git clone https://github.com/libtcod/libtcod.git
    cd libtcod/buildsys/autotools
    autoreconf -i
    ./configure
    make
    sudo make install

This will place libtcod static and shared libraries in the `/usr/local/lib`
directory, and header files in the `/usr/local/include/libtcod` directory.

You should consider adding the `/usr/local` directories to PKG_CONFIG_PATH and
LD_LIBRARY_PATH if you haven't already done so:

    export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
    export LD_LIBRARY_PATH=/usr/local/lib

Now you can get the necessary compile flags for a libtcod project
with `pkg-config libtcod`, for example:

    gcc hello.c `pkg-config libtcod --cflags --libs`

# Troubleshooting #

`./configure` may fail to find `Makefile.in` and `autoreconf -i` will have the
following error:

    error: Libtool library used but 'LIBTOOL' is undefined

This means that libtools has not been installed.
