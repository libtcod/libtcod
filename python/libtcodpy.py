#
# libtcod 1.5.2 python wrapper
# Copyright (c) 2008,2009,2010 Jice & Mingos
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * The name of Jice or Mingos may not be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY JICE AND MINGOS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL JICE OR MINGOS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

import sys
import ctypes
import struct
from ctypes import *

if not hasattr(ctypes, "c_bool"):   # for Python < 2.6
    c_bool = c_uint8

try:  #import NumPy if available
    import numpy
    numpy_available = True
except ImportError:
    numpy_available = False

LINUX=False
MAC=False
MINGW=False
MSVC=False
if sys.platform.find('linux') != -1:
    _lib = ctypes.cdll['./libtcod.so']
    LINUX=True
elif sys.platform.find('darwin') != -1:
    _lib = ctypes.cdll['./libtcod.dylib']
    MAC = True
elif sys.platform.find('haiku') != -1:
    _lib = ctypes.cdll['./libtcod.so']
    HAIKU = True
else:
    try:
        _lib = ctypes.cdll['./libtcod-mingw.dll']
        MINGW=True
    except WindowsError:
        _lib = ctypes.cdll['./libtcod-VS.dll']
        MSVC=True
    # On Windows, ctypes doesn't work well with function returning structs,
    # so we have to user the _wrapper functions instead
    _lib.TCOD_color_multiply = _lib.TCOD_color_multiply_wrapper
    _lib.TCOD_color_add = _lib.TCOD_color_add_wrapper
    _lib.TCOD_color_multiply_scalar = _lib.TCOD_color_multiply_scalar_wrapper
    _lib.TCOD_color_subtract = _lib.TCOD_color_subtract_wrapper
    _lib.TCOD_color_lerp = _lib.TCOD_color_lerp_wrapper
    _lib.TCOD_console_get_default_background = _lib.TCOD_console_get_default_background_wrapper
    _lib.TCOD_console_get_default_foreground = _lib.TCOD_console_get_default_foreground_wrapper
    _lib.TCOD_console_get_char_background = _lib.TCOD_console_get_char_background_wrapper
    _lib.TCOD_console_get_char_foreground = _lib.TCOD_console_get_char_foreground_wrapper
    _lib.TCOD_console_get_fading_color = _lib.TCOD_console_get_fading_color_wrapper
    _lib.TCOD_image_get_pixel = _lib.TCOD_image_get_pixel_wrapper
    _lib.TCOD_image_get_mipmap_pixel = _lib.TCOD_image_get_mipmap_pixel_wrapper
    _lib.TCOD_parser_get_color_property = _lib.TCOD_parser_get_color_property_wrapper

HEXVERSION = 0x010502
STRVERSION = "1.5.2"
TECHVERSION = 0x01050200

############################
# color module
############################
class Color(Structure):
    _fields_ = [('r', c_uint8),
                ('g', c_uint8),
                ('b', c_uint8),
                ]

    def __eq__(self, c):
        return _lib.TCOD_color_equals(self, c)

    def __mul__(self, c):
        if isinstance(c,Color):
            return _lib.TCOD_color_multiply(self, c)
        else:
            return _lib.TCOD_color_multiply_scalar(self, c_float(c))

    def __add__(self, c):
        return _lib.TCOD_color_add(self, c)

    def __sub__(self, c):
        return _lib.TCOD_color_subtract(self, c)

    def __repr__(self):
        return "Color(%d,%d,%d)" % (self.r, self.g, self.b)

    def __getitem__(self, i):
        if type(i) == str:
            return getattr(self, i)
        else:
            return getattr(self, "rgb"[i])

    def __setitem__(self, i, c):
        if type(i) == str:
            setattr(self, i, c)
        else:
            setattr(self, "rgb"[i], c)

    def __iter__(self):
        yield self.r
        yield self.g
        yield self.b

# Should be valid on any platform, check it!  Has to be done after Color is defined.
if MAC:
    from cprotos import setup_protos
    setup_protos(_lib)

_lib.TCOD_color_equals.restype = c_bool
_lib.TCOD_color_multiply.restype = Color
_lib.TCOD_color_multiply_scalar.restype = Color
_lib.TCOD_color_add.restype = Color
_lib.TCOD_color_subtract.restype = Color

# default colors
# grey levels
black=Color(0,0,0)
darkest_grey=Color(31,31,31)
darker_grey=Color(63,63,63)
dark_grey=Color(95,95,95)
grey=Color(127,127,127)
light_grey=Color(159,159,159)
lighter_grey=Color(191,191,191)
lightest_grey=Color(223,223,223)
darkest_gray=Color(31,31,31)
darker_gray=Color(63,63,63)
dark_gray=Color(95,95,95)
gray=Color(127,127,127)
light_gray=Color(159,159,159)
lighter_gray=Color(191,191,191)
lightest_gray=Color(223,223,223)
white=Color(255,255,255)

# sepia
darkest_sepia=Color(31,24,15)
darker_sepia=Color(63,50,31)
dark_sepia=Color(94,75,47)
sepia=Color(127,101,63)
light_sepia=Color(158,134,100)
lighter_sepia=Color(191,171,143)
lightest_sepia=Color(222,211,195)

#standard colors
red=Color(255,0,0)
flame=Color(255,63,0)
orange=Color(255,127,0)
amber=Color(255,191,0)
yellow=Color(255,255,0)
lime=Color(191,255,0)
chartreuse=Color(127,255,0)
green=Color(0,255,0)
sea=Color(0,255,127)
turquoise=Color(0,255,191)
cyan=Color(0,255,255)
sky=Color(0,191,255)
azure=Color(0,127,255)
blue=Color(0,0,255)
han=Color(63,0,255)
violet=Color(127,0,255)
purple=Color(191,0,255)
fuchsia=Color(255,0,255)
magenta=Color(255,0,191)
pink=Color(255,0,127)
crimson=Color(255,0,63)

# dark colors
dark_red=Color(191,0,0)
dark_flame=Color(191,47,0)
dark_orange=Color(191,95,0)
dark_amber=Color(191,143,0)
dark_yellow=Color(191,191,0)
dark_lime=Color(143,191,0)
dark_chartreuse=Color(95,191,0)
dark_green=Color(0,191,0)
dark_sea=Color(0,191,95)
dark_turquoise=Color(0,191,143)
dark_cyan=Color(0,191,191)
dark_sky=Color(0,143,191)
dark_azure=Color(0,95,191)
dark_blue=Color(0,0,191)
dark_han=Color(47,0,191)
dark_violet=Color(95,0,191)
dark_purple=Color(143,0,191)
dark_fuchsia=Color(191,0,191)
dark_magenta=Color(191,0,143)
dark_pink=Color(191,0,95)
dark_crimson=Color(191,0,47)

