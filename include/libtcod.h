/*
* libtcod 1.4.0
* Copyright (c) 2008 J.C.Wilk
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of J.C.Wilk may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY J.C.WILK ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL J.C.WILK BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _TCODLIB_H
#define _TCODLIB_H

// os / compiler identification
#if defined( _MSC_VER )
#define VISUAL_STUDIO
#ifndef WIN32
#define WIN32
#endif
#elif defined( __MINGW32__ )
#ifndef MINGW32
#define MINGW32
#endif
#ifndef WIN32
#define WIN32
#endif
#elif defined( __BORLANDC__ )
#define BORLANDC
#elif defined( __linux )
#define LINUX
#endif

// base types
typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef int int32;

// bool support for C
#ifndef __cplusplus
typedef enum { false, true } bool;
#endif

#ifdef VISUAL_STUDIO
#define strdup _strdup
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif
#if defined( VISUAL_STUDIO ) || defined( MINGW32 )
char *strcasestr (const char *haystack, const char *needle);
#endif

// DLL export
#ifdef WIN32
#ifdef LIBTCOD_EXPORTS
#define TCODLIB_API __declspec(dllexport)
#else
#define TCODLIB_API __declspec(dllimport)
#endif
#else
#define TCODLIB_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/******************************************
 utility macros
 ******************************************/
#define MAX(a,b) ((a)<(b)?(b):(a))
#define MIN(a,b) ((a)>(b)?(b):(a))
#define ABS(a) ((a)<0?-(a):(a))
#define CLAMP(a, b, x)		((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))
#define LERP(a, b, x) ( a + x * (b - a) )

#include "list.h"
#include "color.h"
#include "console.h"
#include "sys.h"
#include "mersenne.h"
#include "mouse.h"
#include "bresenham.h"
#include "noise.h"
#include "fov.h"
#include "path.h"
#include "image.h"
#include "lex.h"
#include "parser.h"
#include "tree.h"
#include "bsp.h"
#include "heightmap.h"
#include "zip.h"
#ifdef __cplusplus
}
#endif

#endif
