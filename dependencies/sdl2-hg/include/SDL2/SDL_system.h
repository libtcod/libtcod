/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2012 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/**
 *  \file SDL_system.h
 *  
 *  Include file for platform specific SDL API functions
 */

#ifndef _SDL_system_h
#define _SDL_system_h

#include "SDL_stdinc.h"

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
/* *INDENT-OFF* */
extern "C" {
/* *INDENT-ON* */
#endif

#if __IPHONEOS__

#include "SDL_video.h"

extern DECLSPEC int SDLCALL SDL_iPhoneSetAnimationCallback(SDL_Window * window, int interval, void (*callback)(void*), void *callbackParam);
extern DECLSPEC void SDLCALL SDL_iPhoneSetEventPump(SDL_bool enabled);

extern DECLSPEC int SDLCALL SDL_iPhoneKeyboardShow(SDL_Window * window);
extern DECLSPEC int SDLCALL SDL_iPhoneKeyboardHide(SDL_Window * window);
extern DECLSPEC SDL_bool SDLCALL SDL_iPhoneKeyboardIsShown(SDL_Window * window);
extern DECLSPEC int SDLCALL SDL_iPhoneKeyboardToggle(SDL_Window * window);

#endif

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
/* *INDENT-OFF* */
}
/* *INDENT-ON* */
#endif
#include "close_code.h"

#endif /* _SDL_system_h */

/* vi: set ts=4 sw=4 expandtab: */