# darker colors
darker_red=Color(127,0,0)
darker_flame=Color(127,31,0)
darker_orange=Color(127,63,0)
darker_amber=Color(127,95,0)
darker_yellow=Color(127,127,0)
darker_lime=Color(95,127,0)
darker_chartreuse=Color(63,127,0)
darker_green=Color(0,127,0)
darker_sea=Color(0,127,63)
darker_turquoise=Color(0,127,95)
darker_cyan=Color(0,127,127)
darker_sky=Color(0,95,127)
darker_azure=Color(0,63,127)
darker_blue=Color(0,0,127)
darker_han=Color(31,0,127)
darker_violet=Color(63,0,127)
darker_purple=Color(95,0,127)
darker_fuchsia=Color(127,0,127)
darker_magenta=Color(127,0,95)
darker_pink=Color(127,0,63)
darker_crimson=Color(127,0,31)

# darkest colors
darkest_red=Color(63,0,0)
darkest_flame=Color(63,15,0)
darkest_orange=Color(63,31,0)
darkest_amber=Color(63,47,0)
darkest_yellow=Color(63,63,0)
darkest_lime=Color(47,63,0)
darkest_chartreuse=Color(31,63,0)
darkest_green=Color(0,63,0)
darkest_sea=Color(0,63,31)
darkest_turquoise=Color(0,63,47)
darkest_cyan=Color(0,63,63)
darkest_sky=Color(0,47,63)
darkest_azure=Color(0,31,63)
darkest_blue=Color(0,0,63)
darkest_han=Color(15,0,63)
darkest_violet=Color(31,0,63)
darkest_purple=Color(47,0,63)
darkest_fuchsia=Color(63,0,63)
darkest_magenta=Color(63,0,47)
darkest_pink=Color(63,0,31)
darkest_crimson=Color(63,0,15)

# light colors
light_red=Color(255,114,114)
light_flame=Color(255,149,114)
light_orange=Color(255,184,114)
light_amber=Color(255,219,114)
light_yellow=Color(255,255,114)
light_lime=Color(219,255,114)
light_chartreuse=Color(184,255,114)
light_green=Color(114,255,114)
light_sea=Color(114,255,184)
light_turquoise=Color(114,255,219)
light_cyan=Color(114,255,255)
light_sky=Color(114,219,255)
light_azure=Color(114,184,255)
light_blue=Color(114,114,255)
light_han=Color(149,114,255)
light_violet=Color(184,114,255)
light_purple=Color(219,114,255)
light_fuchsia=Color(255,114,255)
light_magenta=Color(255,114,219)
light_pink=Color(255,114,184)
light_crimson=Color(255,114,149)

#lighter colors
lighter_red=Color(255,165,165)
lighter_flame=Color(255,188,165)
lighter_orange=Color(255,210,165)
lighter_amber=Color(255,232,165)
lighter_yellow=Color(255,255,165)
lighter_lime=Color(232,255,165)
lighter_chartreuse=Color(210,255,165)
lighter_green=Color(165,255,165)
lighter_sea=Color(165,255,210)
lighter_turquoise=Color(165,255,232)
lighter_cyan=Color(165,255,255)
lighter_sky=Color(165,232,255)
lighter_azure=Color(165,210,255)
lighter_blue=Color(165,165,255)
lighter_han=Color(188,165,255)
lighter_violet=Color(210,165,255)
lighter_purple=Color(232,165,255)
lighter_fuchsia=Color(255,165,255)
lighter_magenta=Color(255,165,232)
lighter_pink=Color(255,165,210)
lighter_crimson=Color(255,165,188)

# lightest colors
lightest_red=Color(255,191,191)
lightest_flame=Color(255,207,191)
lightest_orange=Color(255,223,191)
lightest_amber=Color(255,239,191)
lightest_yellow=Color(255,255,191)
lightest_lime=Color(239,255,191)
lightest_chartreuse=Color(223,255,191)
lightest_green=Color(191,255,191)
lightest_sea=Color(191,255,223)
lightest_turquoise=Color(191,255,239)
lightest_cyan=Color(191,255,255)
lightest_sky=Color(191,239,255)
lightest_azure=Color(191,223,255)
lightest_blue=Color(191,191,255)
lightest_han=Color(207,191,255)
lightest_violet=Color(223,191,255)
lightest_purple=Color(239,191,255)
lightest_fuchsia=Color(255,191,255)
lightest_magenta=Color(255,191,239)
lightest_pink=Color(255,191,223)
lightest_crimson=Color(255,191,207)

# desaturated colors
desaturated_red=Color(127,63,63)
desaturated_flame=Color(127,79,63)
desaturated_orange=Color(127,95,63)
desaturated_amber=Color(127,111,63)
desaturated_yellow=Color(127,127,63)
desaturated_lime=Color(111,127,63)
desaturated_chartreuse=Color(95,127,63)
desaturated_green=Color(63,127,63)
desaturated_sea=Color(63,127,95)
desaturated_turquoise=Color(63,127,111)
desaturated_cyan=Color(63,127,127)
desaturated_sky=Color(63,111,127)
desaturated_azure=Color(63,95,127)
desaturated_blue=Color(63,63,127)
desaturated_han=Color(79,63,127)
desaturated_violet=Color(95,63,127)
desaturated_purple=Color(111,63,127)
desaturated_fuchsia=Color(127,63,127)
desaturated_magenta=Color(127,63,111)
desaturated_pink=Color(127,63,95)
desaturated_crimson=Color(127,63,79)

# metallic
brass=Color(191,151,96)
copper=Color(197,136,124)
gold=Color(229,191,0)
silver=Color(203,203,203)

# miscellaneous
celadon=Color(172,255,175)
peach=Color(255,159,127)

# color functions
_lib.TCOD_color_lerp.restype = Color
def color_lerp(c1, c2, a):
    return _lib.TCOD_color_lerp(c1, c2, c_float(a))

def color_set_hsv(c, h, s, v):
    _lib.TCOD_color_set_HSV(byref(c), c_float(h), c_float(s), c_float(v))

def color_get_hsv(c):
    h = c_float()
    s = c_float()
    v = c_float()
    _lib.TCOD_color_get_HSV(c, byref(h), byref(s), byref(v))
    return h.value, s.value, v.value

def color_scale_HSV(c, scoef, vcoef) :
    _lib.TCOD_color_scale_HSV(byref(c),c_float(scoef),c_float(vcoef))

def color_gen_map(colors, indexes):
    ccolors = (Color * len(colors))(*colors)
    cindexes = (c_int * len(indexes))(*indexes)
    cres = (Color * (max(indexes) + 1))()
    _lib.TCOD_color_gen_map(cres, len(colors), ccolors, cindexes)
    return cres

############################
# console module
############################
class Key(Structure):
    _fields_=[('vk', c_int),
              ('c', c_uint8),
              ('pressed', c_bool),
              ('lalt', c_bool),
              ('lctrl', c_bool),
              ('ralt', c_bool),
              ('rctrl', c_bool),
              ('shift', c_bool),
              ]

