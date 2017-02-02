If you need to, download Scons from http://scons.org/pages/download.html

To test this SCons script you can use the `build` alias, which will build
libtcod and all samples without touching any files outside of this
subdirectory:

```
scons build
```

This automatically downloads SDL2 on Windows and Mac as needed.
On Linux you must install the libsdl2-dev package before running this script.

SCons can compile faster using multiple cores, use the`-j <threads>` flag.

To build and install libtcod and all sample programs into the root libtcod
folder, use the `develop` alias:

```
scons develop
```

If you only need to work with the libtcod shared library you can use
`develop_libtcod` instead.

The compiled binaries will be 32-bit by default.
You can compile 64-bit binaries with the `ARCH=x86_64` variable:

```
scons develop ARCH=x86_64
```

On Mac this script will always make a universal build, ignoring `ARCH`.

Windows has the option of using the MSVC compiler (default) or MinGW compiler.
To use MinGW set the `TOOLSET=mingw` variable.

A packaged release can be made with the `dist` alias.

Additional variables can be changed such as the compiler and linker flags,
to see a list of these extra variables you should run `scons -h`.
