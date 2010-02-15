#
# libtcod 1.5.0 python wrapper
# Copyright (c) 2008 Jice
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * The name of Jice may not be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY Jice ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL Jice BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

import sys
import ctypes
from ctypes import *

try:  #import NumPy if available
	import numpy
	numpy_available = True
except ImportError:
	numpy_available = False

if sys.platform.find('linux') != -1:
    _lib = ctypes.cdll['./libtcod.so']
else:
    _lib = ctypes.cdll['./libtcod-mingw.dll']

HEXVERSION = 0x010500
STRVERSION = "1.5.0"
TECHVERSION = 0x01050003

############################
# color module
############################
class Color(Structure):
    _fields_ = [('r', c_uint, 8),
                ('g', c_uint, 8),
                ('b', c_uint, 8),
                ]

    def __init__(self, r=0,g=0,b=0):
        self.r = r
        self.g = g
        self.b = b

    def __eq__(self, c):
        return (self.r == c.r) and (self.g == c.g) and (self.b == c.b)

    def __mul__(self, c):
	iret=0
        if isinstance(c,Color):
            iret=_lib.TCOD_color_multiply_wrapper(col_to_int(self), col_to_int(c))
        else:
            iret=_lib.TCOD_color_multiply_scalar_wrapper(col_to_int(self), c_float(c))
        return int_to_col(iret)

    def __add__(self, c):
        iret=_lib.TCOD_color_add_wrapper(col_to_int(self), col_to_int(c))
        return int_to_col(iret)

    def __sub__(self, c):
        iret=_lib.TCOD_color_subtract_wrapper(col_to_int(self), col_to_int(c))
        return int_to_col(iret)

def int_to_col(i) :
    c=Color()
    c.r=(i&0xFF0000)>>16
    c.g=(i&0xFF00)>>8
    c.b=(i&0xFF)
    return c

def col_to_int(c) :
    return (int(c.r) <<16) | (c.g<<8) | c.b;

# default colors
# grey levels
black=Color(0,0,0)
darker_grey=Color(31,31,31)
dark_grey=Color(63,63,63)
grey=Color(128,128,128)
light_grey=Color(191,191,191)
darker_gray=Color(31,31,31)
dark_gray=Color(63,63,63)
gray=Color(128,128,128)
light_gray=Color(191,191,191)
white=Color(255,255,255)

#standard colors
red=Color(255,0,0)
orange=Color(255,127,0)
yellow=Color(255,255,0)
chartreuse=Color(127,255,0)
green=Color(0,255,0)
sea=Color(0,255,127)
cyan=Color(0,255,255)
sky=Color(0,127,255)
blue=Color(0,0,255)
violet=Color(127,0,255)
magenta=Color(255,0,255)
pink=Color(255,0,127)

# dark colors
dark_red=Color(127,0,0)
dark_orange=Color(127,63,0)
dark_yellow=Color(127,127,0)
dark_chartreuse=Color(63,127,0)
dark_green=Color(0,127,0)
dark_sea=Color(0,127,63)
dark_cyan=Color(0,127,127)
dark_sky=Color(0,63,127)
dark_blue=Color(0,0,127)
dark_violet=Color(63,0,127)
dark_magenta=Color(127,0,127)
dark_pink=Color(127,0,63)

# darker colors
darker_red=Color(63,0,0)
darker_orange=Color(63,31,0)
darker_yellow=Color(63,63,0)
darker_chartreuse=Color(31,63,0)
darker_green=Color(0,63,0)
darker_sea=Color(0,63,31)
darker_cyan=Color(0,63,63)
darker_sky=Color(0,31,63)
darker_blue=Color(0,0,63)
darker_violet=Color(31,0,63)
darker_magenta=Color(63,0,63)
darker_pink=Color(63,0,31)

# light colors
light_red=Color(255,127,127)
light_orange=Color(255,191,127)
light_yellow=Color(255,255,127)
light_chartreuse=Color(191,255,127)
light_green=Color(127,255,127)
light_sea=Color(127,255,191)
light_cyan=Color(127,255,255)
light_sky=Color(127,191,255)
light_blue=Color(127,127,255)
light_violet=Color(191,127,255)
light_magenta=Color(255,127,255)
light_pink=Color(255,127,191)

# desaturated colors
desaturated_red=Color(127,63,63)
desaturated_orange=Color(127,95,63)
desaturated_yellow=Color(127,127,63)
desaturated_chartreuse=Color(95,127,63)
desaturated_green=Color(63,127,63)
desaturated_sea=Color(63,127,95)
desaturated_cyan=Color(63,127,127)
desaturated_sky=Color(63,95,127)
desaturated_blue=Color(63,63,127)
desaturated_violet=Color(95,63,127)
desaturated_magenta=Color(127,63,127)
desaturated_pink=Color(127,63,95)

# special
silver=Color(203,203,203)
gold=Color(255,255,102)

# color functions
def color_lerp(c1, c2, a):
    iret = _lib.TCOD_color_lerp_wrapper(col_to_int(c1), col_to_int(c2), c_float(a))
    return int_to_col(iret)

def color_set_hsv(c, h, s, v):
    _lib.TCOD_color_set_HSV(byref(c), c_float(h), c_float(s), c_float(v))

def color_get_hsv(c):
    h = c_float()
    s = c_float()
    v = c_float()
    _lib.TCOD_color_get_HSV(c, byref(h), byref(s), byref(v))
    return h.value, s.value, v.value

def color_gen_map(colors, indexes):
    COLOR_ARRAY = c_ubyte * (3 * len(colors))
    IDX_ARRAY = c_int * len(indexes)
    ccolors = COLOR_ARRAY()
    cindexes = IDX_ARRAY()
    maxidx = 0
    for i in range(len(colors)):
        ccolors[3 * i] = colors[i].r
        ccolors[3 * i + 1] = colors[i].g
        ccolors[3 * i + 2] = colors[i].b
        cindexes[i] = c_int(indexes[i])
        if indexes[i] > maxidx:
            maxidx = indexes[i]
    RES_ARRAY = c_ubyte * (3 * (maxidx + 1))
    cres = RES_ARRAY()
    _lib.TCOD_color_gen_map(cres, len(colors), ccolors, cindexes)
    res = list()
    i = 0
    while i < 3 * (maxidx + 1):
        col = Color(cres[i], cres[i + 1], cres[i + 2])
        res.append(col)
        i += 3
    return res

############################
# console module
############################
class Key(Structure):
    _fields_=[('vk', c_int, 32),
              ('c', c_int, 8),
              ('pressed', c_uint, 8),
              ('lalt', c_uint, 8),
              ('lctrl', c_uint, 8),
              ('ralt', c_uint, 8),
              ('rctrl', c_uint, 8),
              ('shift', c_uint, 8),
              ]

_lib.TCOD_console_wait_for_keypress.restype = Key
_lib.TCOD_console_check_for_keypress.restype = Key
_lib.TCOD_console_credits_render.restype = c_uint
_lib.TCOD_console_set_custom_font.argtypes=[c_char_p,c_int]