class ConsoleBuffer:
    # simple console that allows direct (fast) access to cells. simplifies
    # use of the "fill" functions.
    def __init__(self, width, height, back_r=0, back_g=0, back_b=0, fore_r=0, fore_g=0, fore_b=0, char=' '):
        # initialize with given width and height. values to fill the buffer
        # are optional, defaults to black with no characters.
        n = width * height
        self.width = width
        self.height = height
        self.clear(back_r, back_g, back_b, fore_r, fore_g, fore_b, char)

    def clear(self, back_r=0, back_g=0, back_b=0, fore_r=0, fore_g=0, fore_b=0, char=' '):
        # clears the console. values to fill it with are optional, defaults
        # to black with no characters.
        n = self.width * self.height
        self.back_r = [back_r] * n
        self.back_g = [back_g] * n
        self.back_b = [back_b] * n
        self.fore_r = [fore_r] * n
        self.fore_g = [fore_g] * n
        self.fore_b = [fore_b] * n
        self.char = [ord(char)] * n
    
    def copy(self):
        # returns a copy of this ConsoleBuffer.
        other = ConsoleBuffer(0, 0)
        other.width = self.width
        other.height = self.height
        other.back_r = list(self.back_r)  # make explicit copies of all lists
        other.back_g = list(self.back_g)
        other.back_b = list(self.back_b)
        other.fore_r = list(self.fore_r)
        other.fore_g = list(self.fore_g)
        other.fore_b = list(self.fore_b)
        other.char = list(self.char)
        return other
    
    def set_fore(self, x, y, r, g, b, char):
        # set the character and foreground color of one cell.
        i = self.width * y + x
        self.fore_r[i] = r
        self.fore_g[i] = g
        self.fore_b[i] = b
        self.char[i] = ord(char)
    
    def set_back(self, x, y, r, g, b):
        # set the background color of one cell.
        i = self.width * y + x
        self.back_r[i] = r
        self.back_g[i] = g
        self.back_b[i] = b
    
    def set(self, x, y, back_r, back_g, back_b, fore_r, fore_g, fore_b, char):
        # set the background color, foreground color and character of one cell.
        i = self.width * y + x
        self.back_r[i] = back_r
        self.back_g[i] = back_g
        self.back_b[i] = back_b
        self.fore_r[i] = fore_r
        self.fore_g[i] = fore_g
        self.fore_b[i] = fore_b
        self.char[i] = ord(char)
    
    def blit(self, dest, fill_fore=True, fill_back=True):
        # use libtcod's "fill" functions to write the buffer to a console.
        if (console_get_width(dest) != self.width or
            console_get_height(dest) != self.height):
            raise ValueError('ConsoleBuffer.blit: Destination console has an incorrect size.')

        s = struct.Struct('%di' % len(self.back_r))

        if fill_back:
            _lib.TCOD_console_fill_background(dest, (c_int * len(self.back_r))(*self.back_r), (c_int * len(self.back_g))(*self.back_g), (c_int * len(self.back_b))(*self.back_b))

        if fill_fore:
            _lib.TCOD_console_fill_foreground(dest, (c_int * len(self.fore_r))(*self.fore_r), (c_int * len(self.fore_g))(*self.fore_g), (c_int * len(self.fore_b))(*self.fore_b))
            _lib.TCOD_console_fill_char(dest, (c_int * len(self.char))(*self.char))

_lib.TCOD_console_credits_render.restype = c_bool
_lib.TCOD_console_is_fullscreen.restype = c_bool
_lib.TCOD_console_is_window_closed.restype = c_bool
_lib.TCOD_console_has_mouse_focus.restype = c_bool
_lib.TCOD_console_is_active.restype = c_bool
_lib.TCOD_console_get_default_background.restype = Color
_lib.TCOD_console_get_default_foreground.restype = Color
_lib.TCOD_console_get_char_background.restype = Color
_lib.TCOD_console_get_char_foreground.restype = Color
_lib.TCOD_console_get_fading_color.restype = Color
_lib.TCOD_console_is_key_pressed.restype = c_bool

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
BKGND_DEFAULT=13

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
# misc characters
CHAR_BULLET = 7
CHAR_BULLET_INV = 8
CHAR_BULLET_SQUARE = 254
CHAR_CENT = 189
CHAR_CLUB = 5
CHAR_COPYRIGHT = 184
CHAR_CURRENCY = 207
CHAR_DIAMOND = 4
CHAR_DIVISION = 246
CHAR_EXCLAM_DOUBLE = 19
CHAR_FEMALE = 12
CHAR_FUNCTION = 159
CHAR_GRADE = 248
CHAR_HALF = 171
CHAR_HEART = 3
CHAR_LIGHT = 15
CHAR_MALE = 11
CHAR_MULTIPLICATION = 158
CHAR_NOTE = 13
CHAR_NOTE_DOUBLE = 14
CHAR_ONE_QUARTER = 172
CHAR_PILCROW = 20
CHAR_POUND = 156
CHAR_POW1 = 251
CHAR_POW2 = 253
CHAR_POW3 = 252
CHAR_RESERVED = 169
CHAR_SECTION = 21
CHAR_SMILIE = 1
CHAR_SMILIE_INV = 2
CHAR_SPADE = 6
CHAR_THREE_QUARTERS = 243
CHAR_UMLAUT = 249
CHAR_YEN = 190
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
# renderers
RENDERER_GLSL=0
RENDERER_OPENGL=1
RENDERER_SDL=2
NB_RENDERERS=3
# alignment
LEFT=0
RIGHT=1
CENTER=2
# initializing the console
def console_init_root(w, h, title, fullscreen=False, renderer=RENDERER_SDL):
    _lib.TCOD_console_init_root(w, h, c_char_p(title), fullscreen, renderer)

def console_get_width(con):
    return _lib.TCOD_console_get_width(con)

def console_get_height(con):
    return _lib.TCOD_console_get_height(con)

def console_set_custom_font(fontFile, flags=FONT_LAYOUT_ASCII_INCOL, nb_char_horiz=0, nb_char_vertic=0):
    _lib.TCOD_console_set_custom_font(c_char_p(fontFile), flags, nb_char_horiz, nb_char_vertic)

def console_map_ascii_code_to_font(asciiCode, fontCharX, fontCharY):
    if type(asciiCode) == str or type(asciiCode) == bytes:
        _lib.TCOD_console_map_ascii_code_to_font(ord(asciiCode), fontCharX,
                                                 fontCharY)
    else:
        _lib.TCOD_console_map_ascii_code_to_font(asciiCode, fontCharX,
                                                 fontCharY)

def console_map_ascii_codes_to_font(firstAsciiCode, nbCodes, fontCharX,
                                    fontCharY):
    if type(firstAsciiCode) == str or type(firstAsciiCode) == bytes:
        _lib.TCOD_console_map_ascii_codes_to_font(ord(firstAsciiCode), nbCodes,
                                                  fontCharX, fontCharY)
    else:
        _lib.TCOD_console_map_ascii_codes_to_font(firstAsciiCode, nbCodes,
                                                  fontCharX, fontCharY)

