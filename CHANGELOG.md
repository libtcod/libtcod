# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)

This project attempts to adhere to [Semantic Versioning](http://semver.org/)
since `1.7.0`.

## [Unreleased]
### Added
- You can now set the `TCOD_RENDERER` and `TCOD_VSYNC` environment variables to
  force specific options to be used.
  Example: ``TCOD_RENDERER=sdl2 TCOD_VSYNC=1``

### Changed
- `TCOD_sys_set_renderer` now returns an error status.
- `TCODSystem::setRenderer` will now throw instead of terminating on an error.
- `TCOD_console_init_root` and `TCOD_sys_set_renderer` are now no-discard.

### Fixed
- `TCOD_console_map_ascii_code_to_font` functions will now work when called
  before `TCOD_console_init_root`.

## [1.12.3] - 2019-06-02
### Fixed
- Fixed regressions in text alignment for non-rectangle-bound text on UTF-8
  print functions.

## [1.12.2] - 2019-05-24
### Fixed
- Fixed crashes and bad output when using printf-style functions.

## [1.12.1] - 2019-05-17
### Fixed
- Fixed shader compilation issues in the OPENGL2 renderer.
- Fallback fonts should fail less on Linux.

## [1.12.0] - 2019-05-17
### Added
- Added the `TCOD_get_error` and `TCOD_set_error` functions.

### Changed
- `TCOD_console_init_root` and `TCOD_console_set_custom_font` will now return
  an error code instead of terminating on a known error.

## [1.11.5] - 2019-05-10
### Added
- Several functions in the `tcod::console` namespace are now accessible.
  These functions are provisional, so the API may change at anytime.

### Fixed
- Corrected bounding box issues with the stateless UTF-8 print functions.

## [1.11.4] - 2019-04-26
### Fixed
- Fixed a name conflict with endianness macros on FreeBSD.

## [1.11.3] - 2019-04-19
### Changed
- New consoles are initialized as if clear was called.
- The SDL callback type now uses a typed argument instead of void pointer.

### Fixed
- Fixed horizontal alignment for TrueType fonts.
- Fixed screenshots with the older SDL renderer.

### Deprecated
- The SDL callback feature has been deprecated.

## [1.11.2] - 2019-03-29
### Fixed
- Console printing was ignoring color codes at the beginning of a string.
- Releases no longer display debug messages.

## [1.11.1] - 2019-03-15
### Fixed
- Fixed header declaration for `TCOD_sys_accumulate_console`.

## [1.11.0] - 2019-03-15
### Added
- Added new functions which convert from SDL events to libtcod events:
  - `TCOD_sys_process_key_event`
  - `TCOD_sys_process_mouse_event`
  - `tcod::sdl2::process_event`
- Added a way to access libtcod's SDL variables.
  - `TCOD_sys_get_sdl_window`
  - `TCOD_sys_get_sdl_renderer`
- Added `TCOD_sys_accumulate_console` which renders a console to the display
  without automatically presenting it.
- The new renderers can potentially use a fall-back font when none is provided.

### Fixed
- `TCOD_console_has_mouse_focus` and `TCOD_console_is_active` are now more
  reliable with the newer renderers.

## [1.10.8] - 2019-03-01
### Deprecated
- Updated libtcodpy deprecation warning with instructions on how to update to
  python-tcod.

## [1.10.7] - 2019-03-01
### Changed
- GCC releases now include debug symbols.
### Fixed
- `SDL2` and `OPENGL2` now support screenshots.
- `TCOD_sys_get_char_size` fixed on the newer renderers.

## [1.10.6] - 2019-02-15
### Changed
- Printing generates more compact layouts.
### Fixed
- Fixed segmentation faults in the newer print functions caused by bad color
  codes.
- The `OPENGL` and `GLSL` renders work again in static builds.
- The `SDL2` and `OPENGL2` renders can now large numbers of tiles.

## [1.10.5] - 2019-02-06
### Changed
- The SDL2/OPENGL2 renderers will now auto-detect a custom fonts key-color.
### Fixed
- `TCOD_console_printf_frame` was clearing tiles outside if it's bounds.
- The `TCOD_FONT_LAYOUT_CP437` layout was incorrect.

## [1.10.4] - 2019-02-01
### Fixed
- Fixed regression where mouse wheel events unset mouse coordinates.

## [1.10.3] - 2019-01-19
### Fixed
- Use correct math for tile-based delta in mouse events.
- New renderers now support tile-based mouse coordinates.
- SDL2 renderer will now properly refresh after the window is resized.
- MinGW samples now bundle their own runtimes.

## [1.10.2] - 2018-11-23
### Fixed
- Version numbers were not updated.

## [1.10.1] - 2018-11-23
### Changed
- `RENDERER_OPENGL2` now only needs OpenGL 2.0 to load.

### Fixed
- `FONT_LAYOUT_CP437` constant was missing from libtcodpy.
- Function stubs for `TCOD_console_set_keyboard_repeat` and
  `TCOD_console_disable_keyboard_repeat` have been added to the ABI, this
  should fix older programs which depended on them existing.

## [1.10.0] - 2018-11-17
### Added
- Added pkg-config for 'libtcod' on AutoTools builds.
- Added `TCOD_FONT_LAYOUT_CP437`.

### Fixed
- `RENDERER_OPENGL2` constant was missing from libtcodpy.
- Fixed crash when assigning to larger code-points.
- Fixed center alignment on printf functions.
- Fixed missing border tiles on printf frame function.

## [1.9.0] - 2018-10-24
### Added
- New renderer: `TCOD_RENDERER_OPENGL2`
- `TCOD_RENDERER_SDL2` and `TCOD_RENDERER_OPENGL2` now have reliable fullscreen
  support.
- SCons MSVC releases now include libtcod's `.pdb` file.

## Changed
- `src/vendor` must be included when building libtcod.
- `glad.c` added to vendor directory.
- In GCC builds: non-public symbols are no longer visible by default.

## [1.8.2] - 2018-10-13
### Fixed
- Fixed potential crash when splitting lines in UTF-8 print functions.
- Fixed possible hang in `TCOD_lex_parse_until_token_value`.

## [1.8.1] - 2018-10-11
### Changed
- Updated the compiler used to deploy Windows MinGW builds.

## [1.8.0] - 2018-10-07
### Added
- New `TCOD_MAJOR_VERSION`, `TCOD_MINOR_VERSION`, and `TCOD_PATCHLEVEL`
  constants.
- Added UTF-8 print functions for C and C++:
  - `TCOD_console_printf`, `TCOD_console_printf_ex`,
    `TCOD_console_printf_rect`, and `TCOD_console_printf_rect_ex`.
  - `TCOD_console_printf_frame`
  - `TCOD_console_get_height_rect_fmt`
  - New overrides for `TCODConsole::print` which can take `std::string`.
  - `TCODConsole::printf`.
- Added `TCOD_quit` function, this function must be called on exit.
- New renderer: `TCOD_RENDERER_SDL2`

### Changed
- AutoTools now adds version numbers to the libraries it builds.
- lodepng and zlib sources moved into `src/vendor/` folder.
- Bundled zlib library updated from `1.2.8` to `1.2.11`.
- LodePNG updated from `20160501` to `20180611`.
- libtcod C type declarations are stricter.
- libtcod now uses the C99 and C++14 standards.
- In the repository `include/` headers have been moved into `src/`.
- Most headers except for `libtcod.h` and `libtcod.h` have been moved into a
  `libtcod/` sub-folder.
- SCons default architecture is now automatic instead of just 32-bit.
- libtcod gui headers now use LIBTCOD_EXPORTS instead of LIBTCOD_GUI_EXPORTS to
  determine dllexport/dllimport.

### Deprecated
- All functions replaced by the new UTF-8 functions are deprecated.
- The ctypes implementation of libtcodpy will no longer be maintained, use
  python-tcod as a drop-in replacement.
- Deprecated file-system, threading, clipboard, and dynamic library functions.

### Fixed
- `libtcodpy.map_clear`: `transparent` and `walkable` parameters were reversed.
- Resolved build issues on PowerPC platforms.
- Mouse state is now tracked even when mouse events are not asked for.
- libtcod sys event functions no longer crash if the given key pointer is NULL.
- SCons no longer crashes downloading SDL2 for Windows.

### Removed
- Autotools no longer creates `libtcodxx` or `libtcodgui` libraries, C++
  programs should link to `libtcod` instead.

## [1.7.0] - 2018-06-12
### Changed
- `TCOD_console_is_active()` now returns keyboard focus, its previous return
  value was undefined.
- Load font debug information is now suppressed when NDEBUG is defined.
- Error messages when failing to create an SDL window should be a less vague.
- You no longer need to initialize libtcod before you can print to an
  off-screen console.

### Fixed
- Fixed a missing space between a literal and identifier that causes problems
  with some C++ compilers.
- Fixed implicit definitions that could potentially crash formatted console
  printing functions and image saving functions.
- Fixed including libtcod gui headers. (Fabian Wolff)
- Fixed memory leaks when lexer creation fails. (Fabian Wolff)
- Avoid crashes if the root console has a character code higher than expected.
- Fixed `AttributeError: module 'urllib' has no attribute 'urlretrieve'` when
  running SCons builder with Python 3.
- Fixed `UnicodeEncodeError: ordinal not in range(128)` in places where
  libtcodpy says it wants an ASCII string.
- libtcodpy now escapes the `%` character on print functions.
- Fixed libtcodpy print functions not supporting Unicode for Python 3.
- Fixed libtcodpy missing definition `TCOD_console_print_utf`.

## [1.6.7] - 2018-05-17
### Changed
- Renamed `libtcod-CHANGELOG.txt` to `CHANGELOG.md`
- SCons builder now outputs `libtcodgui` instead of `libtcod-gui` to match
  Autotools behaviour.

### Fixed
- REXPaint operations now save/load as little-endian on any platform.
  https://bitbucket.org/libtcod/libtcod/issues/91/fix-the-libtcod-types-endian-support
- `TCOD_console_map_ascii_code_to_font` and related functions can now assign
  character codes that were not automatically assigned during initialization.
  Previously you could only assign codes up to the total number of tiles in the
  font.
- Fixed including libtcod from Autotools installs, the `console_rexpaint.h`
  header was missing. (James Byrnes)

## [1.6.6] - 2018-04-06
### Added
- Added `libtcod_c.c` and `libtcod.cpp` source files to assist with static
  linking.

### Changed
- REXPaint functions now have their own header file: `console_rexpaint.h`
- `TCOD_color_set_HSV` now normalizes bad hue/saturation/value parameters.

### Deprecated
- Non-reentrant line functions `TCOD_line_init` and `TCOD_line_step` are now
  deprecated.
- `TCOD_line_mt` is deprecated.  `TCOD_line` is the same function with an
  unnecessary parameter, and should be prefered.

### Fixed
- `TCOD_line` is now reentrant.
- `FOV_BASIC` (`fov_circular_raycasting`) implementation is now reentrant.
- Fixed `external/pstdint.h` header not being installed correctly by Autotools.
  https://bitbucket.org/libtcod/libtcod/issues/121/include-of-external-pstdinth-fails
- Autotools libtools are no longer bundled, these are automatically installed
  via `autoreconf -i`

## [1.6.5] - 2018-02-20
### Fixed
- Resolved header issues that were leaving some types and functions undefined. (Kyle Stewart)
- key.c is no longer set when key.vk == TCODK_TEXT, this fixes a regression
  which was causing events to be heard twice in the libtcod/Python tutorial. (Kyle Stewart)
- OpenGL renderer alpha blending is now consistent with all other render modes. (Kyle Stewart)
  https://bitbucket.org/libtcod/libtcod/issues/99/opengl-renderer-looks-poorly-compared-to

## [1.6.4] - 2017-11-27
### Added
- Added support for loading/saving REXPaint files. (Kyle Stewart)
- Added documentation to guide people in upgrading from 1.5.x to 1.6.x (rmtew)
  https://bitbucket.org/libtcod/libtcod/issues/88

### Changed
- Upgraded to `stdint.h` source code typing via `pstdint.h` (rmtew)
  https://bitbucket.org/libtcod/libtcod/issues/91
- All libtcod headers are now self-contained. (Kyle Stewart)
  https://bitbucket.org/libtcod/libtcod/issues/93/header-organization
- bool type is now based off of stdbool.h (Kyle Stewart)
- SCons builder defaults to debug builds, now builds with OpenGL, and uses link
  time optimization on release builds. (Kyle Stewart)
  https://bitbucket.org/libtcod/libtcod/issues/92/tcod_renderer_glsl-not-working
- SDL is no longer initialized at module load time. (Kyle Stewart)
  https://bitbucket.org/libtcod/libtcod/issues/107/sdl_init-on-library-load-libtcod

### Fixed
- Text input events now include correct modifier key values. (Kyle Stewart)
- Fixed touch support compilation, although it is still an experimental option that may change (rmtew)
- Console objects should be safe to create and use before libtcod is fully initialized. (Kyle Stewart)
  https://bitbucket.org/libtcod/libtcod/issues/17/offline-libtcod
- Fixed TCOD_list_reverse. (Kyle Stewart)
- Fixed simplex noise artifacts when using negative coordinates. (Kyle Stewart)
- Fixed the GLSL and OPENGL renderers on all platforms. (rmtew) (Kyle Stewart)
  https://bitbucket.org/libtcod/libtcod/issues/92/tcod_renderer_glsl-not-working
- TCOD_image_blit_2x no longer alters a consoles default colors. (Kyle Stewart)
- SDL renderer is only created when using SDL rendering (wump@bitbucket)
  https://bitbucket.org/libtcod/libtcod/pull-requests/74/dont-create-sdl-renderer-when-using-opengl

## [1.6.3] - 2017-02-26
### Added
- Added SCons build support (HexDecimal@bitbucket).
  https://bitbucket.org/libtcod/libtcod/commits/92c5a1b530049f15d1ed36375b12781554272f17
- Exposed clipboard API to Python (rmtew@bitbucket).
- Added TravisCI integration tests (Kyle Stewart) https://bitbucket.org/libtcod/libtcod/pull-requests/61
- Added .gitattributes and .gitignore files (Kyle Stewart) https://bitbucket.org/libtcod/libtcod/pull-requests/61

### Changed
- Updated restrictive FOV code to match MRPAS v1.2 (mingos@bitbucket)
  https://bitbucket.org/libtcod/libtcod/commits/28a19912ac7fef6e68164fb8970230517888c9ff
- Per-platform clipboard get/set code has been removed, and the SDL2 API is used instead (rmtew@bitbucket).
  The clipboard encoding is now UTF-8 on all platforms.
  https://bitbucket.org/libtcod/libtcod/issues/81/
- Documented and undeprecated TCOD_mouse_get_status() as the state is awkwardly exposed via check/wait event calls anyway (rmtew@bitbucket).
- Switched Python tests to use pytest instead of unittest. (Kyle Stewart)
  https://bitbucket.org/libtcod/libtcod/pull-requests/61

### Fixed
- TCOD_path_walk now considers negative values to be blocking the path (HexDecimal@bitbucket)
  https://bitbucket.org/libtcod/libtcod/commits/fc8c88ec9b5d7b113ba3a7906f526daba81b543c
- Fix key handling in name generator in C sample (reported by Chris Hamons@bitbucket)
  https://bitbucket.org/libtcod/libtcod/issues/83/
- Fix for broken SDL callback (reported by grimstain@bitbucket)
  https://bitbucket.org/libtcod/libtcod/issues/78/sdl-callback-null-pixels
- Fix for memory leak introduced with the foreground/background image change (Aaron Wise).
  https://bitbucket.org/libtcod/libtcod/commits/275e8bd97000599e9d87bfa138fa72c066b1cae7
- Fix for bug in Slider::setFormat() which left a dangling pointer (reported by Kai Kratz@bitbucket)
  https://bitbucket.org/libtcod/libtcod/commits/07c1214077d121107c5ad8ee38c589ad677f5e76
- Fix potential wavelet related memory leak in TCOD_noise_delete() (HexDecimal@bitbucket)
  https://bitbucket.org/libtcod/libtcod/commits/a03a79814fd734e05e5d72801376b2af6b315373
- Fix libtcodpy return type for TCOD_console_put_char_ex and TCOD_console_put_char (HexDecimal@bitbucket)
  https://bitbucket.org/libtcod/libtcod/commits/a03a79814fd734e05e5d72801376b2af6b315373
- Fix libtcodpy arguments for TCOD_heightmap_add_fbm and TCOD_heightmap_scale_fbm (HexDecimal@bitbucket)
  https://bitbucket.org/libtcod/libtcod/commits/6f34ffa3d17af39cc274c048ff633be8609998f8
- Fixed creating a libtcodpy pathfinder with a callback would crash on 64-bit Unix systems (Kyle Stewart)
  https://bitbucket.org/libtcod/libtcod/pull-requests/61

## [1.6.2] - 2016-12-28
### Added
- doctcod command added to VS2015 solution.
- Python (PTVS) project for samples_py added to VS2015 solution.
- Console foreground and background colours are images.
  https://bitbucket.org/libtcod/libtcod/issues/51/console-image-unification
- Python unit tests added (Kyle Stewart).
  https://bitbucket.org/libtcod/libtcod/commits/7a66ad5d66832686c0071e3938b85ebad8e7cebb
- Added TCOD_console_get_background_color_image and TCOD_console_get_foreground_color_image API for C++/C/Python.

### Changed
- crash bugs fixed in documentation generation.
- libtcodpy when loading a DLL now explicitly compares the architecture of the Python version uses, to the architecture of the DLL and exits outputting any difference.
- libtcodpy now uses more wrapper functions on Windows to cover ctypes limitations in passing and returning struct values.
- libtcodpy now inlines more ctypes argtypes and restype definitions in order to work better on x64.
- libtcodpy partially rewritten to support Python 3 as best possible and resolve outstanding 64 bit problems.
- samples_py partially rewritten to run under both Python 3 and Python 2.
- Adopted strict prototypes / void in C parameterless functions (Kyle Stewart).
  https://bitbucket.org/libtcod/libtcod/commits/5353098a70254b59b740865a875cccf1d6d84c27
- SDL2 supported revision updated to release-2.0.5 AKA changeset 007dfe83abf8.

### Fixed
- C++ TCODParser memory leak fixed.
  https://bitbucket.org/libtcod/libtcod/issues/27/tcodparser-memory-leaks
- Frost sample fixed for a non-initialised data structure crash.

### Removed
- Removed SDL1 support.
- Removed C/C++/Python API functions `TCOD_console_set_keyboard_repeat` and
  `TCOD_console_disable_keyboard_repeat`.  This was SDL1 functionality, and not supported in SDL2.

## [1.6.1] - 2016-09-23
### Changed
- Python libtcodpy is now better at finding DLLs on Windows
  https://bitbucket.org/libtcod/libtcod/commits/eda0075

## [1.6.0] - 2016-09-16
### Added
- added support for autotools builds
  https://bitbucket.org/libtcod/libtcod/commits/41e1c24
- added support for Visual Studio builds
- added Visual Studio build script that can fetch and build dependencies, the project, and package up the result
- hooked up Visual Studio build script to continuous integration service to run on each commit
- libtcod is now reentrant which allows the window to be resized among other things
  https://bitbucket.org/libtcod/libtcod/commits/14bad22
- added new TCODK_TEXT event to cover the SDL2 SDL_TEXTINPUT event.
  https://bitbucket.org/libtcod/libtcod/commits/7a8b072
- lmeta and rmeta modifiers have been added to TCOD_key_t
  https://bitbucket.org/libtcod/libtcod/commits/e386362

### Changed
- upgraded to SDL2
- upgraded zlib to version 1.2.8
- upgraded lodepng to the 20160501 release
  https://bitbucket.org/libtcod/libtcod/commits/60c127e

### Fixed
- fixed compilation warnings in pathing code
  https://bitbucket.org/libtcod/libtcod/commits/4045633
- fixed memory leaks in pathing (Paral Zsolt)
  https://bitbucket.org/libtcod/libtcod/commits/4c4af80
- map numlock key for SDL2
  https://bitbucket.org/libtcod/libtcod/commits/83d481c
- reset the cursor in text field when the text field is reset (cottog)
  https://bitbucket.org/libtcod/libtcod/commits/6673e6c
- remove superfluous calls to SDL_PumpEvents
  https://bitbucket.org/libtcod/libtcod/commits/1edf96d
- alt-tabbing back to the libtcod window would leave the window blank
  https://bitbucket.org/libtcod/libtcod/commits/73fdf51

### Removed
- removed support for mingw builds because no-one could get it to work

## [1.5.2] - 2012-09-07
### Added
- added mid point displacement algorithm to the heightmap toolkit
- added TCODConsole::hasMouseFocus() and TCODConsole::isActive()
- added TCODParser::hasProperty to check if a property is defined when
  using the default parser
- added `TCODText::setPos(int x, int y)`

### Fixed
- fixed TCODConsole::waitForKeypress returning for both press and release events (returns only on press)
- fixed dynamic font switching not working
- fixed TCOD_image_blit_rect not working with odd width/height
- fixed TCODK_RWIN/TCODK_LWIN not detected
- fixed TCOD_sys_wait_event not returning on mouse events
- fixed mouse dcx/dcy fields always 0 when the cursor moves slowly
- fixed crash in Python console_map_ascii_codes_to_font

## [1.5.1] - 2012-08-29
### Added
- added support for python3k. The samples run with Python 3.2.3
- added support for colored tiles. Standard font characters MUST only use greyscale colors.
- added native support for .ASC and .APF file (Ascii Paint format)
  * TCODConsole::TCODConsole(const char *filename)
  * bool TCODConsole::loadAsc(const char *filename)
  * bool TCODConsole::saveAsc(const char *filename) const
  * bool TCODConsole::loadApf(const char *filename)
  * bool TCODConsole::saveApf(const char *filename) const
- added mouse wheel support in TCOD_mouse_t.wheel_up/wheel_down
- added TCODSystem::fileExists function for checking whether a given file exists
- added dice to the TCODRandom toolkit
- added support for dynamic property declaration in the parser
- added TCODList::reverse(), TCODPath::reverse(), TCODDijkstra::reverse()
- added weighted Gaussian distribution RNG
- added Gaussian distribution RNG with no minimum/maximum bounds (using only mean and standard deviance)
- added clipboard support in TCODSystem (on Linux, only X clipboard supported)
- added GLSL and OpenGL(fixed pipeline) renderer (FPS increased 880% on true color sample !!!)
- added libtcod.cfg (allows the player to tweaks libtcod overriding the game presets)
- added more TCOD_CHAR_* constants
- added TCODColor::scaleHSV (saturation and value scaling)
- added TCODColor::shiftHue (hue shifting up and down)
- added a TCODColor constructor for HSV values
- added TCODColor H, S and V separate getters and setters
- added TCODColor::rotate90
- added native Mac OSX support
- added support for quote-less HTML color values in the parser (col=#FFFFFF instead of col="#FFFFFF")
- added color control helpers for C# (TCODConsole.getColorControlString/getRGBColorControlString)

### Changed
- TCOD_console_wait_for_keypress, TCOD_console_check_for_keypress, TCOD_mouse_get_status
  replaced with TCOD_sys_check_for_event and TCOD_sys_wait_for_event
- source width and height can be 0 in TCODConsole::blit to blit the whole console
- Some of the parser-related functions now can be chained.
- The RNG API now has less functions that choose the distribution based on the set flags.
- The noise API now has less functions that choose the noise type based on the set flags.
- Console default and cell foreground and background colour getters and setters renamed to more intuitive names:
  * `TCOD_console_set_background_color` => `TCOD_console_set_default_background`
  * `TCOD_console_set_foreground_color` => `TCOD_console_set_default_foreground`
  * `TCOD_console_get_background_color` => `TCOD_console_get_default_background`
  * `TCOD_console_get_foreground_color` => `TCOD_console_get_default_foreground`
  * `TCOD_console_set_back` => `TCOD_console_set_char_background`
  * `TCOD_console_set_fore` => `TCOD_console_set_char_foreground`
  * `TCOD_console_get_back` => `TCOD_console_get_char_background`
  * `TCOD_console_get_fore` => `TCOD_console_get_char_foreground`
  * `setBackgroundColor` => `setDefaultBackground`
  * `setForegroundColor` => `setDefaultForeground`
  * `getBackgroundColor` => `getDefaultBackground`
  * `getForegroundColor` => `getDefaultForeground`
  * `setBack` => `setCharBackground`
  * `setFore` => `setCharForeground`
  * `getBack` => `getCharBackground`
  * `getFore` => `getCharForeground`
- `TCODConsole::printLeft/Right/Center` replaced by
  `TCODConsole::setBackgroundFlag(TCOD_bkgnd_flag_t)`
  `TCODConsole::setAlignment(TCOD_alignment_t)`
  `TCODConsole::print(int x, int y, const char *fmt, ...)`
  `TCODConsole::printEx(int x, int y, TCOD_bkgnd_flag_t bkgnd, TCOD_alignment_t alignment, const char *fmt, ...)`
- added `TCOD_BKGND_DEFAULT` as default value in rect / putChar / hline / vline / setBack
  the default background flag for a console is BKGND_NONE
    Warning ! That mean the rect/putChar/... functions now default to BKGND_NONE
    instead of BKGND_SET, except if you call setBackgroundFlag before using them.
- `TCODConsole::getHeight[Left/Right/Center]Rect` replaced by
  `TCODConsole::getHeightRect(int x, int y, int w, int h, const char *fmt, ...);`
  (the 3 functions were useless, the height does not depend on the alignement... X( )
- `TCODConsole::initRoot` has an additional renderer parameter:
  `static void TCODConsole::initRoot(int w, int h, const char * title, bool fullscreen = false, TCOD_renderer_t renderer=TCOD_RENDERER_SDL);`
  Possible values:
  - TCOD_RENDERER_GLSL
  - TCOD_RENDERER_OPENGL
  - TCOD_RENDERER_SDL
- `TCODMap::clear` now sets walkable and transparent status of all cells:
  `void TCODMap::clear` (bool transparent = false, bool walkable = false)
  `void TCOD_map_clear` (TCOD_map_t map, bool transparent, bool walkable)
  map_clear (map, transparent, walkable)
- cmake compilation works on FreeBSD thanks to namor_
- Restrictive FOV algo updated to MRPAS v1.1 (faster, better-looking, improved symmetry)
- Gaussian distribution in `TCODRandom` now uses a more precise Box-Muller transform algorithm
- More default values for `printFrame`:
  `void printFrame(int x,int y,int w,int h, bool clear=true, TCOD_bkgnd_flag_t flag = TCOD_BKGND_DEFAULT, const char *fmt=NULL, ...)`
- hardened fov module
- extended list of colour constants (the old names can produce slightly different colours)
- `TCODMap` memory consumption divided by 4
- now mouse wheel events are properly detected

### Fixed
- fixed namegen crash if generator list is empty
- fixed permissive fov when light_walls is false
- fixed possible crash when clearing an offscreen console before calling initRoot
- fix possible crash when printing a string containing ascii codes > 127
- fixed TCODNamegen returning integers in the Python version
- fixed TCODDijkstra segfault when diagonal movement cost was zero
- fixed setDirty() not working for the last row or column of the console
- fixed Python samples not compiling if numpy installed
- fixed Python TCOD_parser_get_list_property
- fixed TCODDijkstra destructor crash bug
- fixed TCODColor::setHSV bug when using hues below 0 or above 359 grades
- fixed some rare issues in A* pathfinding
- fixed issue in TCODImage::blit2x when blitting only a subregion of the image
- fixed memory leak in TCODImage::save

## [1.5.0] - 2010-02-15
### Added
- added text field toolkit (undocumented)
- added functions for fast Python full console coloring:
  - console_fill_foreground(con,r,g,b)
  - console_fill_background(con,r,g,b)

  r,g,b are 1D arrays of console_width * console_height
- added fast Python rendering sample (Jotaf)
- added TCODConsole::resetCredits() to restart credits animation before it's finished
- added TCODConsole::setDirty(int x, int y, int w,int h) to force libtcod to redraw a part of the console
  This might by needed when using the SDL renderer
- added TCODSystem::getCharSize(int *w, int *h) to get the current font's character size
- added name generation module (Mingos)
- added Dijkstra pathfinding (Mingos)
- added approximated gaussian distribution functions in the RNG module
  `float TCODRandom::getGaussian(float min, float max)`
- added subcell resolution blitting function TCODImage::blit2x
- added more portable filesystem utilities:
  - `static bool TCODSystem::isDirectory(const char *path)`
  - `static TCODList<const char *> TCODSystem::getDirectoryContent(const char *path, const char *pattern)`
- added TCODConsole::putCharEx(int x, int y, int c, TCODColor &fore, TCODColor &back) (at last!)
- added waitThread and thread conditions support in (undocumented) threading API
- added unicode support and 4 unicode fonts courtesy of Mingos. Functions with unicode support are :
  - static void TCODConsole::mapStringToFont(const wchar_t *s, int fontCharX, int fontCharY);
  - void TCODConsole::printLeft(int x, int y, TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...);
  - void TCODConsole::printRight(int x, int y, TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...);
  - void TCODConsole::printCenter(int x, int y, TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...);
  - int TCODConsole::printLeftRect(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...);
  - int TCODConsole::printRightRect(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...);
  - int TCODConsole::printCenterRect(int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, const wchar_t *fmt, ...);
  - int TCODConsole::getHeightLeftRect(int x, int y, int w, int h, const wchar_t *fmt, ...);
  - int TCODConsole::getHeightRightRect(int x, int y, int w, int h, const wchar_t *fmt, ...);
  - int TCODConsole::getHeightCenterRect(int x, int y, int w, int h,const wchar_t *fmt, ...);

  C versions of the functions have _utf suffixes. Unicode fonts are :
  - fonts/consolas_unicode_10x10.png
  - fonts/consolas_unicode_12x12.png
  - fonts/consolas_unicode_16x16.png
  - fonts/consolas_unicode_8x8.png

### Changed
- the random number generator module now support two algorithms (Mingos)
  Mersenne twister, used in previous versions of libtcod
  Complementary Multiply With Carry, 2 to 3 times faster and has a much better
  period. This is the default algo.
  You can choose the algorithm in the constructor:
  ```c
  typedef enum {
    TCOD_RNG_MT,
    TCOD_RNG_CMWC
  } TCOD_random_algo_t;
  ```
  - TCODRandom() => CMWC, default seed
  - TCODRandom(uint32 seed) => CMWC, custom seed
  - TCODRandom(TCOD_random_algo_t algo) => default seed, custom algo
  - TCODRandom(uint32 seed, TCOD_random_algo_t aldo) => custom algo and seed
- TCODConsole::printFrame now takes an extra argument: TCOD_bkgnd_flag_t
- renamed libraries on Linux to improve portability :
  - libtcod++.so  => libtcodxx.so
  - libtcod-gui.so => libtcodgui.so
- new compilation system with a debug and release version of the library
- makefiles have been moved to a makefiles subdirectory
- libtcod credits now use subcell resolution

### Fixed
- fixed wrong ascii code returned by check/waitForKeypress with AltGr+0-9 combination
- fixed RNG going mad if you delete the default RNG
- fixed wait_for_keypress not working in Python
- now the parser can handle strings/identifiers with unlimited size (previously limited to 256 chars)
- fixed TCODZip module missing from MSVC release
- fixed issue with TCOD_COLCTRL_FORE_RGB and TCOD_COLCTRL_BACK_RGB when using 10 or 37 in rgb values
- fixed issue in color_set_hsv Python wrapper
- fixed bug in console blitting when destination coordinates are outside the destination console

### Removed
- TCODRandom::getIntFromByteArray has been deleted
- removed bitfield from TCOD_key_t and TCOD_mouse_t to ease wrappers writing.
  existing working wrappers might be broken, though.

## [1.4.2] - 2009-10-06
### Added
- added possibility to use SDL drawing functions on top of the root console with TCODSystem::registerSDLRenderer
- added Mingos' restrictive precise angle shadowcasting fov algorithm as FOV_RESTRICTIVE
- heightmap: added missing `TCOD_heightmap_set_value` function
- new consolas, dejavu, lucida and prestige fonts from Mingos

### Changed
- `TCODConsole::blit` now takes 2 float parameters:
  `static void blit(const TCODConsole *src,int xSrc, int ySrc, int wSrc, int hSrc, TCODConsole *dst, int xDst, int yDst, float foreground_alpha=1.0f, float background_alpha=1.0f)`

### Fixed
- fixed R/B inversion in color operations in Python
- fixed a crash when saving a TCODImage before initializing the root console
- fixed a crash when destroying an empty TCODZip (file not found)
- fixed torch effect in C++ sample
- fixed ASCII_IN_ROW flag not working
- fixed path_new_using_function not working in Python wrapper
- fixed alignment issues in `TCOD_console_print_*` with strings containing
  ascii codes > 127
- fixed color controls not working with autowrap
- fixed fov post-processing step to handle opaque walkable cells (for example bush)

## [1.4.1] - 2009-03-22
### Added
- added Python wrapper
- added arial, courier fonts courtesy of Mingos
- added some non portable filesystem utilities in TCODSystem :
  - bool TCODSystem::createDirectory(const char *path);
  - bool TCODSystem::deleteDirectory(const char *path);
  - bool TCODSystem::deleteFile(const char *path);
- added multithread utilities in TCODSystem :
  - static TCOD_thread_t TCODSystem::newThread(int (*func)(void *), void *data);
  - static void TCODSystem::deleteThread(TCOD_thread_t th);
  - static TCOD_mutex_t TCODSystem::newMutex();
  - static void TCODSystem::mutexIn(TCOD_mutex_t mut);
  - static void TCODSystem::mutexOut(TCOD_mutex_t mut);
  - static void TCODSystem::deleteMutex(TCOD_mutex_t mut);
  - static TCOD_semaphore_t TCODSystem::newSemaphore(int initVal);
  - static void TCODSystem::lockSemaphore(TCOD_semaphore_t sem);
  - static void TCODSystem::unlockSemaphore(TCOD_semaphore_t sem);
  - static void TCODSystem::deleteSemaphore( TCOD_semaphore_t sem);
- added some image utilities :
  - void TCODImage::clear(const TCODColor col)
  - void TCODImage::invert()
  - void TCODImage::hflip()
  - void TCODImage::vflip()
  - void TCODImage::scale(int neww, int newh)
  - void TCODImage::setKeyColor(const TCODColor keyColor)
  - int TCODImage::getAlpha(int x,int y) const
  - bool TCODImage::isPixelTransparent(int x, int y) const
- can now dynamically modify the content of the font bitmap:
  `static void TCODSystem::updateChar(int asciiCode, int fontx, int fonty,const TCODImage *img,int x,int y)`
- added C and Python code generation in the heightmap tool
- added function to calculate the height of a printed string
  (with autowrapping) without actually printing it:
  - `int TCODConsole::getHeightLeftRect(int x, int y, int w, int h, const char *fmt, ...)`
  - `int TCODConsole::getHeightRightRect(int x, int y, int w, int h, const char *fmt, ...)`
  - `int TCODConsole::getHeightCenterRect(int x, int y, int w, int h, const char *fmt, ...)`
- parser : now strings properties can be split. The parser concatenates them:
  ```c
  myProperty = "string value can use "
               "multi line layout"
  ```
- parser: added missing `getCharProperty` when using default listener
- heightmap: added missing `TCOD_heightmap_get_value` function
- now support fonts with unlimited number of characters
  (was limited to 1024 in 1.4.0)
- added callback-based atomic bresenham function
  `static bool TCODLine::line(int xFrom, int yFrom, int xTo, int yTo, TCODLineListener *listener)`
- added `TCODMap::copy` function
- added TCODList fast remove functions (don't preserve the list order)
  - `template<class T> void TCODList::removeFast(const T elt)`
  - `template<class T> T * TCODList::removeFast(T *elt)`

### Changed
- Default colors have been changed/added.
- `TCODMap::computeFov` changed to support several algorithms:
  `void computeFov(int playerX,int playerY, int maxRadius,bool light_walls = true, TCOD_fov_algorithm_t algo = FOV_BASIC)`
  available algorithms :
  - FOV_BASIC: classic libtcod fov
  - FOV_DIAMOND: http://www.geocities.com/temerra/los_rays.html
  - FOV_SHADOW: http://roguebasin.roguelikedevelopment.org/index.php?title=FOV_using_recursive_shadowcasting
  - FOV_PERMISSIVE: http://roguebasin.roguelikedevelopment.org/index.php?title=Precise_Permissive_Field_of_View
  - light_walls: if false, wall cells are not put in the fov
- `setCustomFont` changed:
  defines the font number of characters instead of character size.
  if not specified, it is deduced from the layout flag
  (16x16 for ascii, 32x8 for tcod)
  `setCustomFont(const char *fontFile, int flags=TCOD_FONT_LAYOUT_ASCII_INCOL,int nbCharHoriz=0, int nbCharVertic=0)`
  The flag values have changed too:
   - TCOD_FONT_LAYOUT_ASCII_INCOL=1
   - TCOD_FONT_LAYOUT_ASCII_INROW=2
   - TCOD_FONT_TYPE_GREYSCALE=4
   - TCOD_FONT_TYPE_GRAYSCALE=4
   - TCOD_FONT_LAYOUT_TCOD=8
- pathfinding: you don't need to provide coordinates in x,y parameters of
  `Path::walk`
- improved double/simple walls special characters in tcod layout fonts

### Fixed
- fixed SDL dependent features not being available before initRoot is called.
  If you want to draw on an offscreen console without calling initRoot, you
  need to call at least setCustomFont to initialize the font. Else characters won't be rendered.
- fixed standalone credits page erasing previously fps limit
- fixed special characters TCOD_CHAR_DTEE* and TCOD_CHAR_DCROSS not printing correctly
- fixed heightmap tool generated code not compiling
- fixed parser C++ error function not prepending file name & line number to the error message
- fixed memory leak in pathfinding
- fixed fov issue with walkable, non transparent cells
- fixed numerical stability issues with heightmap rain erosion
- fixed calculation error in heightmap kernel transformation function
- fixed TCODConsole::renderCredits being able to render the credits only once
- fixed mouse cx,cy coordinates in "padded" fullscreen modes
- fixed mouse cursor hidden when switching fullscreen
- fixed mouse coordinates when going fullscreen->windowed

## [1.4.0] - 2008-10-16
### Added
- added pathfinding module
- added BSP module
- added heightmap module + heightmap tool
- added compression toolkit
- added possibility to pre-allocate TCODList's memory
- added support for PNG images + antialiased fonts from PNG with alpha channel or greyscale
- added ASCII mapping functions to map an ascii code to any character in your font:
  - `static void TCODConsole::mapAsciiCodeToFont(int asciiCode, int fontCharX, int fontCharY);`
  - `static void TCODConsole::mapAsciiCodesToFont(int firstAsciiCode, int nbCodes, int fontCharX, int fontCharY);`
  - `static void TCODConsole::mapStringToFont(const char *s, int fontCharX, int fontCharY);`
- parser : now strings can contain octal/hexadecimal ascii values
  - myStringProperty1 = "embedded hex value : \x80"
  - myStringProperty2 = "embedded octal value : \200"
- parser : now can handle list properties.
  ```c
  myStruct {
    myProperty = [1, 2, 3]
  }
  ```
  to declare: `struct.addListProperty("myProperty",TCOD_TYPE_INT,true);`
  to read (default parser): `TCODList<int> prop(parser.getListProperty("myStruct.myProperty", TCOD_TYPE_INT));`
- added color map generator. Interpolate colors from an array of key colors :
   `TCODColor::genMap(TCODColor *map, int nbKey, TCODColor const *keyColor, int const *keyIndex)`
- added random generator backup function. You can save the state of a generator
  with `TCODRandom *TCODRandom::save()` and restore it with
  `void TCODRandom::restore(const TCODRandom *backup)`
- added simplex noise, similar to perlin but faster,
  especially in 4 dimensions and has better contrast:
  - `TCODNoise::getSimplex`
  - `TCODNoise::getFbmSimplex`
  - `TCODNoise::getTurbulenceSimplex`
  Simplex should be preferred over Perlin for most usages.
- added wavelet noise, similar to perlin, much slower but doesn't blur at high scales. Doesn't work in 4D :
  - `TCODNoise::getWavelet`
  - `TCODNoise::getFbmWavelet`
  - `TCODNoise::getTurbulenceWavelet`
  Noise functions relative times:

  |       |2D|3D|4D|
  |:------|-:|-:|-:|
  |simplex| 1| 1| 1|
  |perlin | 4| 5|17|
  |wavelet|32|14| X|
- you can capture a console to an existing image without creating a new image
  with `TCODImage::refreshConsole`
- non rectangular offscreen consoles:
  you can define a key color on offscreen consoles.
  Cells for which background color = key color are not blitted
- added libtcod credits function. You can either call:
  `TCODConsole::credits()` to print the credits page at the start of your game
  (just after `initRoot`) or
  `bool TCODConsole::renderCredits(int x, int y, bool alpha)` to print the
  credits on top of one of your existing page (your game menu for example).
  Call it until it returns true.

### Changed
- Noise functions renamed:
  - `TCODNoise::getNoise` -> `TCODNoise::getPerlin`
  - `TCODNoise::getFbm` -> `TCODNoise::getFbmPerlin`
  - `TCODNoise::getTurbulence` -> `TCODNoise::getTurbulencePerlin`
- Some special char constants `TCOD_CHAR_x` added/removed
- `setCustomFont` changed:
  `setCustomFont(const char *fontFile,int charWidth=8, int charHeight=8, int flags=0)`
  flags:
  - TCOD_FONT_LAYOUT_ASCII_INROW=1
  - TCOD_FONT_TYPE_GREYSCALE=2
  - TCOD_FONT_LAYOUT_TCOD=4
- improved `TCODConsole::flush` performances:
   frames per second for sample "True colors" (terminal.png font) on my
   reference computer (windowed mode):
   - libtcod 1.3.2:  130
   - libtcod 1.4.0:  300
- `TCODNoise::getTurbulence` is twice faster
- improved mouse click detection. Even with a low framerate, mouse clicks will
  always be detected and reported in the `Xbutton_pressed` fields of
  `TCOD_mouse_t`
- you don't need anymore to provide values in `TCODLine::step` parameters

### Fixed
- fixed memory leak in image module
- fixed DELETE and INSERT keys not detected
- fixed a rendering bug when using a white background color before any other
  background color

## [1.3.2] - 2008-07-14
### Added
- added documentation for the generic container module `TCODList`

### Fixed
- fixed not being able to open and close the root console more than once
- fixed parser not being able to attach a sub-structure to more than one structure
- fixed TCOD_image_from_console not working with root console on C version
- fixed TCODParser::newStruct, addFlag, addProperty handling only static names
- fixed web color parser (#rrggbb) in the parser module
- fixed TCODImage constructor / TCOD_image_new crashing if root console not initialized
- fixed mouse status not updated if the keyboard events are not read with checkForKeypress/waitForKeypress
- fixed fbm, turbulence functions returning NaN for high octaves values

## [1.3.1] - 2008-06-05
### Added
- added default parser listener for basic config files.
- added fields in `TCOD_mouse_t` to easily detect button press events:
  - `lbutton_pressed`: left button pressed event
  - `rbutton_pressed`: right button pressed event
  - `mbutton_pressed`: middle button pressed event

  and to get the mouse movement in cell coordinates (instead of pixels)
- added functions to retrieve data from TCODMap

### Changed
- now TCODConsole::putChar/setChar can use ascii code > 255 (ok, it's no more an ascii code, then)
  this allows you to use fonts with more than 255 characters.

### Fixed
- fixed const correctness in `TCODConsole::print*Rect` functions
- fixed a bug in fov toolkit if TCODMap width < height
- fixed `TCOD_struct_get_type` returning `TCOD_TYPE_NONE` instead of
  `TCOD_TYPE_BOOL` for flags.

## [1.3] - 2008-05-25
### Added
- added mouse support
- added the file parser module
- added TCODColor::setHSV(float h, float s, float v) and TCODColor::getHSV(float *h, float *s, float *v)
- added TCODColor addition and scalar multiplication. All r,g,b values are clamped to [0-255] :
  - C++: `color1 = color2 * 3.5f;`
         `color1 = color1 + color2;`
  - C  : `color1 = TCOD_color_multiply_scalar(color2, 3.5f);`
         `color1 = TCOD_color_add(color1, color2);`
- added `TCODConsole::setKeyboardRepeat(int initialDelay, int interval)` and
  `TCODConsole::disableKeyboardRepeat()`
- added `TCODSystem::getCurrentResolution(int *w, int *h)`

### Changed
- C++ : colors must be initialized by constructor:
  - 1.2.2 : `TCODColor mycol={r,g,b};`
  - 1.3   : `TCODColor mycol(r,g,b);`
- TCOD_console_check_for_keypress now has a parameter that indicates which events are tracked
  - 1.2.2 : `key = TCOD_console_check_for_keypress();`
  - 1.3   : `key = TCOD_console_check_for_keypress(TCOD_KEY_PRESSED);`
- now `TCODFov::computeFov` takes a maxRadius parameter.
  Use 0 for unlimited range (default)
- the mouse cursor is now automatically hidden when using fullscreen
- fixed closing the window resulting in a fake 'Q' keyboard event
- fixed `TCODConsole::print*` and `TCODConsole::rect` functions crashing when
  printing out of the console
- fixed `f` parameter modified when calling fbm and turbulence noise functions.
  Now `f` is no more modified.
- fixed wrong ascii code in `TCOD_key_t.c` when pressing Control and a letter
  key.

## [1.2.2] - 2008-03-18
### Added
- added helpers for real time games
  - `void TCODSystem::setFps(int val)` to limit the number of frames per second.
    Use 0 for unlimited fps (default)
  - `int TCODSystem::getFps()` to return the number of frames rendered during
    the last second
  - `float TCODSystem::getLastFrameLength()` to return the length of the last
    frame in seconds
- added `TCODImage::clear` to fill an image with a color

### Changed
- `TCODConsole::hline` and `vline` now have a `TCOD_bkgnd_flag_t` parameter
- now the `TCODConsole::print*Rect` functions return the height (number of console lines) of the printed string

### Fixed
- fixed TCODConsole::print*Rect functions not truncating the string if it reaches the bottom of the rectangle
  using a rectangle height of 0 means unlimited height
- fixed a color bug when drawing text using the black color (0,0,0)
- fixed TCODConsole::waitForKeypress(true) resulting in Alt/Ctrl/Shift key state not cleared

## [1.2.1] - 2008-03-09
### Added
- added TCODImage::blitRect to easily map an image on a specific part of a console
- added possibility to generate an image from an offscreen console
- added TCODImage::save
- image toolkit now support reading 8bpp bitmap files.

### Changed
- Random number generators now support inverted ranges instead of crashing:
  `TCODRandom::getInstance()->getInt(10,2)`
  => return a value between 2 and 10.

### Fixed
- fixed image toolkit replacing the green component by the red one when loading a bitmap from a file
- fixed console print*Rect functions unnecessarily splitting the string in some cases
- on Linux, you don't need to link with SDL anymore when using libtcod/libtcod++
- fixed linker issues with Visual Studio

## [1.2] - 2008-02-26
### Added
- new 'image' toolkit replacing some of the features previously in the 'system' toolkit.
- now windowed mode works even on 16bits desktops
- improved custom font support.
  You can now use fonts with characters ordered in rows or columns and with a custom transparent color.
  This allows you to use most existing character sets with the doryen library.
  Font characters with grayscale are still not supported.
- new time functions:
  - `uint32 TCODSystem::getElapsedMilli()`
  - `float TCODSystem::getElapsedSeconds()`
- new background blending modes (see the line sample for a demo) :
  - TCOD_BKGND_LIGHTEN : newbk = MAX(oldbk,curbk)
  - TCOD_BKGND_DARKEN  : newbk = MIN(oldbk,curbk)
  - TCOD_BKGND_SCREEN  : newbk = white - (white - oldbk) * (white - curbk) // inverse of multiply : (1-newbk) = (1-oldbk)*(1-curbk)
  - TCOD_BKGND_COLOR_DODGE : newbk = curbk / (white - oldbk)
  - TCOD_BKGND_COLOR_BURN : newbk = white - (white - oldbk) / curbk
  - TCOD_BKGND_ADD : newbk = oldbk + curbk
  - TCOD_BKGND_ADDALPHA(alpha) : newbk = oldbk + alpha * curbk, 0.0<=alpha<=1.0
  - TCOD_BKGND_BURN : newbk = oldbk + curbk - white
  - TCOD_BKGND_OVERLAY : newbk = curbk <= 0.5 ? 2*curbk*oldbk : white - 2*(white-curbk)*(white-oldbk)
  - TCOD_BKGND_ALPHA(alpha) : newbk = (1.0f-alpha)*oldbk + alpha*(curbk-oldbk), 0.0<=alpha<=1.0
- The samples can now use custom bitmap fonts / screen resolution. Use following syntax :
  sample_c[pp] [options]
  options :
  * -fullscreen : start directly in fullscreen mode
  * -font <font_filename> (default "terminal.bmp")
  * -font-char-size <char_width> <char_height> (default 8 8)
  * -font-layout <nb_char_horiz> <nb_char_vertic> (default 16 16)
  * -font-in-row : characters in the bitmap are ordered in rows (default : columns)
  * -font-key-color <r> <g> <b> : transparent color, r,g,b between 0 and 255. (default 0 0 0)
  * -fullscreen-resolution <width> <height> (default 640 400)

### Changed
- headers renamed from `tcodlib.h*` to `libtcod.h*`
- on Linux, the library is split into `libtcod.so` and `libtcod++.so`
  allowing C developers to use it without installing g++
- the font name is no more a parameter of `TCODConsole::initRoot`
- `TCODConsole::setBitmapFontSize` renamed to `TCODConsole::setCustomFont`
- `TCODConsole::printFrame` is now a variadic function, like the other text output functions
- some background blending flags have been renamed:
  - `TCOD_BKGND_NOBK` replaced by `TCOD_BKGND_NONE`
  - `TCOD_BKGND_BK` replaced by `TCOD_BKGND_SET`
- the fov toolkit now uses two properties per cell : `transparent`/`walkable`.
  This is necessary to fix a fov issue with window cells (transparent, but not walkable).
  `void TCODFov::setTransparent(int x,int y, bool isTransparent)` has been replaced by :
  `void TCODFov::setProperties(int x,int y, bool isTransparent, bool isWalkable)`
- improved const correctness, added some default parameter values in the C++ API.

### Fixed
- fixed the window size when using a custom font size
- fixed `TCODK_PRINTSCREEN` key event not detected
- fixed crash in printing functions if the string length was > 255.
  Now they can handle strings of any size.

## [1.1] - 2008-01-27
### Added
- added the noise toolkit
- added the field of view toolkit
- added customizable bitmap font size

## [1.0.1] - 2008-01-19
### Added
- added C/C++ samples

### Changed
- `TCODConsole::waitForKeyPress` now has a bool parameter indicating if we
  flush the keyboard buffer before waiting.

### Fixed
- fixed a color bug when drawing text using the grey color (196,196,196)
- fixed wrong key codes returned by wait/checkForKeyPress on some keyboard layouts

## [1.0] - 2008-01-05
- Initial release