# background rendering modes
BKGND_NONE = 0
BKGND_SET = 1
BKGND_MULTIPLY = 2
BKGND_LIGHTEN = 3
BKGND_DARKEN = 4
BKGND_SCREEN = 5
BKGND_COLOR_DODGE = 6
BKGND_COLOR_BURN = 7
BKGND_ADD = 8
BKGND_ADDA = 9
BKGND_BURN = 10
BKGND_OVERLAY = 11
BKGND_ALPH = 12

def BKGND_ALPHA(a):
    return BKGND_ALPH | (int(a * 255) << 8)

def BKGND_ADDALPHA(a):
    return BKGND_ADDA | (int(a * 255) << 8)

# non blocking key events types
KEY_PRESSED = 1
KEY_RELEASED = 2
# key codes
KEY_NONE = 0
KEY_ESCAPE = 1
KEY_BACKSPACE = 2
KEY_TAB = 3
KEY_ENTER = 4
KEY_SHIFT = 5
KEY_CONTROL = 6
KEY_ALT = 7
KEY_PAUSE = 8
KEY_CAPSLOCK = 9
KEY_PAGEUP = 10
KEY_PAGEDOWN = 11
KEY_END = 12
KEY_HOME = 13
KEY_UP = 14
KEY_LEFT = 15
KEY_RIGHT = 16
KEY_DOWN = 17
KEY_PRINTSCREEN = 18
KEY_INSERT = 19
KEY_DELETE = 20
KEY_LWIN = 21
KEY_RWIN = 22
KEY_APPS = 23
KEY_0 = 24
KEY_1 = 25
KEY_2 = 26
KEY_3 = 27
KEY_4 = 28
KEY_5 = 29
KEY_6 = 30
KEY_7 = 31
KEY_8 = 32
KEY_9 = 33
KEY_KP0 = 34
KEY_KP1 = 35
KEY_KP2 = 36
KEY_KP3 = 37
KEY_KP4 = 38
KEY_KP5 = 39
KEY_KP6 = 40
KEY_KP7 = 41
KEY_KP8 = 42
KEY_KP9 = 43
KEY_KPADD = 44
KEY_KPSUB = 45
KEY_KPDIV = 46
KEY_KPMUL = 47
KEY_KPDEC = 48
KEY_KPENTER = 49
KEY_F1 = 50
KEY_F2 = 51
KEY_F3 = 52
KEY_F4 = 53
KEY_F5 = 54
KEY_F6 = 55
KEY_F7 = 56
KEY_F8 = 57
KEY_F9 = 58
KEY_F10 = 59
KEY_F11 = 60
KEY_F12 = 61
KEY_NUMLOCK = 62
KEY_SCROLLLOCK = 63
KEY_SPACE = 64
KEY_CHAR = 65
# special chars
# single walls
CHAR_HLINE = 196
CHAR_VLINE = 179
CHAR_NE = 191
CHAR_NW = 218
CHAR_SE = 217
CHAR_SW = 192
CHAR_TEEW = 180
CHAR_TEEE = 195
CHAR_TEEN = 193
CHAR_TEES = 194
CHAR_CROSS = 197
# double walls
CHAR_DHLINE = 205
CHAR_DVLINE = 186
CHAR_DNE = 187
CHAR_DNW = 201
CHAR_DSE = 188
CHAR_DSW = 200
CHAR_DTEEW = 185
CHAR_DTEEE = 204
CHAR_DTEEN = 202
CHAR_DTEES = 203
CHAR_DCROSS = 206
# blocks
CHAR_BLOCK1 = 176
CHAR_BLOCK2 = 177
CHAR_BLOCK3 = 178
# arrows
CHAR_ARROW_N = 24
CHAR_ARROW_S = 25
CHAR_ARROW_E = 26
CHAR_ARROW_W = 27
# arrows without tail
CHAR_ARROW2_N = 30
CHAR_ARROW2_S = 31
CHAR_ARROW2_E = 16
CHAR_ARROW2_W = 17
# double arrows
CHAR_DARROW_H = 29
CHAR_DARROW_V = 18
# GUI stuff
CHAR_CHECKBOX_UNSET = 224
CHAR_CHECKBOX_SET = 225
CHAR_RADIO_UNSET = 9
CHAR_RADIO_SET = 10
# sub-pixel resolution kit
CHAR_SUBP_NW = 226
CHAR_SUBP_NE = 227
CHAR_SUBP_N = 228
CHAR_SUBP_SE = 229
CHAR_SUBP_DIAG = 230
CHAR_SUBP_E = 231
CHAR_SUBP_SW = 232
# font flags
FONT_LAYOUT_ASCII_INCOL = 1
FONT_LAYOUT_ASCII_INROW = 2
FONT_TYPE_GREYSCALE = 4
FONT_TYPE_GRAYSCALE = 4
FONT_LAYOUT_TCOD = 8
# color control codes
COLCTRL_1=1
COLCTRL_2=2
COLCTRL_3=3
COLCTRL_4=4
COLCTRL_5=5
COLCTRL_NUMBER=5
COLCTRL_FORE_RGB=6
COLCTRL_BACK_RGB=7
COLCTRL_STOP=8

# initializing the console
def console_init_root(w, h, title, fullscreen=False):
    _lib.TCOD_console_init_root(w, h, title, c_uint(fullscreen))

def console_get_width(con):
	return _lib.TCOD_console_get_width(con)

def console_get_height(con):
	return _lib.TCOD_console_get_height(con)

def console_set_custom_font(fontFile, flags=FONT_LAYOUT_ASCII_INCOL, nb_char_horiz=0, nb_char_vertic=0):
    _lib.TCOD_console_set_custom_font(fontFile, flags, nb_char_horiz, nb_char_vertic)

def console_map_ascii_code_to_font(asciiCode, fontCharX, fontCharY):
    if type(asciiCode) == str:
        _lib.TCOD_console_map_ascii_code_to_font(ord(asciiCode), fontCharX,
                                                 fontCharY)
    else:
        _lib.TCOD_console_map_ascii_code_to_font(asciiCode, fontCharX,
                                                 fontCharY)

def console_map_ascii_codes_to_font(firstAsciiCode, nbCodes, fontCharX,
                                    fontCharY):
    if type(firstAsciiCode) == str:
        _lib.TCOD_console_map_ascii_codes_to_font(ord(firstAsciiCode), nbCodes,
                                                  fontCharX, fontCharY)
    else:
        _lib.TCOD_console_map_ascii_codes_to_font(firstAsciiCode, nbCodes,
                                                  fontCharX, fontCharY)

def console_map_string_to_font(s, fontCharX, fontCharY):
    _lib.TCOD_console_map_string_to_font(s, fontCharX, fontCharY)

def console_is_fullscreen():
    return _lib.TCOD_console_is_fullscreen() == 1

def console_set_fullscreen(fullscreen):
    _lib.TCOD_console_set_fullscreen(c_int(fullscreen))

def console_is_window_closed():
    return _lib.TCOD_console_is_window_closed() == 1

def console_set_window_title(title):
    _lib.TCOD_console_set_window_title(title)

def console_credits():
    _lib.TCOD_console_credits()

def console_credits_reset():
    _lib.TCOD_console_credits_reset()

def console_credits_render(x, y, alpha):
    return _lib.TCOD_console_credits_render(x, y, c_int(alpha)) == 1

def console_flush():
    _lib.TCOD_console_flush()