def console_map_string_to_font(s, fontCharX, fontCharY):
    if type(s) == bytes:
        _lib.TCOD_console_map_string_to_font(s, fontCharX, fontCharY)
    else:
        _lib.TCOD_console_map_string_to_font_utf(s, fontCharX, fontCharY)

def console_is_fullscreen():
    return _lib.TCOD_console_is_fullscreen()

def console_set_fullscreen(fullscreen):
    _lib.TCOD_console_set_fullscreen(c_int(fullscreen))

def console_is_window_closed():
    return _lib.TCOD_console_is_window_closed()

def console_has_mouse_focus():
    return _lib.TCOD_console_has_mouse_focus()

def console_is_active():
    return _lib.TCOD_console_is_active()

def console_set_window_title(title):
    _lib.TCOD_console_set_window_title(c_char_p(title))

def console_credits():
    _lib.TCOD_console_credits()

def console_credits_reset():
    _lib.TCOD_console_credits_reset()

def console_credits_render(x, y, alpha):
    return _lib.TCOD_console_credits_render(x, y, c_int(alpha))

def console_flush():
    _lib.TCOD_console_flush()

# drawing on a console
def console_set_default_background(con, col):
    _lib.TCOD_console_set_default_background(con, col)

def console_set_default_foreground(con, col):
    _lib.TCOD_console_set_default_foreground(con, col)

def console_clear(con):
    return _lib.TCOD_console_clear(con)

def console_put_char(con, x, y, c, flag=BKGND_DEFAULT):
    if type(c) == str or type(c) == bytes:
        _lib.TCOD_console_put_char(con, x, y, ord(c), flag)
    else:
        _lib.TCOD_console_put_char(con, x, y, c, flag)

def console_put_char_ex(con, x, y, c, fore, back):
    if type(c) == str or type(c) == bytes:
        _lib.TCOD_console_put_char_ex(con, x, y, ord(c), fore, back)
    else:
        _lib.TCOD_console_put_char_ex(con, x, y, c, fore, back)

def console_set_char_background(con, x, y, col, flag=BKGND_SET):
    _lib.TCOD_console_set_char_background(con, x, y, col, flag)

def console_set_char_foreground(con, x, y, col):
    _lib.TCOD_console_set_char_foreground(con, x, y, col)

def console_set_char(con, x, y, c):
    if type(c) == str or type(c) == bytes:
        _lib.TCOD_console_set_char(con, x, y, ord(c))
    else:
        _lib.TCOD_console_set_char(con, x, y, c)

def console_set_background_flag(con, flag):
    _lib.TCOD_console_set_background_flag(con, c_int(flag))

def console_get_background_flag(con):
    return _lib.TCOD_console_get_background_flag(con)

def console_set_alignment(con, alignment):
    _lib.TCOD_console_set_alignment(con, c_int(alignment))

def console_get_alignment(con):
    return _lib.TCOD_console_get_alignment(con)

def console_print(con, x, y, fmt):
    if type(fmt) == bytes:
        _lib.TCOD_console_print(c_void_p(con), x, y, c_char_p(fmt))
    else:
        _lib.TCOD_console_print_utf(c_void_p(con), x, y, fmt)

def console_print_ex(con, x, y, flag, alignment, fmt):
    if type(fmt) == bytes:
        _lib.TCOD_console_print_ex(c_void_p(con), x, y, flag, alignment, c_char_p(fmt))
    else:
        _lib.TCOD_console_print_ex_utf(c_void_p(con), x, y, flag, alignment, fmt)

def console_print_rect(con, x, y, w, h, fmt):
    if type(fmt) == bytes:
        return _lib.TCOD_console_print_rect(c_void_p(con), x, y, w, h, c_char_p(fmt))
    else:
        return _lib.TCOD_console_print_rect_utf(c_void_p(con), x, y, w, h, fmt)

def console_print_rect_ex(con, x, y, w, h, flag, alignment, fmt):
    if type(fmt) == bytes:
        return _lib.TCOD_console_print_rect_ex(c_void_p(con), x, y, w, h, flag, alignment, c_char_p(fmt))
    else:
        return _lib.TCOD_console_print_rect_ex_utf(c_void_p(con), x, y, w, h, flag, alignment, fmt)

def console_get_height_rect(con, x, y, w, h, fmt):
    if type(fmt) == bytes:
        return _lib.TCOD_console_get_height_rect(c_void_p(con), x, y, w, h, c_char_p(fmt))
    else:
        return _lib.TCOD_console_get_height_rect_utf(c_void_p(con), x, y, w, h, fmt)

def console_rect(con, x, y, w, h, clr, flag=BKGND_DEFAULT):
    _lib.TCOD_console_rect(con, x, y, w, h, c_int(clr), flag)

def console_hline(con, x, y, l, flag=BKGND_DEFAULT):
    _lib.TCOD_console_hline( con, x, y, l, flag)

def console_vline(con, x, y, l, flag=BKGND_DEFAULT):
    _lib.TCOD_console_vline( con, x, y, l, flag)

def console_print_frame(con, x, y, w, h, clear=True, flag=BKGND_DEFAULT, fmt=0):
    _lib.TCOD_console_print_frame(c_void_p(con), x, y, w, h, c_int(clear), flag, c_char_p(fmt))

def console_set_color_control(con,fore,back) :
    _lib.TCOD_console_set_color_control(con,fore,back)

def console_get_default_background(con):
    return _lib.TCOD_console_get_default_background(con)

def console_get_default_foreground(con):
    return _lib.TCOD_console_get_default_foreground(con)

def console_get_char_background(con, x, y):
    return _lib.TCOD_console_get_char_background(con, x, y)

def console_get_char_foreground(con, x, y):
    return _lib.TCOD_console_get_char_foreground(con, x, y)

def console_get_char(con, x, y):
    return _lib.TCOD_console_get_char(con, x, y)

def console_set_fade(fade, fadingColor):
    _lib.TCOD_console_set_fade(fade, fadingColor)
    ##_lib.TCOD_console_set_fade_wrapper(fade, fadingColor)

def console_get_fade():
    return _lib.TCOD_console_get_fade().value

def console_get_fading_color():
    return _lib.TCOD_console_get_fading_color()

# handling keyboard input
def console_wait_for_keypress(flush):
    k=Key()
    _lib.TCOD_console_wait_for_keypress_wrapper(byref(k),c_bool(flush))
    return k

def console_check_for_keypress(flags=KEY_RELEASED):
    k=Key()
    _lib.TCOD_console_check_for_keypress_wrapper(byref(k),c_int(flags))
    return k

def console_is_key_pressed(key):
    return _lib.TCOD_console_is_key_pressed(key)

def console_set_keyboard_repeat(initial_delay, interval):
    _lib.TCOD_console_set_keyboard_repeat(initial_delay, interval)

def console_disable_keyboard_repeat():
    _lib.TCOD_console_disable_keyboard_repeat()

# using offscreen consoles
def console_new(w, h):
    return _lib.TCOD_console_new(w, h)
