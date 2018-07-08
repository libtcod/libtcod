If you need to, download Scons from http://scons.org/pages/download.html

This builder automatically downloads SDL2 on Windows and Mac as needed.
On Linux you must install SDL2 'the Unix way' or by installing the libsdl2-dev
package before running this script.

On Windows you will need an installation of Microsoft Visual Studio or MinGW.

To use this SCons script you can use the `build` alias, which will build
libtcod in a subdirectory:

```
scons build
```

SCons can compile faster using multiple cores, use the`-j <threads>` flag.

To build and install libtcod and all sample programs into the root libtcod
folder, use the `develop_all` alias.  The default mode is `MODE=DEBUG`, so
you'll also need to change `MODE` if you want a release build:

```
scons develop_all MODE=RELEASE
```

For libtcod devs, if you only need to update the libtcod shared library you can
use `develop` or `develop_libtcod` instead.

The compiled binaries will be 32-bit by default.
You can compile 64-bit binaries with the `ARCH=x86_64` variable:

```
scons develop_all MODE=RELEASE ARCH=x86_64
```

On Mac this script will always make a universal build, ignoring `ARCH`.

Windows has the option of using the MSVC compiler (default) or MinGW compiler.
To use MinGW set the `TOOLSET=mingw` variable.

A packaged release can be made with the `dist` alias.

Additional variables can be changed such as the compiler and linker flags.  The
easiest way to change these options is by editing `config.py`.
To see an additional list of extra variables you should run `scons -h`.