# drawing on a console
def console_set_background_color(con, col):
    _lib.TCOD_console_set_background_color(con, col)

def console_set_foreground_color(con, col):
    _lib.TCOD_console_set_foreground_color(con, col)

def console_clear(con):
    return _lib.TCOD_console_clear(con)

def console_put_char(con, x, y, c, flag=BKGND_SET):
    if type(c) == str:
        _lib.TCOD_console_put_char(con, x, y, ord(c), flag)
    else:
        _lib.TCOD_console_put_char(con, x, y, c, flag)

def console_put_char_ex(con, x, y, c, fore, back):
    if type(c) == str:
        _lib.TCOD_console_put_char_ex(con, x, y, ord(c), fore, back)
    else:
        _lib.TCOD_console_put_char_ex(con, x, y, c, fore, back)

def console_set_back(con, x, y, col, flag=BKGND_SET):
    _lib.TCOD_console_set_back(con, x, y, col, flag)

def console_set_fore(con, x, y, col):
    _lib.TCOD_console_set_fore(con, x, y, col)

def console_set_char(con, x, y, c):
    if type(c) == str:
        _lib.TCOD_console_set_char(con, x, y, ord(c))
    else:
        _lib.TCOD_console_set_char(con, x, y, c)

def console_print_left(con, x, y, bk, s):
    _lib.TCOD_console_print_left(con, x, y, bk, s)

def console_print_right(con, x, y, bk, s):
    _lib.TCOD_console_print_right(con, x, y, bk, s)

def console_print_center(con, x, y, bk, s):
    _lib.TCOD_console_print_center(con, x, y, bk, s)

def console_print_left_rect(con, x, y, w, h, bk, s):
    return _lib.TCOD_console_print_left_rect(con, x, y, w, h, bk, s)

def console_print_right_rect(con, x, y, w, h, bk, s):
    return _lib.TCOD_console_print_right_rect(con, x, y, w, h, bk, s)

def console_print_center_rect(con, x, y, w, h, bk, s):
    return _lib.TCOD_console_print_center_rect(con, x, y, w, h, bk, s)

def console_height_left_rect(con, x, y, w, h, s):
    return _lib.TCOD_console_height_left_rect(con, x, y, w, h, s)

def console_height_right_rect(con, x, y, w, h, s):
    return _lib.TCOD_console_height_right_rect(con, x, y, w, h, s)

def console_height_center_rect(con, x, y, w, h, s):
    return _lib.TCOD_console_height_center_rect(con, x, y, w, h, s)

def console_rect(con, x, y, w, h, clr, flag=BKGND_SET):
    _lib.TCOD_console_rect(con, x, y, w, h, c_int(clr), flag)

def console_hline(con, x, y, l):
    _lib.TCOD_console_hline( con, x, y, l)

def console_vline(con, x, y, l):
    _lib.TCOD_console_vline( con, x, y, l)

def console_print_frame(con, x, y, w, h, clr, bkflg, s):
    _lib.TCOD_console_print_frame(con, x, y, w, h, c_int(clr), bkflg, s)

def console_set_color_control(con,fore,back) :
    _lib.TCOD_console_set_color_control(con,fore,back)

def console_get_background_color(con):
    iret=_lib.TCOD_console_get_background_color_wrapper(con)
    return int_to_col(iret)

def console_get_foreground_color(con):
    iret=_lib.TCOD_console_get_foreground_color_wrapper(con)
    return int_to_col(iret)

def console_get_back(con, x, y):
    iret=_lib.TCOD_console_get_back_wrapper(con, x, y)
    return int_to_col(iret)

def console_get_fore(con, x, y):
    iret=_lib.TCOD_console_get_fore_wrapper(con, x, y)
    return int_to_col(iret)

def console_get_char(con, x, y):
    return _lib.TCOD_console_get_char(con, x, y)

def console_set_fade(fade, fadingColor):
    _lib.TCOD_console_set_fade_wrapper(fade, col_to_int(fadingColor))

def console_get_fade():
    return _lib.TCOD_console_get_fade().value

def console_get_fading_color():
    return int_to_col(_lib.TCOD_console_get_fading_color_wrapper())

# handling keyboard input
def console_wait_for_keypress(flush):
    k=Key()
    _lib.TCOD_console_wait_for_keypress_wrapper(byref(k),c_int(flush))
    return k

def console_check_for_keypress(flags=KEY_RELEASED):
	k=Key()
	_lib.TCOD_console_check_for_keypress_wrapper(byref(k),c_int(flags))
	return k

def console_is_key_pressed(key):
    return _lib.TCOD_console_is_key_pressed(key) == 1

def console_set_keyboard_repeat(initial_delay, interval):
    _lib.TCOD_console_set_keyboard_repeat(initial_delay, interval)

def console_disable_keyboard_repeat():
    _lib.TCOD_console_disable_keyboard_repeat()

# using offscreen consoles
def console_new(w, h):
    return _lib.TCOD_console_new(w, h)
	
def console_get_width(con):
	return _lib.TCOD_console_get_width(con)
	
def console_get_height(con):
	return _lib.TCOD_console_get_height(con)

def console_blit(src, x, y, w, h, dst, xdst, ydst, ffade=1.0,bfade=1.0):
    _lib.TCOD_console_blit(src, x, y, w, h, dst, xdst, ydst, c_float(ffade), c_float(bfade))

def console_set_key_color(con, col):
    _lib.TCOD_console_set_key_color(con, col)

def console_delete(con):
    _lib.TCOD_console_delete(con)

# fast color filling
def console_fill_foreground(con,r,g,b) :
	if (numpy_available and isinstance(r, numpy.ndarray) and
		isinstance(g, numpy.ndarray) and isinstance(b, numpy.ndarray)):
		#numpy arrays, use numpy's ctypes functions
		r = numpy.ascontiguousarray(r, dtype=numpy.int_)
		g = numpy.ascontiguousarray(g, dtype=numpy.int_)
		b = numpy.ascontiguousarray(b, dtype=numpy.int_)
		cr = r.ctypes.data_as(ctypes.POINTER(ctypes.c_int))
		cg = g.ctypes.data_as(ctypes.POINTER(ctypes.c_int))
		cb = b.ctypes.data_as(ctypes.POINTER(ctypes.c_int))
		
	elif (isinstance(r, list) and isinstance(g, list) and isinstance(b, list)):
		#simple python lists, convert using ctypes
		cr = (c_int * len(r))(*r)
		cg = (c_int * len(g))(*g)
		cb = (c_int * len(b))(*b)
	else:
		raise TypeError('R, G and B must all be of the same type (list or NumPy array)')
	
	if len(r) != len(g) or len(r) != len(b):
		raise TypeError('R, G and B must all have the same size.')
	
	_lib.TCOD_console_fill_foreground(con, cr, cg, cb)

