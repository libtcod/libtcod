(getting-started)=

# Getting Started

***This guide sets up a portable project which will work for all platforms (Windows/Mac/Linux) as long as these instructions are followed closely.***
If you are unsure about anything then you can refer to the [libtcod template project](https://github.com/HexDecimal/libtcod-vcpkg-template) for a known working example.
The template project includes Emscripten support which will not be covered by this getting started example.

This guide will show how to setup a Libtcod project with a CMake/Vcpkg build system.
You will need [Git](https://git-scm.com/downloads), [CMake](https://cmake.org/download/), and a compiler.
For Windows [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/) will be used for the compiler.

Here is a simple example of setting up a libtcod context in C++ without using deprecated functions.
Put this code at `./src/main.cpp`:
```cpp
#include <SDL.h>
#include <libtcod.hpp>

int main(int argc, char* argv[]) {
  auto console = tcod::Console{80, 25};  // Main console.

  // Configure the context.
  auto params = TCOD_ContextParams{};
  params.tcod_version = TCODFOV_COMPILEDVERSION;  // This is required.
  params.console = console.get();  // Derive the window size from the console size.
  params.window_title = "Libtcod Project";
  params.sdl_window_flags = SDL_WINDOW_RESIZABLE;
  params.vsync = true;
  params.argc = argc;  // This allows some user-control of the context.
  params.argv = argv;

  // Tileset example using a Code Page 437 font.
  // "terminal8x8_gs_ro.png" must be in the working directory.
  // auto tileset = tcod::load_tilesheet("terminal8x8_gs_ro.png", {16, 16}, tcod::CHARMAP_CP437);
  // params.tileset = tileset.get();

  auto context = tcod::Context(params);

  while (1) {  // Game loop.
    TCOD_console_clear(console.get());
    tcod::print(console, {0, 0}, "Hello World", std::nullopt, std::nullopt);
    context.present(console);  // Updates the visible display.

    SDL_Event event;
    SDL_WaitEvent(nullptr);  // Optional, sleep until events are available.
    while (SDL_PollEvent(&event)) {
      context.convert_event_coordinates(event);  // Optional, converts pixel coordinates into tile coordinates.
      switch (event.type) {
        case SDL_QUIT:
          return 0;  // Exit.
      }
    }
  }
}
```

This source file assumes that an environment with SDL2 and libtcod is setup correctly.
To setup this environment CMake will be used to invoke Vcpkg to include those libraries.

This is done with a `./CMakeLists.txt` script:
```cmake
cmake_minimum_required (VERSION 3.21)

# Set the default toolchain to use a Vcpkg submodule.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

project(
    libtcod-getting-started  # Project name, change this as needed.
    LANGUAGES C CXX
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")  # Save all runtime files to this directory.

file(
    GLOB_RECURSE SOURCE_FILES
    CONFIGURE_DEPENDS  # Automatically reconfigure if source files are added/removed.
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.hpp
)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Ensure the C++17 standard is available.
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Enforce UTF-8 encoding on MSVC.
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()

# Enable warnings recommended for new projects.
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

find_package(SDL2 CONFIG REQUIRED)
find_package(libtcod CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main libtcod::libtcod)
```

Since this CMake script is configured to use a Vcpkg submodule we will set that up now.

Open a terminal to the project directory.
If the project is opened in an IDE then you should have a "terminal" tab which will be opened to the project root directory.
Most platforms will let you open a terminal by right-clicking on the folder from its file explorer.
On Windows after you've installed [Git for Windows](https://git-scm.com/download/win) then you should have a "Git Bash Here" as an easy option to open a terminal.

With the terminal run `git init` to turn the folder into a Git repo,
then run `git submodule add https://github.com/microsoft/vcpkg.git` to add the Vcpkg submodule.
You should have a new folder at `./vcpkg` with the Vcpkg repo.

While SDL2 and libtcod could be manually installed with Vcpkg, they can also be automatically managed with Vcpkg's [Manifest Mode](https://github.com/microsoft/vcpkg/blob/master/docs/users/manifests.md).
In Manifest Mode CMake will invoke Vcpkg which will automatically download and compile any libraries listed in the manifest.

Create a `./vcpkg.json` manifest:
```json
{
  "$schema": "https://raw.githubusercontent.com/microsoft/vcpkg/master/scripts/vcpkg.schema.json",
  "name": "getting-started",
  "version-semver": "0.0.0",
  "dependencies": [
    "libtcod",
    "sdl2",
    {
      "name": "sdl2",
      "default-features": false,
      "features": [
        "x11"
      ],
      "platform": "linux"
    }
  ]
}
```

The build system is fully setup.
You can compile and run the program with CMake or any IDE which integrates with CMake.
Visual Studio Community is one of those IDE's, you can open the project folder and then pick `libtcod-getting-started` as the target to run when asked.
[VScode](https://code.visualstudio.com/download) with the [CMake Tools plugin](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) can also build CMake projects, when asked pick the Visual Studio Community toolkit matching the compiler you installed and then pick `libtcod-getting-started` as the launch target.
VSCode works on all platforms and can be told to use whichever compiler is available locally.
Most other IDE's should have CMake integration if you look them up.

If you are stuck then try getting the [libtcod template project](https://github.com/HexDecimal/libtcod-vcpkg-template) to run since it is a known working example.
If you are still stuck then you can open a [discussion on GitHub](https://github.com/libtcod/libtcod/discussions).
