/* libtcod
 * Copyright © 2008-2019 Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The name of copyright holder nor the names of its contributors may not
 *       be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _LIBTCOD_H
#define _LIBTCOD_H

#include "portability.h"
#include "utility.h"
#include "version.h"

#include "bresenham.h"
#include "bsp.h"
#include "color.h"
#include "console.h"
#include "fov.h"
#include "heightmap.h"
#include "image.h"
#include "lex.h"
#include "list.h"
#include "mersenne.h"
#include "mouse.h"
#include "namegen.h"
#include "noise.h"
#include "path.h"
#include "parser.h"
#include "sys.h"
#include "tree.h"
#include "txtfield.h"
#include "zip.h"

#include "console/drawing.h"
#include "console/printing.h"
#include "console/rexpaint.h"

#include "engine/backend.h"
#include "engine/display.h"
#include "engine/globals.h"

#include "tileset/observer.h"
#include "tileset/tileset.h"
#include "tileset/tile.h"

#ifdef __cplusplus
#include "bresenham.hpp"
#include "bsp.hpp"
#include "color.hpp"
#include "console.hpp"
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
#include "tree.hpp"
#include "txtfield.hpp"
#include "zip.hpp"
#endif // __cplusplus

#endif