def console_fill_background(con,r,g,b) :
	if (numpy_available and isinstance(r, numpy.ndarray) and
		isinstance(g, numpy.ndarray) and isinstance(b, numpy.ndarray)):
		#numpy arrays, use numpy's ctypes functions
		
		r = numpy.ascontiguousarray(r, dtype=numpy.int_)
		g = numpy.ascontiguousarray(g, dtype=numpy.int_)
		b = numpy.ascontiguousarray(b, dtype=numpy.int_)
		cr = r.ctypes.data_as(ctypes.POINTER(ctypes.c_int))
		cg = g.ctypes.data_as(ctypes.POINTER(ctypes.c_int))
		cb = b.ctypes.data_as(ctypes.POINTER(ctypes.c_int))
		
	elif (isinstance(r, list) and isinstance(g, list) and isinstance(b, list)):
		#simple python lists, convert using ctypes
		cr = (c_int * len(r))(*r)
		cg = (c_int * len(g))(*g)
		cb = (c_int * len(b))(*b)
	else:
		raise TypeError('R, G and B must all be of the same type (list or NumPy array)')
	
	if len(r) != len(g) or len(r) != len(b):
		raise TypeError('R, G and B must all have the same size.')
	
	_lib.TCOD_console_fill_background(con, cr, cg, cb)

############################
# sys module
############################
_lib.TCOD_sys_get_last_frame_length.restype = c_float
_lib.TCOD_sys_elapsed_seconds.restype = c_float

# high precision time functions
def sys_set_fps(fps):
    _lib.TCOD_sys_set_fps(fps)

def sys_get_fps():
    return _lib.TCOD_sys_get_fps()

def sys_get_last_frame_length():
    return _lib.TCOD_sys_get_last_frame_length()

def sys_sleep_milli(val):
    _lib.TCOD_sys_sleep_milli(c_uint(val))

def sys_update_char(ascii,img,x,y):
    _lib.TCOD_sys_update_char(c_uint(ascii),img,c_int(x),c_int(y))

def sys_elapsed_milli():
    return _lib.TCOD_sys_elapsed_milli()

def sys_elapsed_seconds():
    return _lib.TCOD_sys_elapsed_seconds()

# easy screenshots
def sys_save_screenshot(name=0):
    _lib.TCOD_sys_save_screenshot(name)

# custom fullscreen resolution
def sys_force_fullscreen_resolution(width, height):
    _lib.TCOD_sys_force_fullscreen_resolution(width, height)

def sys_get_current_resolution():
    w = c_int()
    h = c_int()
    _lib.TCOD_sys_get_current_resolution(byref(w), byref(h))
    return w.value, h.value

def sys_get_char_size():
    w = c_int()
    h = c_int()
    _lib.TCOD_sys_get_char_size(byref(w), byref(h))
    return w.value, h.value

# update font bitmap
def sys_update_char(asciiCode, fontx, fonty, img, x, y) :
    _lib.TCOD_sys_update_char(c_int(asciiCode),c_int(fontx),c_int(fonty),img,c_int(x),c_int(y))

# custom SDL post renderer
SDL_RENDERER_FUNC = None
sdl_renderer_func = None
def sys_register_SDL_renderer(func):
    global SDL_RENDERER_FUNC
    global sdl_renderer_func
    SDL_RENDERER_FUNC = CFUNCTYPE(None, c_void_p)
    sdl_renderer_func = SDL_RENDERER_FUNC(func)
    _lib.TCOD_sys_register_SDL_renderer(sdl_renderer_func)

############################
# line module
############################
_lib.TCOD_line_step.restype = c_uint
_lib.TCOD_line.restype=c_uint

def line_init(xo, yo, xd, yd):
    _lib.TCOD_line_init(xo, yo, xd, yd)

def line_step():
    x = c_int()
    y = c_int()
    ret = _lib.TCOD_line_step(byref(x), byref(y))
    if not ret:
        return x.value, y.value
    return None,None

def line(xo,yo,xd,yd,py_callback) :
	LINE_CBK_FUNC=CFUNCTYPE(c_uint,c_int,c_int)
	def int_callback(x,y) :
		if py_callback(x,y) :
			return 1
		return 0
	c_callback=LINE_CBK_FUNC(int_callback)
	return _lib.TCOD_line(xo,yo,xd,yd,c_callback) == 1

############################
# image module
############################
_lib.TCOD_image_is_pixel_transparent.restype = c_uint

def image_new(width, height):
    return _lib.TCOD_image_new(width, height)

def image_clear(image,col) :
    _lib.TCOD_image_clear(image,col)

def image_invert(image) :
    _lib.TCOD_image_invert(image)

def image_hflip(image) :
    _lib.TCOD_image_hflip(image)

def image_vflip(image) :
    _lib.TCOD_image_vflip(image)

def image_scale(image, neww, newh) :
    _lib.TCOD_image_scale(image,c_int(neww),c_int(newh))

def image_set_key_color(image,col) :
    _lib.TCOD_image_set_key_color(image,col)

def image_get_alpha(image,x,y) :
    return _lib.TCOD_image_get_alpha(image,c_int(x),c_int(y))

def image_is_pixel_transparent(image,x,y) :
    return _lib.TCOD_image_is_pixel_transparent(image,c_int(x),c_int(y)) == 1

def image_load(filename):
    return _lib.TCOD_image_load(filename)

def image_from_console(console):
    return _lib.TCOD_image_from_console(console)

def image_refresh_console(image, console):
    _lib.TCOD_image_refresh_console(image, console)

def image_get_size(image):
    w=c_int()
    h=c_int()
    _lib.TCOD_image_get_size(image, byref(w), byref(h))
    return w.value, h.value

def image_get_pixel(image, x, y):
    return int_to_col(_lib.TCOD_image_get_pixel_wrapper(image, x, y))

def image_get_mipmap_pixel(image, x0, y0, x1, y1):
    return int_to_col(_lib.TCOD_image_get_mipmap_pixel_wrapper(image, c_float(x0), c_float(y0),
                                            c_float(x1), c_float(y1)))
def image_put_pixel(image, x, y, col):
    _lib.TCOD_image_put_pixel_wrapper(image, x, y, col_to_int(col))

def image_blit(image, console, x, y, bkgnd_flag, scalex, scaley, angle):
    _lib.TCOD_image_blit(image, console, c_float(x), c_float(y), bkgnd_flag,
                         c_float(scalex), c_float(scaley), c_float(angle))

def image_blit_rect(image, console, x, y, w, h, bkgnd_flag):
    _lib.TCOD_image_blit_rect(image, console, x, y, w, h, bkgnd_flag)

def image_blit_2x(image, console, dx, dy, sx=0, sy=0, w=-1, h=-1):
    _lib.TCOD_image_blit_2x(image, console, dx,dy,sx,sy,w,h)

def image_save(image, filename):
    _lib.TCOD_image_save(image, filename)

def image_delete(image):
    _lib.TCOD_image_delete(image)

############################
# mouse module
############################
class Mouse(Structure):
    _fields_=[('x', c_int, 32),
              ('y', c_int, 32),
              ('dx', c_int, 32),
              ('dy', c_int, 32),
              ('cx', c_int, 32),
              ('cy', c_int, 32),
              ('dcx', c_int, 32),
              ('dcy', c_int, 32),
              ('lbutton', c_uint, 8),
              ('rbutton', c_uint, 8),
              ('mbutton', c_uint, 8),
              ('lbutton_pressed', c_uint, 8),
              ('rbutton_pressed', c_uint, 8),
              ('mbutton_pressed', c_uint, 8),
              ('wheel_up', c_uint, 8),
              ('wheel_down', c_uint, 8),
              ]