def console_from_file(filename):
    return _lib.TCOD_console_from_file(filename)
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
    if len(r) != len(g) or len(r) != len(b):
        raise TypeError('R, G and B must all have the same size.')

    if (numpy_available and isinstance(r, numpy.ndarray) and
        isinstance(g, numpy.ndarray) and isinstance(b, numpy.ndarray)):
        #numpy arrays, use numpy's ctypes functions
        r = numpy.ascontiguousarray(r, dtype=numpy.int32)
        g = numpy.ascontiguousarray(g, dtype=numpy.int32)
        b = numpy.ascontiguousarray(b, dtype=numpy.int32)
        cr = r.ctypes.data_as(POINTER(c_int))
        cg = g.ctypes.data_as(POINTER(c_int))
        cb = b.ctypes.data_as(POINTER(c_int))
    else:
        # otherwise convert using ctypes arrays
        cr = (c_int * len(r))(*r)
        cg = (c_int * len(g))(*g)
        cb = (c_int * len(b))(*b)

    _lib.TCOD_console_fill_foreground(con, cr, cg, cb)

def console_fill_background(con,r,g,b) :
    if len(r) != len(g) or len(r) != len(b):
        raise TypeError('R, G and B must all have the same size.')

    if (numpy_available and isinstance(r, numpy.ndarray) and
        isinstance(g, numpy.ndarray) and isinstance(b, numpy.ndarray)):
        #numpy arrays, use numpy's ctypes functions
        r = numpy.ascontiguousarray(r, dtype=numpy.int32)
        g = numpy.ascontiguousarray(g, dtype=numpy.int32)
        b = numpy.ascontiguousarray(b, dtype=numpy.int32)
        cr = r.ctypes.data_as(POINTER(c_int))
        cg = g.ctypes.data_as(POINTER(c_int))
        cb = b.ctypes.data_as(POINTER(c_int))
    else:
        # otherwise convert using ctypes arrays
        cr = (c_int * len(r))(*r)
        cg = (c_int * len(g))(*g)
        cb = (c_int * len(b))(*b)

    _lib.TCOD_console_fill_background(con, cr, cg, cb)

def console_fill_char(con,arr) :
    if (numpy_available and isinstance(arr, numpy.ndarray) ):
        #numpy arrays, use numpy's ctypes functions
        arr = numpy.ascontiguousarray(arr, dtype=numpy.int32)
        carr = arr.ctypes.data_as(POINTER(c_int))
    else:
        #otherwise convert using the struct module
        carr = struct.pack('%di' % len(arr), *arr)

    _lib.TCOD_console_fill_char(con, carr)
        
def console_load_asc(con, filename) :
    _lib.TCOD_console_load_asc(con,filename)
def console_save_asc(con, filename) :
    _lib.TCOD_console_save_asc(con,filename)
def console_load_apf(con, filename) :
    _lib.TCOD_console_load_apf(con,filename)
def console_save_apf(con, filename) :
    _lib.TCOD_console_save_apf(con,filename)

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

def sys_elapsed_milli():
    return _lib.TCOD_sys_elapsed_milli()

def sys_elapsed_seconds():
    return _lib.TCOD_sys_elapsed_seconds()

def sys_set_renderer(renderer):
    _lib.TCOD_sys_set_renderer(renderer)

def sys_get_renderer():
    return _lib.TCOD_sys_get_renderer()

# easy screenshots
def sys_save_screenshot(name=0):
    _lib.TCOD_sys_save_screenshot(c_char_p(name))

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
SDL_RENDERER_FUNC = CFUNCTYPE(None, c_void_p)
def sys_register_SDL_renderer(callback):
    global sdl_renderer_func
    sdl_renderer_func = SDL_RENDERER_FUNC(callback)
    _lib.TCOD_sys_register_SDL_renderer(sdl_renderer_func)

# events
EVENT_NONE=0
EVENT_KEY_PRESS=1
EVENT_KEY_RELEASE=2
EVENT_KEY=EVENT_KEY_PRESS|EVENT_KEY_RELEASE
EVENT_MOUSE_MOVE=4
EVENT_MOUSE_PRESS=8
EVENT_MOUSE_RELEASE=16
EVENT_MOUSE=EVENT_MOUSE_MOVE|EVENT_MOUSE_PRESS|EVENT_MOUSE_RELEASE
EVENT_ANY=EVENT_KEY|EVENT_MOUSE
def sys_check_for_event(mask,k,m) :
    return _lib.TCOD_sys_check_for_event(c_int(mask),byref(k),byref(m))

def sys_wait_for_event(mask,k,m,flush) :
    return _lib.TCOD_sys_wait_for_event(c_int(mask),byref(k),byref(m),c_bool(flush))

############################
# line module
############################
_lib.TCOD_line_step.restype = c_bool
_lib.TCOD_line.restype=c_bool
_lib.TCOD_line_step_mt.restype = c_bool

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
    LINE_CBK_FUNC=CFUNCTYPE(c_bool,c_int,c_int)
    c_callback=LINE_CBK_FUNC(py_callback)
    return _lib.TCOD_line(xo,yo,xd,yd,c_callback)

def line_iter(xo, yo, xd, yd):
    data = (c_int * 9)()        # struct TCOD_bresenham_data_t
    _lib.TCOD_line_init_mt(xo, yo, xd, yd, data)
    x = c_int(xo)
    y = c_int(yo)
    done = False
    while not done:
        yield x.value, y.value
        done = _lib.TCOD_line_step_mt(byref(x), byref(y), data)

############################
# image module
############################
_lib.TCOD_image_is_pixel_transparent.restype = c_bool
_lib.TCOD_image_get_pixel.restype = Color
_lib.TCOD_image_get_mipmap_pixel.restype = Color

def image_new(width, height):
    return _lib.TCOD_image_new(width, height)

def image_clear(image,col) :
    _lib.TCOD_image_clear(image,col)

def image_invert(image) :
    _lib.TCOD_image_invert(image)

def image_hflip(image) :
    _lib.TCOD_image_hflip(image)

def image_rotate90(image, num=1) :
    _lib.TCOD_image_rotate90(image,num)

def image_vflip(image) :
    _lib.TCOD_image_vflip(image)

def image_scale(image, neww, newh) :
    _lib.TCOD_image_scale(image,c_int(neww),c_int(newh))

def image_set_key_color(image,col) :
    _lib.TCOD_image_set_key_color(image,col)

def image_get_alpha(image,x,y) :
    return _lib.TCOD_image_get_alpha(image,c_int(x),c_int(y))

def image_is_pixel_transparent(image,x,y) :
    return _lib.TCOD_image_is_pixel_transparent(image,c_int(x),c_int(y))

def image_load(filename):
    return _lib.TCOD_image_load(c_char_p(filename))

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
    return _lib.TCOD_image_get_pixel(image, x, y)

def image_get_mipmap_pixel(image, x0, y0, x1, y1):
    return _lib.TCOD_image_get_mipmap_pixel(image, c_float(x0), c_float(y0),
                                            c_float(x1), c_float(y1))
