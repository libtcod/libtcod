#ifndef _TCODLIB_H
#define _TCODLIB_H

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

#ifdef VISUAL_STUDIO
#define strdup _strdup
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
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
#include "perlin.h"
#include "fov.h"
#include "image.h"
#include "lex.h"
#include "parser.h"
#include "tree.h"

#ifdef __cplusplus
}
#endif

#endif