_lib.TCOD_mouse_is_cursor_visible.restype = c_uint

def mouse_show_cursor(visible):
    _lib.TCOD_mouse_show_cursor(c_int(visible))

def mouse_is_cursor_visible():
    return _lib.TCOD_mouse_is_cursor_visible() == 1

def mouse_move(x, y):
    _lib.TCOD_mouse_move(x, y)

def mouse_get_status():
    m = Mouse()
    _lib.TCOD_mouse_get_status_wrapper(byref(m))
    return m

############################
# parser module
############################
_lib.TCOD_struct_get_name.restype = c_char_p
_lib.TCOD_struct_is_mandatory.restype = c_uint
_lib.TCOD_parser_get_bool_property.restype = c_uint
_lib.TCOD_parser_get_float_property.restype = c_float
_lib.TCOD_parser_get_string_property.restype = c_char_p

class Dice(Structure):
    _fields_=[('nb_dices', c_int),
              ('nb_faces', c_int),
              ('multiplier', c_float),
              ('addsub', c_float),
              ]

class _CValue(Union):
    _fields_=[('c',c_uint),
              ('i',c_int),
              ('f',c_float),
              ('s',c_char_p),
              ('col',Color),
              ('dice',Dice),
              ('custom',c_void_p),
              ]

_CFUNC_NEW_STRUCT = CFUNCTYPE(c_uint, c_void_p, c_char_p)
_CFUNC_NEW_FLAG = CFUNCTYPE(c_uint, c_char_p)
_CFUNC_NEW_PROPERTY = CFUNCTYPE(c_uint, c_char_p, c_int, _CValue)

class _CParserListener(Structure):
    _fields_=[('new_struct', _CFUNC_NEW_STRUCT),
              ('new_flag',_CFUNC_NEW_FLAG),
              ('new_property',_CFUNC_NEW_PROPERTY),
              ('end_struct',_CFUNC_NEW_STRUCT),
              ('error',_CFUNC_NEW_FLAG),
              ]

# property types
TYPE_NONE = 0
TYPE_BOOL = 1
TYPE_CHAR = 2
TYPE_INT = 3
TYPE_FLOAT = 4
TYPE_STRING = 5
TYPE_COLOR = 6
TYPE_DICE = 7
TYPE_VALUELIST00 = 8
TYPE_VALUELIST01 = 9
TYPE_VALUELIST02 = 10
TYPE_VALUELIST03 = 11
TYPE_VALUELIST04 = 12
TYPE_VALUELIST05 = 13
TYPE_VALUELIST06 = 14
TYPE_VALUELIST07 = 15
TYPE_VALUELIST08 = 16
TYPE_VALUELIST09 = 17
TYPE_VALUELIST10 = 18
TYPE_VALUELIST11 = 19
TYPE_VALUELIST12 = 20
TYPE_VALUELIST13 = 21
TYPE_VALUELIST14 = 22
TYPE_VALUELIST15 = 23
TYPE_LIST = 1024

def parser_new():
    return _lib.TCOD_parser_new()

def parser_new_struct(parser, name):
    return _lib.TCOD_parser_new_struct(parser, name)

def struct_add_flag(struct, name):
    _lib.TCOD_struct_add_flag(struct, name)

def struct_add_property(struct, name, typ, mandatory):
    _lib.TCOD_struct_add_property(struct, name, typ, c_int(mandatory))

def struct_add_value_list(struct, name, value_list, mandatory):
    CARRAY = c_char_p * (len(value_list) + 1)
    cvalue_list = CARRAY()
    for i in range(len(value_list)):
        cvalue_list[i] = cast(value_list[i], c_char_p)
    cvalue_list[len(value_list)] = 0
    _lib.TCOD_struct_add_value_list(struct, name, cvalue_list, c_int(mandatory))

def struct_add_list_property(struct, name, typ, mandatory):
    _lib.TCOD_struct_add_list_property(struct, name, typ, c_int(mandatory))

def struct_add_structure(struct, sub_struct):
    _lib.TCOD_struct_add_structure(struct, sub_struct)

def struct_get_name(struct):
    return _lib.TCOD_struct_get_name(struct)

def struct_is_mandatory(struct, name):
    return _lib.TCOD_struct_is_mandatory(struct, name) == 1

def struct_get_type(struct, name):
    return _lib.TCOD_struct_get_type(struct, name)

def parser_run(parser, filename, listener=0):
    if listener != 0:
        clistener=_CParserListener()
        def value_converter(name, typ, value):
            if typ == TYPE_BOOL:
                return listener.new_property(name, typ, value.c == 1)
            elif typ == TYPE_CHAR:
                return listener.new_property(name, typ, '%c' % (value.c & 0xFF))
            elif typ == TYPE_INT:
                return listener.new_property(name, typ, value.i)
            elif typ == TYPE_FLOAT:
                return listener.new_property(name, typ, value.f)
            elif typ == TYPE_STRING or \
                 TYPE_VALUELIST15 >= typ >= TYPE_VALUELIST00:
                 return listener.new_property(name, typ, value.s)
            elif typ == TYPE_COLOR:
                return listener.new_property(name, typ, value.col)
            elif typ == TYPE_DICE:
                return listener.new_property(name, typ, value.dice)
            return True
        clistener.new_struct = _CFUNC_NEW_STRUCT(listener.new_struct)
        clistener.new_flag = _CFUNC_NEW_FLAG(listener.new_flag)
        clistener.new_property = _CFUNC_NEW_PROPERTY(value_converter)
        clistener.end_struct = _CFUNC_NEW_STRUCT(listener.end_struct)
        clistener.error = _CFUNC_NEW_FLAG(listener.error)
        _lib.TCOD_parser_run(parser, filename, byref(clistener))
    else:
        _lib.TCOD_parser_run(parser, filename, 0)

def parser_delete(parser):
    _lib.TCOD_parser_delete(parser)

def parser_get_bool_property(parser, name):
    return _lib.TCOD_parser_get_bool_property(parser, name) == 1

def parser_get_int_property(parser, name):
    return _lib.TCOD_parser_get_int_property(parser, name)

def parser_get_char_property(parser, name):
    return '%c' % _lib.TCOD_parser_get_char_property(parser, name)

def parser_get_float_property(parser, name):
    return _lib.TCOD_parser_get_float_property(parser, name)

def parser_get_string_property(parser, name):
    return _lib.TCOD_parser_get_string_property(parser, name)

def parser_get_color_property(parser, name):
    iret=_lib.TCOD_parser_get_color_property_wrapper(parser, name)
    return int_to_col(iret)

def parser_get_dice_property(parser, name):
    d = Dice()
    _lib.TCOD_parser_get_dice_property_py(parser, name, byref(d))
    return d

def parser_get_list_property(parser, name):
    clist = _lib.TCOD_parser_get_list_property(parser, name)
    res = list()
    for i in range(_lib.TCOD_list_size(clist)):
        elt = _lib.TCOD_list_get(clist, i)
        res.append(elt)
    return res

############################
# random module
############################
_lib.TCOD_random_get_float.restype = c_float
_lib.TCOD_random_get_gaussian_float.restype = c_float
RNG_MT = 0
RNG_CMWC = 1

def random_get_instance():
    return _lib.TCOD_random_get_instance()

