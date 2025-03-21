/* BSD 3-Clause License
 *
 * Copyright © 2008-2025, Jice and the libtcod contributors.
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
#pragma once
#ifndef LIBTCOD_H_
#define LIBTCOD_H_

#include "bresenham.h"
#include "bsp.h"
#include "color.h"
#include "console.h"
#include "console_drawing.h"
#include "console_etc.h"
#include "console_init.h"
#include "console_printing.h"
#include "console_rexpaint.h"
#include "context.h"
#include "context_init.h"
#include "error.h"
#include "fov.h"
#include "globals.h"
#include "heightmap.h"
#include "image.h"
#include "lex.h"
#include "list.h"
#include "logging.h"
#include "mersenne.h"
#include "mouse.h"
#include "namegen.h"
#include "noise.h"
#include "parser.h"
#include "path.h"
#include "pathfinder.h"
#include "pathfinder_frontier.h"
#include "portability.h"
#include "random.h"
#include "renderer_sdl2.h"
#include "sdl2/event.h"
#include "sys.h"
#include "tileset.h"
#include "tileset_bdf.h"
#include "tileset_fallback.h"
#include "tileset_render.h"
#include "tileset_truetype.h"
#include "tree.h"
#include "txtfield.h"
#include "utility.h"
#include "version.h"
#include "zip.h"

#ifdef __cplusplus
#include "bresenham.hpp"
#include "bsp.hpp"
#include "color.hpp"
#include "console.hpp"
#include "console_printing.hpp"
#include "console_rexpaint.hpp"
#include "console_types.hpp"
#include "context.hpp"
#include "fov.hpp"
#include "heightmap.hpp"
#include "image.hpp"
#include "lex.hpp"
#include "list.hpp"
#include "mersenne.hpp"
#include "mouse.hpp"
#include "namegen.hpp"
#include "noise.hpp"
#include "parser.hpp"
#include "path.hpp"
#include "sys.hpp"
#include "tileset.hpp"
#include "tileset_bdf.hpp"
#include "tileset_fallback.hpp"
#include "tree.hpp"
#include "txtfield.hpp"
#include "zip.hpp"
#endif  // __cplusplus

#endif  // LIBTCOD_H_
