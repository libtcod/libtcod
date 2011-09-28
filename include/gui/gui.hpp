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

