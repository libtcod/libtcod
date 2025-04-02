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
#ifndef LIBTCOD_VERSION_H
#define LIBTCOD_VERSION_H

#define TCOD_MAJOR_VERSION 2
#define TCOD_MINOR_VERSION 1
#define TCOD_PATCHLEVEL 1

#define TCOD_STRVERSION "2.1.1"

/**
    Converts version numbers into a numeric value.
    \rst
    (1, 2, 3) -> 0x10203

    .. versionadded:: 1.16
    \endrst
 */
#define TCOD_VERSIONNUM(major, minor, patch) ((major) * 0x010000 + (minor) * 0x0100 + (patch))

/**
    The version of libtcod currently being compiled.
    \rst
    .. versionadded:: 1.16
    \endrst
 */
#define TCOD_COMPILEDVERSION TCOD_VERSIONNUM(TCOD_MAJOR_VERSION, TCOD_MINOR_VERSION, TCOD_PATCHLEVEL)

/**
    \rst
    .. deprecated:: 1.16
    \endrst
 */
#define TCOD_HEXVERSION TCOD_COMPILEDVERSION

/**
    \rst
    .. deprecated:: 1.16
    \endrst
 */
#define TCOD_TECHVERSION (TCOD_HEXVERSION * 0x100)

/**
    \rst
    .. deprecated:: 1.16
    \endrst
 */
#define TCOD_STRVERSIONNAME "libtcod " TCOD_STRVERSION
/***************************************************************************
    @brief Returns true if the compiled version of libtcod is at least (major, minor).

    \rst
    .. versionadded:: 1.19
    \endrst
 */
#define TCOD_VERSION_ATLEAST(major, minor) (TCOD_COMPILEDVERSION >= TCOD_VERSIONNUM(major, minor, 0))

#endif /* LIBTCOD_VERSION_H */