def image_put_pixel(image, x, y, col):
    _lib.TCOD_image_put_pixel(image, x, y, col)
    ##_lib.TCOD_image_put_pixel_wrapper(image, x, y, col)

def image_blit(image, console, x, y, bkgnd_flag, scalex, scaley, angle):
    _lib.TCOD_image_blit(image, console, c_float(x), c_float(y), bkgnd_flag,
                         c_float(scalex), c_float(scaley), c_float(angle))

def image_blit_rect(image, console, x, y, w, h, bkgnd_flag):
    _lib.TCOD_image_blit_rect(image, console, x, y, w, h, bkgnd_flag)

def image_blit_2x(image, console, dx, dy, sx=0, sy=0, w=-1, h=-1):
    _lib.TCOD_image_blit_2x(image, console, dx,dy,sx,sy,w,h)

def image_save(image, filename):
    _lib.TCOD_image_save(image, c_char_p(filename))

def image_delete(image):
    _lib.TCOD_image_delete(image)

############################
# mouse module
############################
class Mouse(Structure):
    _fields_=[('x', c_int),
              ('y', c_int),
              ('dx', c_int),
              ('dy', c_int),
              ('cx', c_int),
              ('cy', c_int),
              ('dcx', c_int),
              ('dcy', c_int),
              ('lbutton', c_bool),
              ('rbutton', c_bool),
              ('mbutton', c_bool),
              ('lbutton_pressed', c_bool),
              ('rbutton_pressed', c_bool),
              ('mbutton_pressed', c_bool),
              ('wheel_up', c_bool),
              ('wheel_down', c_bool),
              ]

_lib.TCOD_mouse_is_cursor_visible.restype = c_bool

def mouse_show_cursor(visible):
    _lib.TCOD_mouse_show_cursor(c_int(visible))

def mouse_is_cursor_visible():
    return _lib.TCOD_mouse_is_cursor_visible()

def mouse_move(x, y):
    _lib.TCOD_mouse_move(x, y)

def mouse_get_status():
    mouse=Mouse()
    _lib.TCOD_mouse_get_status_wrapper(byref(mouse))
    return mouse

############################
# parser module
############################
_lib.TCOD_struct_get_name.restype = c_char_p
_lib.TCOD_struct_is_mandatory.restype = c_bool
_lib.TCOD_parser_has_property.restype = c_bool
_lib.TCOD_parser_get_bool_property.restype = c_bool
_lib.TCOD_parser_get_float_property.restype = c_float
_lib.TCOD_parser_get_string_property.restype = c_char_p
_lib.TCOD_parser_get_color_property.restype = Color

class Dice(Structure):
    _fields_=[('nb_dices', c_int),
              ('nb_faces', c_int),
              ('multiplier', c_float),
              ('addsub', c_float),
              ]

    def __repr__(self):
        return "Dice(%d, %d, %s, %s)" % (self.nb_dices, self.nb_faces,
                                      self.multiplier, self.addsub)

