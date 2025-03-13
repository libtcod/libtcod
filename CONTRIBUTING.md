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
This isn't strictly necessary, but it can help with larger documentation tasks.

Newer C/C++ documentation on functions should generally try to follow this format:
```c
/***************************************************************************
    @brief [Brief summary]

    @details [Extended description]

    @param value Description of parameter.
    @return Description of the return value if any.

    @code{.cpp}
      // Code example if necessary.
    @endcode
    @versionadded{Unreleased}
 */
int example(int value);
```
Functions which are expected to always be documented are the public functions in headers, and the static functions in sources.

# Building libtcod for Development

The easiest and recommended build method is to use the Visual Studio Code with the CMake Tools extension.
This automates a significant portion of the build process.

Code formatting is handled via [clang-format](https://clang.llvm.org/docs/ClangFormat.html)
and [EditorConfig](https://editorconfig.org/), make sure your IDE supports these tools.

## Visual Studio Code

This is the common setup for developing libtcod on Windows.  It should also work on all platforms.
Dependencies are installed using Vcpkg and CMake is invoked by Visual Studio Code.

* Install [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/)'s desktop C++
* Install [Visual Studio Code](https://code.visualstudio.com/).
* Install [LLVM](https://releases.llvm.org/download.html) and make sure it's added to your PATH.
* Clone the libtcod repository and its submodules.
* In Visual Studio Code choose `File -> Open folder...` then select the libtcod repository.
* Install the extensions recommended for this workspace.
* CMake Tools will ask permission to configure the project, select yes.
* When CMake Tools asks for a kit, the recommended option is: `Visual Studio Community 2019 Release - amd64`
* When CMake Tools asks for a launch target.  Scroll down and pick `samples_cpp`.

The status bar at the bottom of Visual Studio Code can be used to configure CMake Tools.

You can now run the samples project from the IDE.  Other launch targets like
`samples_c` or `unittest` may be useful choices.
If you set `libtcod` as the build target you could check libtcod for compile errors without having to build and run the samples too.

## MacOS / Linux

For MacOS and Linux you should be able to compile libtcod easily from the
command line.  See the instructions in the relevant [buildsys](buildsys) subfolder.

Some compilation methods, including SCons (Windows, Linux, MacOS) and Autotools (Linux, MacOS), are located within the `buildsys/` subdirectory.

SCons automatically downloads SDL2 and can be used on all platforms.
Instructions are [provided here](https://github.com/libtcod/libtcod/tree/master/buildsys/scons).
The current release builds are built using SCons.

Autotools is a common standard on Linux, and can be used for MacOS.
Instructions are [provided here](https://github.com/libtcod/libtcod/tree/master/buildsys/autotools).

## CMake

The libtcod repository includes a CMake script for compiling libtcod and its tests and samples.
You can include the repository as a submodule allowing another project to build and run any version of libtcod.

By default it is assumed that Vcpkg will be used to get dependencies, but this can be changed by setting the following cache variables.
`find_package` means CMake's `find_package` command will be used.
`vendored` means that sources bundled in the repository will be statically compiled, this is generally not recommended.
`conan` and `vcpkg` means that package manager specific scripts are used to link these dependencies.
`disable` can be used to ignore a library, but functions which require that library will no longer function.

| Cache Variable   | Default      | Options | Notes |
| ---------------- | ------------ | ------- | ----- |
| LIBTCOD_SDL3     | find_package | conan, disable, find_package | Support for libtcod contexts.
| LIBTCOD_ZLIB     | find_package | conan, disable, find_package | Support for REXPaint and TCODZip.
| LIBTCOD_LODEPNG  | find_package | disable, find_package, vendored |
| LIBTCOD_UTF8PROC | vcpkg        | disable, find_package, vcpkg, vendored | Support for console printing functions.
| LIBTCOD_STB      | find_package | find_package, vendored |
| LIBTCOD_THREADS  | false        | bool | Support for deprecated functions, leave this off.
