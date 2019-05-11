/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/** \file libtcod.cpp
 *
 *  To statically link a C++ program with libtcod you'll need to follow
 *  the directions for compiling `libtcod_c.c` and then add `libtcod.cpp` to
 *  your source files.
 */
#include "libtcod/bresenham.cpp"
#include "libtcod/bsp.cpp"
#include "libtcod/color.cpp"
#include "libtcod/console.cpp"
#include "libtcod/console_c.cpp"
#include "libtcod/deprecated.cpp"
#include "libtcod/fov.cpp"
#include "libtcod/heightmap.cpp"
#include "libtcod/image.cpp"
#include "libtcod/image_c.cpp"
#include "libtcod/lex.cpp"
#include "libtcod/mersenne.cpp"
#include "libtcod/mouse.cpp"
#include "libtcod/namegen.cpp"
#include "libtcod/noise.cpp"
#include "libtcod/parser.cpp"
#include "libtcod/path.cpp"
#include "libtcod/sys.cpp"
#include "libtcod/sys_c.cpp"
#include "libtcod/sys_opengl_c.cpp"
#include "libtcod/sys_sdl_c.cpp"
#include "libtcod/sys_sdl_img_bmp.cpp"
#include "libtcod/sys_sdl_img_png.cpp"
#include "libtcod/sys_sdl2_c.cpp"
#include "libtcod/txtfield.cpp"
#include "libtcod/wrappers.cpp"
#include "libtcod/zip.cpp"

#include "libtcod/color/canvas.cpp"
#include "libtcod/console/console.cpp"
#include "libtcod/console/drawing.cpp"
#include "libtcod/console/printing.cpp"
#include "libtcod/console/rexpaint.cpp"
#include "libtcod/engine/backend.cpp"
#include "libtcod/engine/display.cpp"
#include "libtcod/engine/error.cpp"
#include "libtcod/engine/globals.cpp"
#include "libtcod/pathfinding/astar.cpp"
#include "libtcod/pathfinding/generic.cpp"
#include "libtcod/pathfinding/dijkstra.cpp"
#include "libtcod/sdl2/event.cpp"
#include "libtcod/sdl2/gl_alias.cpp"
#include "libtcod/sdl2/gl2_display.cpp"
#include "libtcod/sdl2/gl2_raii.cpp"
#include "libtcod/sdl2/gl2_renderer.cpp"
#include "libtcod/sdl2/legacy_backend.cpp"
#include "libtcod/sdl2/sdl2_alias.cpp"
#include "libtcod/sdl2/sdl2_display.cpp"
#include "libtcod/sdl2/sdl2_renderer.cpp"
#include "libtcod/tileset/fallback.cpp"
#include "libtcod/tileset/observer.cpp"
#include "libtcod/tileset/tile.cpp"
#include "libtcod/tileset/tileset.cpp"
#include "libtcod/tileset/tilesheet.cpp"
#include "libtcod/tileset/truetype.cpp"

#include "libtcod/gui/button.cpp"
#include "libtcod/gui/container.cpp"
#include "libtcod/gui/flatlist.cpp"
#include "libtcod/gui/hbox.cpp"
#include "libtcod/gui/image.cpp"
#include "libtcod/gui/label.cpp"
#include "libtcod/gui/radiobutton.cpp"
#include "libtcod/gui/slider.cpp"
#include "libtcod/gui/statusbar.cpp"
#include "libtcod/gui/textbox.cpp"
#include "libtcod/gui/togglebutton.cpp"
#include "libtcod/gui/toolbar.cpp"
#include "libtcod/gui/vbox.cpp"
#include "libtcod/gui/widget.cpp"
