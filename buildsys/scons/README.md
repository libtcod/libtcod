If you need to, download Scons from http://scons.org/pages/download.html

This builder automatically downloads SDL2 on Windows and Intel Macs as needed.
On Linux you must install SDL2 'the Unix way' or by installing the libsdl2-dev
package before running this script.

On Windows you will need an installation of Microsoft Visual Studio or MinGW.

To use this SCons script you can use the `build` alias, which will build
libtcod in a subdirectory:

```
scons build
```

To build and install libtcod and all sample programs into the root libtcod
folder, use the `develop_all` alias.  The default mode is `MODE=DEBUG`, so
you'll also need to change `MODE` if you want a release build:

```
scons develop_all MODE=RELEASE
```

For libtcod devs, if you only need to update the libtcod shared library you can
use `develop` or `develop_libtcod` instead.

The compiled binaries will default to your installed Python architecture
(where SCons is run from.)
The default architecture on MacOS is `ARCH=universal2`.
You can compile 32-bit or 64-bit binaries with the `ARCH=x86` or `ARCH=x86_64`
variables respectfully:

```
scons develop_all MODE=RELEASE ARCH=x86_64
```

On MacOS this script can make a universal build with `ARCH=x86.x86_64` or a
universal 2 build (x86_64/arm64) with `ARCH=universal2`.

Windows has the option of using the MSVC compiler (default) or MinGW compiler.
To use MinGW set the `TOOLSET=mingw` variable.

A packaged release can be made with the `dist` alias.

Additional variables can be changed such as the compiler and linker flags.  The
easiest way to change these options is by editing `config.py`.
To see an additional list of extra variables you should run `scons -h`.

## ARM64 MacOS Users
There is experimental support for using scons to build libtcod with arm64
support when running MacOS on an arm64 CPU.
1. Use Homebrew or Macports to install scons - running in rosetta mode
doesn't seem to be an issue.
2. from ./buildsys/scons, run `scons build ARCH=arm64`.
3. The build script will put out a compiled `libtcod` binary in the scons folder
with a name that looks like `libtcod-1.16.0-alpha.15-arm64-DEBUG-macos`.
Make sure that `arm64` is in the filename. It will also output a folder called
`Frameworks` with a corresponding arm64 framework blob of SDL.

Unfortunately `scons develop_all MODE=RELEASE ARCH=arm64` does not currently
work due to inline assembly that is being consumed by the sample apps.
