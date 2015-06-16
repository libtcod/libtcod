# README #

libtcod is a free, fast, portable and uncomplicated API for roguelike developers providing an advanced true color console, input, and lots of other utilities frequently used in roguelikes.

### How do I get set up? ###

Makefiles are provided for a variety of platforms.

* Linux 32 bits.
* Linux 64 bits.
* Windows/Mingw32 (SDL).
* Windows/Mingw32 (SDL2).
* Haiku.

To be able to compile libtcod on Linux, make sure that following packages are installed (if this list is out of date, please file a bug):

* gcc
* g++
* make
* zlib
* libpng12
* libsdl1.2
* upx
* electric-fence
* libpng12-dev
* libsdl1.2-dev

### SDL vs SDL2 ###

SDL2 is the latest version of SDL.  SDL itself, is no longer actively developed.

While libtcod still supports SDL, it is no longer actively developed.  The current development efforts of the SDL project are focused on SDL2.  And similarly, libtcod development will be focused on the SDL2 code path.

For now, SDL should be considered the most stable version to use, and SDL2 experimental.  At this time, SDL2 key handling is flawed and favours tablet and mobile devices with no touch screen or numeric keypad.