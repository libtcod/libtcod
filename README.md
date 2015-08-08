libtcod is a free, fast, portable and uncomplicated API for roguelike developers providing an advanced true color console, input, and lots of other utilities frequently used in roguelikes.

**Note that 1.6 is bleeding edge and still needs polishing before it can replace 1.5.  It is recommended you use 1.5.**

## What can it do? ##

If you want to get a quick overview of the features that libtcod provides, check out the [Features](https://bitbucket.org/libtcod/libtcod/wiki/Features) page.

If you want to get a quick overview of games which have used libtcod, check out the [Projects](http://roguecentral.org/doryen/projects-2/) page.

libtcod is also well known for it's [easy to follow tutorial](http://www.roguebasin.com/index.php?title=Complete_Roguelike_Tutorial,_using_python%2Blibtcod), which many people have used to get started developing a roguelike.  Note that the tutorial uses the last stable version of libtcod, so you can't just grab the latest source code from here and run with it.  Follow the relevant links the tutorial gives, and it will point you to the right locations.

### How do I get set up? ###

All makefiles, solutions and projects are found under the `build/` subdirectory.

If you are using Windows, it is recommended you use the Visual Studio solution.

* Visual Studio.
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

**If you are using 1.5, or following the tutorial, then SDL should be your choice -- unless the tutorial you are doing refers to SDL2.**

SDL2 is the latest version of SDL.  SDL itself, is no longer actively developed.

While libtcod still supports SDL, it is no longer actively developed.  The current development efforts of the SDL project are focused on SDL2.  And similarly, libtcod development will be focused on the SDL2 code path.

For now, SDL should be considered the most stable version to use, and SDL2 experimental.  At this time, SDL2 key handling is flawed and favours tablet and mobile devices with no touch screen or numeric keypad.