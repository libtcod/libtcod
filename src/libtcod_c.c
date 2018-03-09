/*
* libtcod
* Copyright (c) 2018 Jice & Mingos & rmtew
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

/** \file libtcod_c.c
 *
 *  To statically link a C program with libtcod:
 *
 *  * Add `libtcod_c.c` to your list of source files to compile.
 *  * Add libtcod's include directory.
 *  * Add `SDL2` to be dynamically linked, and add its include directory.
 *    You can get the development libraries from:
 *    https://www.libsdl.org/download-2.0.php
 *  * Add `GL` to be dynamically linked.
 *  * Compile and link libtcod's src/png/lodepng.c file.
 *  * Link with `zlib`, and add its include directory.  You can include
 *    libtcod's src/zlib directory and compile all `.c` files in there to
 *    statically link it.
 *  * If you're using C++ then add `libtcod.cpp` to your source files.
 *
 *  You can define `NO_OPENGL` to remove the dependency on the GL library.
 *  You can also define `TCOD_BARE` to remove the dependency on SDL2 and GL.
 */
#include "bresenham_c.c"
#include "bsp_c.c"
#include "color_c.c"
#include "console_c.c"
#include "console_rexpaint.c"
#include "fov_c.c"
#include "fov_circular_raycasting.c"
#include "fov_diamond_raycasting.c"
#include "fov_permissive2.c"
#include "fov_recursive_shadowcasting.c"
#include "fov_restrictive.c"
#include "heightmap_c.c"
#include "image_c.c"
#include "lex_c.c"
#include "list_c.c"
#include "mersenne_c.c"
#include "namegen_c.c"
#include "noise_c.c"
#include "parser_c.c"
#include "path_c.c"
#include "sys_c.c"
#include "tree_c.c"
#include "txtfield_c.c"
#include "wrappers.c"
#include "zip_c.c"

#ifndef TCOD_BARE
#include "sys_sdl_c.c"
#include "sys_sdl_img_bmp.c"
#include "sys_sdl_img_png.c"
#include "sys_sdl2_c.c"

#ifndef NO_OPENGL
#include "sys_opengl_c.c"
#endif /* NO_OPENGL */

#endif /* TCOD_BARE */
