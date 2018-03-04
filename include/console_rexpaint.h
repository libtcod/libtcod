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

#ifndef TCOD_CONSOLE_REXPAINT_H_
#define TCOD_CONSOLE_REXPAINT_H_

#include "libtcod_portability.h"
#include "console.h"

#ifdef __cplusplus
extern "C" {
#endif

TCODLIB_API TCOD_console_t TCOD_console_from_xp(const char *filename);
TCODLIB_API bool TCOD_console_load_xp(TCOD_console_t con, const char *filename);
TCODLIB_API bool TCOD_console_save_xp(TCOD_console_t con,
                                      const char *filename, int compress_level);
TCODLIB_API TCOD_list_t TCOD_console_list_from_xp(const char *filename);
TCODLIB_API bool TCOD_console_list_save_xp(
    TCOD_list_t console_list, const char *filename, int compress_level);

#ifdef __cplusplus
}
#endif

#endif /* TCOD_CONSOLE_REXPAINT_H_ */
