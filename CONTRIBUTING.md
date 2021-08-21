# Issue Reporting / Suggestions

The easiest way to help is to provide feedback using the
[GitHub issues](https://github.com/libtcod/libtcod/issues) page.
Most of libtcod's tasks are organized here.

Python specific issues should go to the [python-tcod issues page](https://github.com/libtcod/python-tcod/issues).

# Documentation

The current setup for doc generation is by using
[Breathe](https://breathe.readthedocs.io/en/latest/) to add
[Sphinx](https://www.sphinx-doc.org/en/master/) directives for
[Doxygen](https://www.doxygen.nl/index.html)'s documentation style.

The [docs](docs) directory has instructions on how to build the documentation.
This isn't strictly necessary, but it can help with larger tasks.

Newer C/C++ documentation on functions should try to follow this format:
```c
/**
    [Brief summary]

    [Extended description]
    \rst
    [Sphinx directives]
    \endrst
 */
void example(void);
```
Functions which should always be documented are public functions in headers and
static functions in sources.

# Building libtcod for Development

Code formatting is handled via [clang-format](https://clang.llvm.org/docs/ClangFormat.html)
and [EditorConfig](https://editorconfig.org/), make sure your IDE supports these tools.

Some compilation methods, including SCons (Windows, Linux, MacOS) and Autotools (Linux, MacOS), are located within the `buildsys/` subdirectory.

SCons automatically downloads SDL2 and can be used on all platforms.
Instructions are [provided here](https://github.com/libtcod/libtcod/tree/master/buildsys/scons).
The current release builds are built using SCons.

Autotools is a common standard on Linux, and can be used for MacOS.
Instructions are [provided here](https://github.com/libtcod/libtcod/tree/master/buildsys/autotools).

## MacOS / Linux

For MacOS and Linux you should be able to compile libtcod easily from the
command line.  See the instructions in the relevant [buildsys](buildsys) subfolder.

## Visual Studio Code on Windows

This is a common setup for developing libtcod on Windows.
Dependencies are installed using Vcpkg and CMake is invoked by Visual Studio Code.

* Install [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/)'s desktop C++
* Install [Visual Studio Code](https://code.visualstudio.com/).
* Install [LLVM](https://releases.llvm.org/download.html) and make sure it's added to your PATH.
* Clone the [Vcpkg repository](https://github.com/microsoft/vcpkg) and run the bootstrap script.
* Ensure the [VCPKG_ROOT environment variable](https://github.com/microsoft/vcpkg/blob/master/docs/users/config-environment.md#vcpkg_root) is set to the repository folder.
* You'll need to have the command line in the vcpkg directory or have that added to PATH for the following:
  * Use Vcpkg to install libtcod's dependencies as needed:
    ```
    vcpkg install sdl2:x64-windows zlib:x64-windows glad:x64-windows stb:x64-windows utf8proc:x64-windows lodepng-c:x64-windows
    ```
* Clone the libtcod repository.
* In Visual Studio Code choose `File -> Open folder...` then select the libtcod repository.
* Install the recommended extensions.
* CMake Tools will ask permission to configure the project, select yes.
* When CMake Tools asks for a kit, the recommended option is: `Visual Studio Community 2019 Release - amd64`
* When CMake Tools asks for a launch target.  Scroll down and pick `samples_cpp`.

The status bar at the bottom of Visual Studio Code can be used to configure CMake Tools.

You can now run the samples project from the IDE.  Other launch targets like
`samples_c` or `unittest` may be useful choices.
If you set `libtcod` as the build target you could check libtcod for compile errors without having to build and run the samples too.

## CMake

The libtcod repository includes a CMake script for compiling libtcod and its tests and samples.
You can include the repository as a submodule allowing another project to build and run any version of libtcod.

By default it is assumed that Vcpkg will be used to get dependencies, but this can be changed by setting the following cache variables.
`find_package` means CMake's `find_package` command will be used.
`vendored` means that sources bundled in the repository will be statically compiled, this is generally not recommended.
`conan` and `vcpkg` means that package manager specific scripts are used to link these dependencies.

| Cache Variable   | Default      | Options |
| ---------------- | ------------ | ------- |
| LIBTCOD_SDL2     | find_package | conan, find_package |
| LIBTCOD_ZLIB     | find_package | conan, find_package |
| LIBTCOD_GLAD     | find_package | find_package, vendored |
| LIBTCOD_LODEPNG  | find_package | find_package, vendored |
| LIBTCOD_UTF8PROC | vcpkg        | find_package, vcpkg |
| LIBTCOD_STB      | vcpkg        | vcpkg, vendored |