def random_new(algo=RNG_CMWC):
    return _lib.TCOD_random_new(algo)

def random_new_from_seed(seed, algo=RNG_CMWC):
    return _lib.TCOD_random_new_from_seed(algo,c_uint(seed))

def random_get_int(rnd, mi, ma):
    return _lib.TCOD_random_get_int(rnd, mi, ma)

def random_get_float(rnd, mi, ma):
    return _lib.TCOD_random_get_float(rnd, c_float(mi), c_float(ma))

def random_get_gaussian_float(rnd, mi, ma):
    return _lib.TCOD_random_get_gaussian_float(rnd, c_float(mi), c_float(ma))
    
def random_get_gaussian_int(rnd, mi, ma):
    return _lib.TCOD_random_get_gaussian_int(rnd, mi, ma)

def random_save(rnd):
    return _lib.TCOD_random_save(rnd)

def random_restore(rnd, backup):
    _lib.TCOD_random_restore(rnd, backup)

def random_delete(rnd):
    _lib.TCOD_random_delete(rnd)

############################
# noise module
############################
_lib.TCOD_noise_perlin.restype = c_float
_lib.TCOD_noise_simplex.restype = c_float
_lib.TCOD_noise_wavelet.restype = c_float
_lib.TCOD_noise_fbm_perlin.restype = c_float
_lib.TCOD_noise_fbm_simplex.restype = c_float
_lib.TCOD_noise_fbm_wavelet.restype = c_float
_lib.TCOD_noise_turbulence_perlin.restype = c_float
_lib.TCOD_noise_turbulence_simplex.restype = c_float
_lib.TCOD_noise_turbulence_wavelet.restype = c_float

NOISE_DEFAULT_HURST = 0.5
NOISE_DEFAULT_LACUNARITY = 2.0

def noise_new(dim, h=NOISE_DEFAULT_HURST, l=NOISE_DEFAULT_LACUNARITY, rnd=0):
    return _lib.TCOD_noise_new(dim, c_float(h), c_float(l), rnd)

def _noise_int(n, f, func):
    ct = c_float * len(f)
    cf = ct()
    i = 0
    for value in f:
        cf[i] = c_float(value)
        i += 1
    return func(n, cf)

def noise_perlin(n, f):
    return _noise_int(n, f, _lib.TCOD_noise_perlin)

def noise_simplex(n, f):
    return _noise_int(n, f, _lib.TCOD_noise_simplex)

def noise_wavelet(n, f):
    return _noise_int(n, f, _lib.TCOD_noise_wavelet)

def _noise_int2(n, f, oc, func):
    ct = c_float * len(f)
    cf = ct()
    i = 0
    for value in f:
        cf[i] = c_float(value)
        i += 1
    return func(n, cf, c_float(oc))

def noise_fbm_perlin(n, f, oc):
    return _noise_int2(n, f, oc, _lib.TCOD_noise_fbm_perlin)

def noise_fbm_simplex(n, f, oc):
    return _noise_int2(n, f, oc, _lib.TCOD_noise_fbm_simplex)

def noise_fbm_wavelet(n, f, oc):
    return _noise_int2(n, f, oc, _lib.TCOD_noise_fbm_wavelet)

def noise_turbulence_perlin(n, f, oc):
    return _noise_int2(n, f, oc, _lib.TCOD_noise_turbulence_perlin)

def noise_turbulence_simplex(n, f, oc):
    return _noise_int2(n, f, oc, _lib.TCOD_noise_turbulence_simplex)

def noise_turbulence_wavelet(n, f, oc):
    return _noise_int2(n, f, oc, _lib.TCOD_noise_turbulence_wavelet)

def noise_delete(n):
    _lib.TCOD_noise_delete(n)

############################
# fov module
############################
FOV_BASIC = 0
FOV_DIAMOND = 1
FOV_SHADOW = 2
FOV_PERMISSIVE_0 = 3
FOV_PERMISSIVE_1 = 4
FOV_PERMISSIVE_2 = 5
FOV_PERMISSIVE_3 = 6
FOV_PERMISSIVE_4 = 7
FOV_PERMISSIVE_5 = 8
FOV_PERMISSIVE_6 = 9
FOV_PERMISSIVE_7 = 10
FOV_PERMISSIVE_8 = 11
FOV_RESTRICTIVE = 12
NB_FOV_ALGORITHMS = 13

def FOV_PERMISSIVE(p) :
    return FOV_PERMISSIVE_0+p

def map_new(w, h):
    return _lib.TCOD_map_new(w, h)

def map_copy(source, dest):
    return _lib.TCOD_map_copy(source, dest)

def map_set_properties(m, x, y, isTrans, isWalk):
    _lib.TCOD_map_set_properties(m, x, y, c_int(isTrans), c_int(isWalk))

def map_clear(m):
    _lib.TCOD_map_clear(m)

def map_compute_fov(m, x, y, radius=0, light_walls=True, algo=FOV_RESTRICTIVE ):
    _lib.TCOD_map_compute_fov(m, x, y, c_int(radius), c_uint(light_walls), c_int(algo))

def map_is_in_fov(m, x, y):
    return _lib.TCOD_map_is_in_fov(m, x, y) == 1

def map_is_transparent(m, x, y):
    return _lib.TCOD_map_is_transparent(m, x, y) == 1

def map_is_walkable(m, x, y):
    return _lib.TCOD_map_is_walkable(m, x, y) == 1

def map_delete(m):
    return _lib.TCOD_map_delete(m)

############################
# pathfinding module
############################
_lib.TCOD_path_compute.restype = c_uint
_lib.TCOD_path_is_empty.restype = c_uint
_lib.TCOD_path_walk.restype = c_uint

def path_new_using_map(m, dcost=1.41):
    return _lib.TCOD_path_new_using_map(c_void_p(m), c_float(dcost))

PATH_CBK_FUNC = None
cbk_func = None
def path_new_using_function(w, h, func, userdata=0, dcost=1.41):
    global PATH_CBK_FUNC
    global cbk_func
    PATH_CBK_FUNC = CFUNCTYPE(c_float, c_int, c_int, c_int, c_int, py_object)
    cbk_func = PATH_CBK_FUNC(func)
    return _lib.TCOD_path_new_using_function(w, h, cbk_func,
                                             py_object(userdata), c_float(dcost))

def path_compute(p, ox, oy, dx, dy):
    return _lib.TCOD_path_compute(p, ox, oy, dx, dy) == 1

def path_get_origin(p):
    x = c_int()
    y = c_int()
    _lib.TCOD_path_get_origin(p, byref(x), byref(y))
    return x.value, y.value

def path_get_destination(p):
    x = c_int()
    y = c_int()
    _lib.TCOD_path_get_destination(p, byref(x), byref(y))
    return x.value, y.value

def path_size(p):
    return _lib.TCOD_path_size(p)

def path_get(p, idx):
    x = c_int()
    y = c_int()
    _lib.TCOD_path_get(p, idx, byref(x), byref(y))
    return x.value, y.value

def path_is_empty(p):
    return _lib.TCOD_path_is_empty(p) == 1

def path_walk(p, recompute):
    x = c_int()
    y = c_int()
    if _lib.TCOD_path_walk(p, byref(x), byref(y), c_int(recompute)):
        return x.value, y.value
    return None,None

