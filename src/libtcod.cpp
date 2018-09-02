/*
* libtcod
* Copyright (c) 2008-2018 Jice & Mingos & rmtew
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice or Mingos may not be used to endorse or promote
*       products derived from this software without specific prior written
*       permission.
*
* THIS SOFTWARE IS PROVIDED BY JICE, MINGOS AND RMTEW ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE, MINGOS OR RMTEW BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/** \file libtcod.cpp
 *
 *  To statically link a C++ program with libtcod you'll need to follow
 *  the directions for compiling `libtcod_c.c` and then add `libtcod.cpp` to
 *  your source files.
 */
#include "bresenham.cpp"
#include "bsp.cpp"
#include "color.cpp"
#include "console.cpp"
#include "console_c.cpp"
#include "fov.cpp"
#include "heightmap.cpp"
#include "image.cpp"
#include "lex.cpp"
#include "mersenne.cpp"
#include "mouse.cpp"
#include "namegen.cpp"
#include "noise.cpp"
#include "parser.cpp"
#include "path.cpp"
#include "sys.cpp"
#include "sys_c.cpp"
#include "txtfield.cpp"
#include "zip.cpp"

#ifndef TCOD_BARE
#include "sys_sdl_c.cpp"
#include "sys_sdl2_c.cpp"
#include "sys_sdl_img_bmp.cpp"
#include "sys_sdl_img_png.cpp"
#ifndef NO_OPENGL
#include "sys_opengl_c.cpp"
#endif /* NO_OPENGL */
#endif /* TCOD_BARE */

#include "backend/backend.cc"
#include "sdl2/legacy_backend.cc"
#include "tileset/observer.cc"
#include "tileset/tile.cc"
#include "tileset/tileset.cc"
#include "tileset/tilesheet.cc"

/*
#include "gui/button.cpp"
#include "gui/container.cpp"
#include "gui/flatlist.cpp"
#include "gui/hbox.cpp"
#include "gui/image.cpp"
#include "gui/label.cpp"
#include "gui/radiobutton.cpp"
#include "gui/slider.cpp"
#include "gui/statusbar.cpp"
#include "gui/textbox.cpp"
#include "gui/togglebutton.cpp"
#include "gui/toolbar.cpp"
#include "gui/vbox.cpp"
#include "gui/widget.cpp"
*/
