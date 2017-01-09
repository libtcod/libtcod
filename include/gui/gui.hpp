/*
* libtcod 1.6.3
* Copyright (c) 2008,2009,2010,2012,2013,2016,2017 Jice & Mingos & rmtew
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice or Mingos may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
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
#ifndef _GUI_HPP
#define _GUI_HPP

#include "libtcod.hpp"

#ifdef TCOD_VISUAL_STUDIO
#pragma warning(disable:4996)
#pragma warning(disable:4251)
#endif

// DLL export
#ifdef TCOD_WINDOWS
#ifdef LIBTCOD_GUI_EXPORTS
#define TCODLIB_GUI_API __declspec(dllexport)
#else
#define TCODLIB_GUI_API __declspec(dllimport)
#endif
#else
#define TCODLIB_GUI_API
#endif

#include "widget.hpp"
#include "button.hpp"
#include "radiobutton.hpp"
#include "container.hpp"
#include "label.hpp"
#include "statusbar.hpp"
#include "textbox.hpp"
#include "flatlist.hpp"
#include "slider.hpp"
#include "togglebutton.hpp"
#include "toolbar.hpp"
#include "vbox.hpp"
#include "hbox.hpp"
#include "image.hpp"

#endif