def path_delete(p):
    _lib.TCOD_path_delete(p)

_lib.TCOD_dijkstra_path_set.restype = c_uint
_lib.TCOD_dijkstra_is_empty.restype = c_uint
_lib.TCOD_dijkstra_size.restype = c_int
_lib.TCOD_dijkstra_path_walk.restype = c_uint
_lib.TCOD_dijkstra_get_distance.restype = c_float

def dijkstra_new(m, dcost=1.41):
    return _lib.TCOD_dijkstra_new(c_void_p(m), c_float(dcost))

PATH_CBK_FUNC = None
cbk_func = None
def dijkstra_new_using_function(w, h, func, userdata=0, dcost=1.41):
    global PATH_CBK_FUNC
    global cbk_func
    PATH_CBK_FUNC = CFUNCTYPE(c_float, c_int, c_int, c_int, c_int, py_object)
    cbk_func = PATH_CBK_FUNC(func)
    return _lib.TCOD_path_dijkstra_using_function(w, h, cbk_func,
                                             py_object(userdata), c_float(dcost))

def dijkstra_compute(p, ox, oy):
    _lib.TCOD_dijkstra_compute(p, c_int(ox), c_int(oy))

def dijkstra_path_set(p, x, y):
    return _lib.TCOD_dijkstra_path_set(p, c_int(x), c_int(y))

def dijkstra_get_distance(p, x, y):
    return _lib.TCOD_dijkstra_get_distance(p, c_int(x), c_int(y))

def dijkstra_size(p):
    return _lib.TCOD_dijkstra_size(p)

def dijkstra_get(p, idx):
    x = c_int()
    y = c_int()
    _lib.TCOD_dijkstra_get(p, c_int(idx), byref(x), byref(y))
    return x.value, y.value

def dijkstra_is_empty(p):
    return _lib.TCOD_dijkstra_is_empty(p) == 1

def dijkstra_path_walk(p):
    x = c_int()
    y = c_int()
    if _lib.TCOD_dijkstra_path_walk(p, byref(x), byref(y)):
        return x.value, y.value
    return None,None

def dijkstra_delete(p):
    _lib.TCOD_dijkstra_delete(p)

############################
# bsp module
############################
class _CBsp(Structure):
    _fields_ = [('next', c_int, 32),
                ('father', c_int, 32),
                ('son', c_int, 32),
                ('x', c_int, 32),
                ('y', c_int, 32),
                ('w', c_int, 32),
                ('h', c_int, 32),
                ('position', c_int, 32),
                ('level', c_uint, 32),
                ('horizontal', c_uint, 32),
                ]

_lib.TCOD_bsp_new_with_size.restype = POINTER(_CBsp)
_lib.TCOD_bsp_left.restype = POINTER(_CBsp)
_lib.TCOD_bsp_right.restype = POINTER(_CBsp)
_lib.TCOD_bsp_father.restype = POINTER(_CBsp)
_lib.TCOD_bsp_is_leaf.restype = c_uint
_lib.TCOD_bsp_contains.restype = c_uint
_lib.TCOD_bsp_find_node.restype = POINTER(_CBsp)

# python class encapsulating the _CBsp pointer
class Bsp(object):
    def __init__(self, cnode):
        pcbsp = cast(cnode, POINTER(_CBsp))
        self.p = pcbsp

    def getx(self):
        return self.p.contents.x
    def setx(self, value):
        self.p.contents.x = value
    x = property(getx, setx)

    def gety(self):
        return self.p.contents.y
    def sety(self, value):
        self.p.contents.y = value
    y = property(gety, sety)

    def getw(self):
        return self.p.contents.w
    def setw(self, value):
        self.p.contents.w = value
    w = property(getw, setw)

    def geth(self):
        return self.p.contents.h
    def seth(self, value):
        self.p.contents.h = value
    h = property(geth, seth)

    def getpos(self):
        return self.p.contents.position
    def setpos(self, value):
        self.p.contents.position = value
    position = property(getpos, setpos)

    def gethor(self):
        return self.p.contents.horizontal == 1
    def sethor(self,value):
        self.p.contents.horizontal = c_int(value)
    horizontal = property(gethor, sethor)

    def getlev(self):
        return self.p.contents.level
    def setlev(self,value):
        self.p.contents.level = value
    level = property(getlev, setlev)


def bsp_new_with_size(x, y, w, h):
    return Bsp(_lib.TCOD_bsp_new_with_size(x, y, w, h))

def bsp_split_once(node, horizontal, position):
    _lib.TCOD_bsp_split_once(node.p, c_int(horizontal), position)

def bsp_split_recursive(node, randomizer, nb, minHSize, minVSize, maxHRatio,
                        maxVRatio):
    _lib.TCOD_bsp_split_recursive(node.p, randomizer, nb, minHSize, minVSize,
                                  c_float(maxHRatio), c_float(maxVRatio))

def bsp_resize(node, x, y, w, h):
    _lib.TCOD_bsp_resize(node.p, x, y, w, h)

def bsp_left(node):
    return Bsp(_lib.TCOD_bsp_left(node.p))

def bsp_right(node):
    return Bsp(_lib.TCOD_bsp_right(node.p))

def bsp_father(node):
    return Bsp(_lib.TCOD_bsp_father(node.p))

def bsp_is_leaf(node):
    return _lib.TCOD_bsp_is_leaf(node.p) == 1

def bsp_contains(node, cx, cy):
    return _lib.TCOD_bsp_contains(node.p, cx, cy) == 1

def bsp_find_node(node, cx, cy):
    return Bsp(_lib.TCOD_bsp_find_node(node.p, cx, cy))

def _bsp_traverse(node, callback, userData, func):
    BSP_CBK_FUNC = CFUNCTYPE(c_int, c_void_p, c_void_p)
    # convert the c node into a python node
    #before passing it to the actual callback
    def node_converter(cnode, data):
        node = Bsp(cnode)
        return callback(node, data)
    cbk_func = BSP_CBK_FUNC(node_converter)
    func(node.p, cbk_func, userData)

def bsp_traverse_pre_order(node, callback, userData=0):
    _bsp_traverse(node, callback, userData, _lib.TCOD_bsp_traverse_pre_order)

def bsp_traverse_in_order(node, callback, userData=0):
    _bsp_traverse(node, callback, userData, _lib.TCOD_bsp_traverse_in_order)

def bsp_traverse_post_order(node, callback, userData=0):
    _bsp_traverse(node, callback, userData, _lib.TCOD_bsp_traverse_post_order)

def bsp_traverse_level_order(node, callback, userData=0):
    _bsp_traverse(node, callback, userData, _lib.TCOD_bsp_traverse_level_order)

def bsp_traverse_inverted_level_order(node, callback, userData=0):
    _bsp_traverse(node, callback, userData,
                  _lib.TCOD_bsp_traverse_inverted_level_order)

def bsp_remove_sons(node):
    _lib.TCOD_bsp_remove_sons(node.p)

def bsp_delete(node):
    _lib.TCOD_bsp_delete(node.p)

############################
# heightmap module
############################
class _CHeightMap(Structure):
    _fields_=[('w', c_int),
              ('h', c_int),
              ('values', POINTER(c_float)),
              ]