class _CValue(Union):
    _fields_=[('c',c_uint8),
              ('i',c_int),
              ('f',c_float),
              ('s',c_char_p),
              # JBR03192012 See http://bugs.python.org/issue14354 for why these are not defined as their actual types
              ('col',c_uint8 * 3),
              ('dice',c_int * 4),
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

def _convert_TCODList(clist, typ):
    res = list()
    for i in range(_lib.TCOD_list_size(clist)):
        elt = _lib.TCOD_list_get(clist, i)
        elt = cast(elt, c_void_p)
        if typ == TYPE_BOOL:
            elt = c_bool.from_buffer(elt).value
        elif typ == TYPE_CHAR:
            elt = c_char.from_buffer(elt).value
        elif typ == TYPE_INT:
            elt = c_int.from_buffer(elt).value
        elif typ == TYPE_FLOAT:
            elt = c_float.from_buffer(elt).value
        elif typ == TYPE_STRING or TYPE_VALUELIST15 >= typ >= TYPE_VALUELIST00:
            elt = cast(elt, c_char_p).value
        elif typ == TYPE_COLOR:
            elt = Color.from_buffer_copy(elt)
        elif typ == TYPE_DICE:
            # doesn't work
            elt = Dice.from_buffer_copy(elt)
        res.append(elt)
    return res

def parser_new():
    return _lib.TCOD_parser_new()

def parser_new_struct(parser, name):
    return _lib.TCOD_parser_new_struct(parser, name)

def struct_add_flag(struct, name):
    _lib.TCOD_struct_add_flag(struct, name)

def struct_add_property(struct, name, typ, mandatory):
    _lib.TCOD_struct_add_property(struct, name, typ, c_bool(mandatory))

def struct_add_value_list(struct, name, value_list, mandatory):
    CARRAY = c_char_p * (len(value_list) + 1)
    cvalue_list = CARRAY()
    for i in range(len(value_list)):
        cvalue_list[i] = cast(value_list[i], c_char_p)
    cvalue_list[len(value_list)] = 0
    _lib.TCOD_struct_add_value_list(struct, name, cvalue_list, c_bool(mandatory))

def struct_add_list_property(struct, name, typ, mandatory):
    _lib.TCOD_struct_add_list_property(struct, name, typ, c_bool(mandatory))

def struct_add_structure(struct, sub_struct):
    _lib.TCOD_struct_add_structure(struct, sub_struct)

def struct_get_name(struct):
    return _lib.TCOD_struct_get_name(struct)

def struct_is_mandatory(struct, name):
    return _lib.TCOD_struct_is_mandatory(struct, name)

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
                col = cast(value.col, POINTER(Color)).contents
                return listener.new_property(name, typ, col)
            elif typ == TYPE_DICE:
                dice = cast(value.dice, POINTER(Dice)).contents
                return listener.new_property(name, typ, dice)
            elif typ & TYPE_LIST:
                return listener.new_property(name, typ,
                                        _convert_TCODList(value.custom, typ & 0xFF))
            return True
        clistener.new_struct = _CFUNC_NEW_STRUCT(listener.new_struct)
        clistener.new_flag = _CFUNC_NEW_FLAG(listener.new_flag)
        clistener.new_property = _CFUNC_NEW_PROPERTY(value_converter)
        clistener.end_struct = _CFUNC_NEW_STRUCT(listener.end_struct)
        clistener.error = _CFUNC_NEW_FLAG(listener.error)
        _lib.TCOD_parser_run(parser, c_char_p(filename), byref(clistener))
    else:
        _lib.TCOD_parser_run(parser, c_char_p(filename), 0)

def parser_delete(parser):
    _lib.TCOD_parser_delete(parser)

def parser_has_property(parser, name):
    return _lib.TCOD_parser_has_property(parser, c_char_p(name))

def parser_get_bool_property(parser, name):
    return _lib.TCOD_parser_get_bool_property(parser, c_char_p(name))

def parser_get_int_property(parser, name):
    return _lib.TCOD_parser_get_int_property(parser, c_char_p(name))

def parser_get_char_property(parser, name):
    return '%c' % _lib.TCOD_parser_get_char_property(parser, c_char_p(name))

def parser_get_float_property(parser, name):
    return _lib.TCOD_parser_get_float_property(parser, c_char_p(name))

def parser_get_string_property(parser, name):
    return _lib.TCOD_parser_get_string_property(parser, c_char_p(name))

def parser_get_color_property(parser, name):
    return _lib.TCOD_parser_get_color_property(parser, c_char_p(name))

def parser_get_dice_property(parser, name):
    d = Dice()
    _lib.TCOD_parser_get_dice_property_py(c_void_p(parser), c_char_p(name), byref(d))
    return d

def parser_get_list_property(parser, name, typ):
    clist = _lib.TCOD_parser_get_list_property(parser, c_char_p(name), c_int(typ))
    return _convert_TCODList(clist, typ)

############################
# random module
############################
_lib.TCOD_random_get_float.restype = c_float
_lib.TCOD_random_get_double.restype = c_double

RNG_MT = 0
RNG_CMWC = 1

DISTRIBUTION_LINEAR = 0
DISTRIBUTION_GAUSSIAN = 1
DISTRIBUTION_GAUSSIAN_RANGE = 2
DISTRIBUTION_GAUSSIAN_INVERSE = 3
DISTRIBUTION_GAUSSIAN_RANGE_INVERSE = 4

def random_get_instance():
    return _lib.TCOD_random_get_instance()

def random_new(algo=RNG_CMWC):
    return _lib.TCOD_random_new(algo)

def random_new_from_seed(seed, algo=RNG_CMWC):
    return _lib.TCOD_random_new_from_seed(algo,c_uint(seed))

def random_set_distribution(rnd, dist) :
	_lib.TCOD_random_set_distribution(rnd, dist)

def random_get_int(rnd, mi, ma):
    return _lib.TCOD_random_get_int(rnd, mi, ma)

def random_get_float(rnd, mi, ma):
    return _lib.TCOD_random_get_float(rnd, c_float(mi), c_float(ma))

def random_get_double(rnd, mi, ma):
    return _lib.TCOD_random_get_double(rnd, c_double(mi), c_double(ma))

def random_get_int_mean(rnd, mi, ma, mean):
    return _lib.TCOD_random_get_int_mean(rnd, mi, ma, mean)

def random_get_float_mean(rnd, mi, ma, mean):
    return _lib.TCOD_random_get_float_mean(rnd, c_float(mi), c_float(ma), c_float(mean))

def random_get_double_mean(rnd, mi, ma, mean):
    return _lib.TCOD_random_get_double_mean(rnd, c_double(mi), c_double(ma), c_double(mean))

def random_save(rnd):
    return _lib.TCOD_random_save(rnd)

def random_restore(rnd, backup):
    _lib.TCOD_random_restore(rnd, backup)

def random_delete(rnd):
    _lib.TCOD_random_delete(rnd)

############################
# noise module
############################
_lib.TCOD_noise_get.restype = c_float
_lib.TCOD_noise_get_ex.restype = c_float
_lib.TCOD_noise_get_fbm.restype = c_float
_lib.TCOD_noise_get_fbm_ex.restype = c_float
_lib.TCOD_noise_get_turbulence.restype = c_float
_lib.TCOD_noise_get_turbulence_ex.restype = c_float

NOISE_DEFAULT_HURST = 0.5
NOISE_DEFAULT_LACUNARITY = 2.0

NOISE_DEFAULT = 0
NOISE_PERLIN = 1
NOISE_SIMPLEX = 2
NOISE_WAVELET = 4

_NOISE_PACKER_FUNC = (None,
                      (c_float * 1),
                      (c_float * 2),
                      (c_float * 3),
                      (c_float * 4),
                      )

def noise_new(dim, h=NOISE_DEFAULT_HURST, l=NOISE_DEFAULT_LACUNARITY, random=0):
    return _lib.TCOD_noise_new(dim, c_float(h), c_float(l), random)

def noise_set_type(n, typ) :
    _lib.TCOD_noise_set_type(n,typ)

def noise_get(n, f, typ=NOISE_DEFAULT):
    return _lib.TCOD_noise_get_ex(n, _NOISE_PACKER_FUNC[len(f)](*f), typ)

def noise_get_fbm(n, f, oc, typ=NOISE_DEFAULT):
    return _lib.TCOD_noise_get_fbm_ex(n, _NOISE_PACKER_FUNC[len(f)](*f), c_float(oc), typ)

def noise_get_turbulence(n, f, oc, typ=NOISE_DEFAULT):
    return _lib.TCOD_noise_get_turbulence_ex(n, _NOISE_PACKER_FUNC[len(f)](*f), c_float(oc), typ)

def noise_delete(n):
    _lib.TCOD_noise_delete(n)

############################
# fov module
############################
_lib.TCOD_map_is_in_fov.restype = c_bool
_lib.TCOD_map_is_transparent.restype = c_bool
_lib.TCOD_map_is_walkable.restype = c_bool

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

def map_clear(m,walkable=False,transparent=False):
    _lib.TCOD_map_clear(m,c_int(walkable),c_int(transparent))

def map_compute_fov(m, x, y, radius=0, light_walls=True, algo=FOV_RESTRICTIVE ):
    _lib.TCOD_map_compute_fov(m, x, y, c_int(radius), c_bool(light_walls), c_int(algo))

def map_is_in_fov(m, x, y):
    return _lib.TCOD_map_is_in_fov(m, x, y)

def map_is_transparent(m, x, y):
    return _lib.TCOD_map_is_transparent(m, x, y)

def map_is_walkable(m, x, y):
    return _lib.TCOD_map_is_walkable(m, x, y)

def map_delete(m):
    return _lib.TCOD_map_delete(m)

def map_get_width(map):
    return _lib.TCOD_map_get_width(map)

def map_get_height(map):
    return _lib.TCOD_map_get_height(map)

############################
# pathfinding module
############################
_lib.TCOD_path_compute.restype = c_bool
_lib.TCOD_path_is_empty.restype = c_bool
_lib.TCOD_path_walk.restype = c_bool

PATH_CBK_FUNC = CFUNCTYPE(c_float, c_int, c_int, c_int, c_int, py_object)

def path_new_using_map(m, dcost=1.41):
    return (_lib.TCOD_path_new_using_map(c_void_p(m), c_float(dcost)), None)

def path_new_using_function(w, h, func, userdata=0, dcost=1.41):
    cbk_func = PATH_CBK_FUNC(func)
    return (_lib.TCOD_path_new_using_function(w, h, cbk_func,
            py_object(userdata), c_float(dcost)), cbk_func)

def path_compute(p, ox, oy, dx, dy):
    return _lib.TCOD_path_compute(p[0], ox, oy, dx, dy)

def path_get_origin(p):
    x = c_int()
    y = c_int()
    _lib.TCOD_path_get_origin(p[0], byref(x), byref(y))
    return x.value, y.value

def path_get_destination(p):
    x = c_int()
    y = c_int()
    _lib.TCOD_path_get_destination(p[0], byref(x), byref(y))
    return x.value, y.value

def path_size(p):
    return _lib.TCOD_path_size(p[0])

def path_reverse(p):
    _lib.TCOD_path_reverse(p[0])  

def path_get(p, idx):
    x = c_int()
    y = c_int()
    _lib.TCOD_path_get(p[0], idx, byref(x), byref(y))
    return x.value, y.value

def path_is_empty(p):
    return _lib.TCOD_path_is_empty(p[0])

def path_walk(p, recompute):
    x = c_int()
    y = c_int()
    if _lib.TCOD_path_walk(p[0], byref(x), byref(y), c_int(recompute)):
        return x.value, y.value
    return None,None

def path_delete(p):
    _lib.TCOD_path_delete(p[0])

_lib.TCOD_dijkstra_path_set.restype = c_bool
_lib.TCOD_dijkstra_is_empty.restype = c_bool
_lib.TCOD_dijkstra_path_walk.restype = c_bool
_lib.TCOD_dijkstra_get_distance.restype = c_float

def dijkstra_new(m, dcost=1.41):
    return (_lib.TCOD_dijkstra_new(c_void_p(m), c_float(dcost)), None)

def dijkstra_new_using_function(w, h, func, userdata=0, dcost=1.41):
    cbk_func = PATH_CBK_FUNC(func)
    return (_lib.TCOD_path_dijkstra_using_function(w, h, cbk_func,
            py_object(userdata), c_float(dcost)), cbk_func)

def dijkstra_compute(p, ox, oy):
    _lib.TCOD_dijkstra_compute(p[0], c_int(ox), c_int(oy))

def dijkstra_path_set(p, x, y):
    return _lib.TCOD_dijkstra_path_set(p[0], c_int(x), c_int(y))

def dijkstra_get_distance(p, x, y):
    return _lib.TCOD_dijkstra_get_distance(p[0], c_int(x), c_int(y))

def dijkstra_size(p):
    return _lib.TCOD_dijkstra_size(p[0])

def dijkstra_reverse(p):
    _lib.TCOD_dijkstra_reverse(p[0])

def dijkstra_get(p, idx):
    x = c_int()
    y = c_int()
    _lib.TCOD_dijkstra_get(p[0], c_int(idx), byref(x), byref(y))
    return x.value, y.value

def dijkstra_is_empty(p):
    return _lib.TCOD_dijkstra_is_empty(p[0])

def dijkstra_path_walk(p):
    x = c_int()
    y = c_int()
    if _lib.TCOD_dijkstra_path_walk(p[0], byref(x), byref(y)):
        return x.value, y.value
    return None,None

def dijkstra_delete(p):
    _lib.TCOD_dijkstra_delete(p[0])

############################
# bsp module
############################
class _CBsp(Structure):
    _fields_ = [('next', c_void_p),
                ('father', c_void_p),
                ('son', c_void_p),
                ('x', c_int),
                ('y', c_int),
                ('w', c_int),
                ('h', c_int),
                ('position', c_int),
                ('level', c_uint8),
                ('horizontal', c_bool),
                ]

_lib.TCOD_bsp_new_with_size.restype = POINTER(_CBsp)
_lib.TCOD_bsp_left.restype = POINTER(_CBsp)
_lib.TCOD_bsp_right.restype = POINTER(_CBsp)
_lib.TCOD_bsp_father.restype = POINTER(_CBsp)
_lib.TCOD_bsp_is_leaf.restype = c_bool
_lib.TCOD_bsp_contains.restype = c_bool
_lib.TCOD_bsp_find_node.restype = POINTER(_CBsp)

BSP_CBK_FUNC = CFUNCTYPE(c_int, c_void_p, c_void_p)

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
        return self.p.contents.horizontal
    def sethor(self,value):
        self.p.contents.horizontal = value
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
    return _lib.TCOD_bsp_is_leaf(node.p)

def bsp_contains(node, cx, cy):
    return _lib.TCOD_bsp_contains(node.p, cx, cy)

def bsp_find_node(node, cx, cy):
    return Bsp(_lib.TCOD_bsp_find_node(node.p, cx, cy))

def _bsp_traverse(node, callback, userData, func):
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
_lib.TCOD_heightmap_has_land_on_border.restype = c_bool

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

def heightmap_mid_point_displacement(hm, rng, roughness):
    _lib.TCOD_heightmap_mid_point_displacement(hm.p, rng, c_float(roughness))

def heightmap_rain_erosion(hm, nbDrops, erosionCoef, sedimentationCoef, rnd=0):
    _lib.TCOD_heightmap_rain_erosion(hm.p, nbDrops, c_float( erosionCoef),
                                     c_float( sedimentationCoef), rnd)

def heightmap_kernel_transform(hm, kernelsize, dx, dy, weight, minLevel,
                               maxLevel):
    FARRAY = c_float * kernelsize
    IARRAY = c_int * kernelsize
    cdx = IARRAY(*dx)
    cdy = IARRAY(*dy)
    cweight = FARRAY(*weight)
    _lib.TCOD_heightmap_kernel_transform(hm.p, kernelsize, cdx, cdy, cweight,
                                         c_float(minLevel), c_float(maxLevel))

def heightmap_add_voronoi(hm, nbPoints, nbCoef, coef, rnd=0):
    FARRAY = c_float * nbCoef
    ccoef = FARRAY(*coef)
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
    cpx = IARRAY(*px)
    cpy = IARRAY(*py)
    _lib.TCOD_heightmap_dig_bezier(hm.p, cpx, cpy, c_float(startRadius),
                                   c_float(startDepth), c_float(endRadius),
                                   c_float(endDepth))

def heightmap_get_value(hm, x, y):
    return _lib.TCOD_heightmap_get_value(hm.p, x, y)

def heightmap_get_interpolated_value(hm, x, y):
    return _lib.TCOD_heightmap_get_interpolated_value(hm.p, c_float(x),
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
    return _lib.TCOD_heightmap_has_land_on_border(hm.p, c_float(waterlevel))

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
_lib.TCOD_namegen_generate.restype = c_char_p
_lib.TCOD_namegen_generate_custom.restype = c_char_p

def namegen_parse(filename,random=0) :
    _lib.TCOD_namegen_parse(filename,random)

def namegen_generate(name) :
    return _lib.TCOD_namegen_generate(name, 0)

def namegen_generate_custom(name, rule) :
    return _lib.TCOD_namegen_generate(name, rule, 0)

def namegen_get_sets():
    nb=_lib.TCOD_namegen_get_nb_sets_wrapper()
    SARRAY = c_char_p * nb;
    setsa = SARRAY()
    _lib.TCOD_namegen_get_sets_wrapper(setsa)
    return list(setsa)

def namegen_destroy() :
    _lib.TCOD_namegen_destroy()