_lib.TCOD_heightmap_new.restype = POINTER(_CHeightMap)
_lib.TCOD_heightmap_get_value.restype = c_float
_lib.TCOD_heightmap_has_land_on_border.restype = c_uint

class HeightMap(object):
    def __init__(self, chm):
        pchm = cast(chm, POINTER(_CHeightMap))
        self.p = pchm

    def getw(self):
        return self.p.contents.w
    def setw(self, value):
        self.p.contents.w = value
    w = property(getw, setw)

    def geth(self):
        return self.p.contents.h
    def seth(self, value):
        self.p.contents.h = value
    h = property(geth, seth)

def heightmap_new(w, h):
    phm = _lib.TCOD_heightmap_new(w, h)
    return HeightMap(phm)

def heightmap_set_value(hm, x, y, value):
    _lib.TCOD_heightmap_set_value(hm.p, x, y, c_float(value))

def heightmap_add(hm, value):
    _lib.TCOD_heightmap_add(hm.p, c_float(value))

def heightmap_scale(hm, value):
    _lib.TCOD_heightmap_scale(hm.p, c_float(value))

def heightmap_clear(hm):
    _lib.TCOD_heightmap_clear(hm.p)

def heightmap_clamp(hm, mi, ma):
    _lib.TCOD_heightmap_clamp(hm.p, c_float(mi),c_float(ma))

def heightmap_copy(hm1, hm2):
    _lib.TCOD_heightmap_copy(hm1.p, hm2.p)

def heightmap_normalize(hm,  mi=0.0, ma=1.0):
    _lib.TCOD_heightmap_normalize(hm.p, c_float(mi), c_float(ma))

def heightmap_lerp_hm(hm1, hm2, hm3, coef):
    _lib.TCOD_heightmap_lerp_hm(hm1.p, hm2.p, hm3.p, c_float(coef))

def heightmap_add_hm(hm1, hm2, hm3):
    _lib.TCOD_heightmap_add_hm(hm1.p, hm2.p, hm3.p)

def heightmap_multiply_hm(hm1, hm2, hm3):
    _lib.TCOD_heightmap_multiply_hm(hm1.p, hm2.p, hm3.p)

def heightmap_add_hill(hm, x, y, radius, height):
    _lib.TCOD_heightmap_add_hill(hm.p, c_float( x), c_float( y),
                                 c_float( radius), c_float( height))

def heightmap_dig_hill(hm, x, y, radius, height):
    _lib.TCOD_heightmap_dig_hill(hm.p, c_float( x), c_float( y),
                                 c_float( radius), c_float( height))

def heightmap_rain_erosion(hm, nbDrops, erosionCoef, sedimentationCoef, rnd=0):
    _lib.TCOD_heightmap_rain_erosion(hm.p, nbDrops, c_float( erosionCoef),
                                     c_float( sedimentationCoef), rnd)

def heightmap_kernel_transform(hm, kernelsize, dx, dy, weight, minLevel,
                               maxLevel):
    FARRAY = c_float * kernelsize
    IARRAY = c_int * kernelsize
    cdx = IARRAY()
    cdy = IARRAY()
    cweight = FARRAY()
    for i in range(kernelsize):
        cdx[i] = c_int(dx[i])
        cdy[i] = c_int(dy[i])
        cweight[i] = c_float(weight[i])
    _lib.TCOD_heightmap_kernel_transform(hm.p, kernelsize, cdx, cdy, cweight,
                                         c_float(minLevel), c_float(maxLevel))

def heightmap_add_voronoi(hm, nbPoints, nbCoef, coef, rnd=0):
    FARRAY = c_float * nbCoef
    ccoef = FARRAY()
    for i in range(nbCoef):
        ccoef[i] = c_float(coef[i])
    _lib.TCOD_heightmap_add_voronoi(hm.p, nbPoints, nbCoef, ccoef, rnd)

def heightmap_add_fbm(hm, noise, mulx, muly, addx, addy, octaves, delta, scale):
    _lib.TCOD_heightmap_add_fbm(hm.p, noise, c_float(mulx), c_float(muly),
                                c_float(addx), c_float(addy),
                                c_float(octaves), c_float(delta),
                                c_float(scale))
def heightmap_scale_fbm(hm, noise, mulx, muly, addx, addy, octaves, delta,
                        scale):
    _lib.TCOD_heightmap_scale_fbm(hm.p, noise, c_float(mulx), c_float(muly),
                                  c_float(addx), c_float(addy),
                                  c_float(octaves), c_float(delta),
                                  c_float(scale))
def heightmap_dig_bezier(hm, px, py, startRadius, startDepth, endRadius,
                         endDepth):
    IARRAY = c_int * 4
    cpx = IARRAY()
    cpy = IARRAY()
    for i in range(4):
        cpx[i] = c_int(px[i])
        cpy[i] = c_int(py[i])
    _lib.TCOD_heightmap_dig_bezier(hm.p, cpx, cpy, c_float(startRadius),
                                   c_float(startDepth), c_float(endRadius),
                                   c_float(endDepth))

def heightmap_get_value(hm, x, y):
    return _lib.TCOD_heightmap_get_value(hm.p, x, y)

def heightmap_get_interpolated_value(hm, x, y):
    return _lib.TCOD_heightmap_get_interplated_value(hm.p, c_float(x),
                                                     c_float(y))

def heightmap_get_slope(hm, x, y):
    return _lib.TCOD_heightmap_get_slope(hm.p, x, y)

def heightmap_get_normal(hm, x, y, waterLevel):
    FARRAY = c_float * 3
    cn = FARRAY()
    _lib.TCOD_heightmap_get_normal(hm.p, c_float(x), c_float(y), cn,
                                   c_float(waterLevel))
    return cn[0], cn[1], cn[2]

def heightmap_count_cells(hm, mi, ma):
    return _lib.TCOD_heightmap_count_cells(hm.p, c_float(mi), c_float(ma))

def heightmap_has_land_on_border(hm, waterlevel):
    return _lib.TCOD_heightmap_has_land_on_border(hm.p,
                                                  c_float(waterlevel)) == 1

def heightmap_get_minmax(hm):
    mi = c_float()
    ma = c_float()
    _lib.TCOD_heightmap_get_minmax(hm.p, byref(mi), byref(ma))
    return mi.value, ma.value

def heightmap_delete(hm):
    _lib.TCOD_heightmap_delete(hm.p)


############################
# name generator module
############################

_lib.TCOD_namegen_get_nb_sets_wrapper.restype = c_int

def namegen_parse(filename,random=0) :
	_lib.TCOD_namegen_parse(filename,random)

def namegen_generate(name, allocate=0) :
	return _lib.TCOD_namegen_generate(name, c_int(allocate))

def namegen_generate_custom(name, rule, allocate=0) :
	return _lib.TCOD_namegen_generate(name, rule, c_int(allocate))

def namegen_get_sets():
	nb=_lib.TCOD_namegen_get_nb_sets_wrapper()
	SARRAY = c_char_p * nb;
	setsa = SARRAY()
	_lib.TCOD_namegen_get_sets_wrapper(setsa)
	ret=list()
	for i in range(nb):
		ret.append(setsa[i])
	return ret

def namegen_destroy() :
	_lib.TCOD_namegen_destroy()


