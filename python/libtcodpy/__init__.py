#
# libtcod Python wrapper
# Copyright (c) 2008-2018 Jice & Mingos & rmtew
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * The name of Jice or Mingos may not be used to endorse or promote
#       products derived from this software without specific prior written
#       permission.
#
# THIS SOFTWARE IS PROVIDED BY JICE, MINGOS AND RMTEW ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL JICE, MINGOS OR RMTEW BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

from __future__ import print_function
import os
import sys
import ctypes
import struct
import warnings
from ctypes import *

# We do not have a fully unicode API on libtcod, so all unicode strings have to
# be implicitly converted to ascii, and any unicode specific operations have to
# be explicitly made by users
#   v = v.encode('latin-1')
# Returned byte strings from the api, should be converted to unicode, so that
# if formatted via %, do not appear as "b'sds'".
#   v = v.decode("utf-8")
is_python_3 = sys.version_info > (3, 0)

if is_python_3:
    def convert_to_ascii(v):
        if not isinstance(v, bytes):
            return v.encode('utf-8')
        warnings.warn("Passing bytes to this call is deprecated.",
                      DeprecationWarning, stacklevel=3)
        return v
else:
    def convert_to_ascii(v):
        if isinstance(v, unicode):
            return v.encode('utf-8')
        return v

if sys.version_info[0] == 2: # Python 2
    def _bytes(string):
        if isinstance(string, unicode):
            return string.encode('latin-1')
        return string

    def _unicode(string):
        if not isinstance(string, unicode):
            return string.decode('latin-1')
        return string

else: # Python 3
    def _bytes(string):
        if isinstance(string, str):
            return string.encode('latin-1')
        warnings.warn("Passing bytes to this call is deprecated.",
                      DeprecationWarning, stacklevel=4)
        return string

    def _unicode(string):
        if isinstance(string, bytes):
            warnings.warn("Passing bytes to this call is deprecated.",
                          DeprecationWarning, stacklevel=4)
            return string.decode('latin-1')
        return string


def _fmt_bytes(string):
    return _bytes(string).replace(b'%', b'%%')

def _fmt_unicode(string):
    return _unicode(string).replace(u'%', u'%%')

if not hasattr(ctypes, "c_bool"):   # for Python < 2.6
    c_bool = c_uint8

c_void = None

try:  #import NumPy if available
    import numpy
    numpy_available = True
except ImportError:
    numpy_available = False

LINUX=False
MAC=False
MINGW=False
MSVC=False

def _get_cdll(libname):
    '''
        get the library libname using a manual search path that will first
        check the package directory and then the development path

        returns the ctypes lib object
    '''
    def get_pe_architecture(filePath):
        # From: https://github.com/tgandor/meats/blob/master/missing/arch_of.py
        with open(filePath, 'rb') as f:
            doshdr = f.read(64)
            magic, padding, offset = struct.unpack('2s58si', doshdr)
            # print (magic, offset)
            if magic != b'MZ':
                return None
            f.seek(offset, os.SEEK_SET)
            pehdr = f.read(6)
            # careful! H == unsigned short, x64 is negative with signed
            magic, padding, machine = struct.unpack('2s2sH', pehdr)
            # print (magic, hex(machine))
            if magic != b'PE':
                return None
            if machine == 0x014c:
                return 'i386'
            if machine == 0x0200:
                return 'IA64'
            if machine == 0x8664:
                return 'x64'
            return 'unknown'

    pythonExePath = sys.executable
    pythonExeArchitecture = get_pe_architecture(pythonExePath)

    pathsToTry = []
    # 1. Try the directory this script is located in.
    pathsToTry.append(os.path.join(__path__[0], libname))
    # 2. Try the directory of the command-line script.
    scriptFilePath = sys.argv[0]
    scriptPath = os.path.dirname(scriptFilePath)
    if len(scriptPath):
        pathsToTry.append(os.path.join(scriptPath, libname))
    else:
        pathsToTry.append(os.path.join(os.getcwd(), libname))
    # 3. Try the environment variable LIBTCOD_DLL_PATH.
    if "LIBTCOD_DLL_PATH" in os.environ:
        envPaths = os.environ["LIBTCOD_DLL_PATH"].split(";")
        for envPath in envPaths:
            if os.path.exists(envPath):
                pathsToTry.append(os.path.join(envPath, libname))
    # 4. Try the top-level path in the development tree.
    potentialTopLevelPath = os.path.realpath(os.path.join(__path__[0], os.pardir, os.pardir))
    pythonPath = os.path.join(potentialTopLevelPath, "python")
    if os.path.exists(pythonPath):
        pathsToTry.append(os.path.join(potentialTopLevelPath, libname))

    for libPath in pathsToTry:
        if os.path.exists(libPath):
            # get library from the package
            libArchitecture = get_pe_architecture(libPath)
            if libArchitecture != pythonExeArchitecture:
                libName = os.path.basename(libPath)
                print ("Error: Incompatible architecture, python is %s, %s is %s" % (pythonExeArchitecture, libName, libArchitecture))
                sys.exit(1)
            return ctypes.cdll[libPath]

    raise Exception("unable to locate: "+ libname)

if sys.platform.find('linux') != -1:
    _lib = _get_cdll('libtcod.so')
    LINUX=True
elif sys.platform.find('darwin') != -1:
    _lib = _get_cdll('libtcod.dylib')
    MAC = True
elif sys.platform.find('haiku') != -1:
    _lib = _get_cdll('libtcod.so')
    HAIKU = True
else:
    _get_cdll('SDL2.dll')
    _lib = _get_cdll('libtcod.dll')
    MSVC=True
    # On Windows, ctypes doesn't work well with function returning structs,
    # so we have to user the _wrapper functions instead
    for function_name in [
        "TCOD_color_equals",
        "TCOD_color_add",
        "TCOD_color_subtract",
        "TCOD_color_multiply",
        "TCOD_color_multiply_scalar",
        "TCOD_color_lerp",
        "TCOD_color_get_HSV",
        "TCOD_color_get_hue",
        "TCOD_color_get_saturation",
        "TCOD_color_get_value",
        "TCOD_console_get_default_background",
        "TCOD_console_get_default_foreground",
        "TCOD_console_set_default_background",
        "TCOD_console_set_default_foreground",
        "TCOD_console_get_char_foreground",
        "TCOD_console_get_char_background",
        "TCOD_console_set_char_background",
        "TCOD_console_set_char_foreground",
        "TCOD_console_put_char_ex",
        "TCOD_console_set_fade",
        "TCOD_console_get_fading_color",
        "TCOD_console_set_color_control",
        "TCOD_image_clear",
        "TCOD_image_get_pixel",
        "TCOD_image_get_mipmap_pixel",
        "TCOD_image_put_pixel",
        "TCOD_image_set_key_color",
        "TCOD_parser_get_color_property",
        "TCOD_console_set_key_color",
    ]:
        wrapper_func = getattr(_lib, function_name +"_wrapper", None)
        if wrapper_func is not None:
            setattr(_lib, function_name, wrapper_func)
        else:
            raise Exception("unable to find wrapper", function_name)

HEXVERSION = 0x010604
STRVERSION = "1.6.4"
TECHVERSION = 0x01060400

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

_lib.TCOD_color_equals.restype=c_bool
_lib.TCOD_color_equals.argtypes=[Color, Color]

_lib.TCOD_color_add.restype=Color
_lib.TCOD_color_add.argtypes=[Color, Color]

_lib.TCOD_color_subtract.restype=Color
_lib.TCOD_color_subtract.argtypes=[Color, Color]

_lib.TCOD_color_multiply.restype=Color
_lib.TCOD_color_multiply.argtypes=[Color , Color ]

_lib.TCOD_color_multiply_scalar.restype=Color
_lib.TCOD_color_multiply_scalar.argtypes=[Color , c_float ]

# Should be valid on any platform, check it!  Has to be done after Color is defined.
# NOTE(rmtew): This should ideally be deleted.  Most of it is moved or duplicated here.
if MAC:
    from .cprotos import setup_protos
    setup_protos(_lib)


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

_lib.TCOD_color_set_HSV.restype=c_void
_lib.TCOD_color_set_HSV.argtypes=[POINTER(Color),c_float , c_float , c_float ]
def color_set_hsv(c, h, s, v):
    _lib.TCOD_color_set_HSV(byref(c), c_float(h), c_float(s), c_float(v))

_lib.TCOD_color_get_HSV.restype=c_void
_lib.TCOD_color_get_HSV.argtypes=[Color ,POINTER(c_float) , POINTER(c_float) , POINTER(c_float) ]
def color_get_hsv(c):
    h = c_float()
    s = c_float()
    v = c_float()
    _lib.TCOD_color_get_HSV(c, byref(h), byref(s), byref(v))
    return h.value, s.value, v.value

_lib.TCOD_color_scale_HSV.restype=c_void
_lib.TCOD_color_scale_HSV.argtypes=[POINTER(Color), c_float , c_float ]
def color_scale_HSV(c, scoef, vcoef) :
    _lib.TCOD_color_scale_HSV(byref(c),c_float(scoef),c_float(vcoef))

_lib.TCOD_color_gen_map.restype=c_void
_lib.TCOD_color_gen_map.argtypes=[POINTER(Color), c_int, POINTER(Color), POINTER(c_int)]
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
              ('text',c_char * 32),
              ('pressed', c_bool),
              ('lalt', c_bool),
              ('lctrl', c_bool),
              ('lmeta', c_bool),
              ('ralt', c_bool),
              ('rctrl', c_bool),
              ('rmeta', c_bool),
              ('shift', c_bool)
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
        self.fore_r[i] = int(r)
        self.fore_g[i] = int(g)
        self.fore_b[i] = int(b)
        self.char[i] = ord(char)

    def set_back(self, x, y, r, g, b):
        # set the background color of one cell.
        i = self.width * y + x
        self.back_r[i] = int(r)
        self.back_g[i] = int(g)
        self.back_b[i] = int(b)

    def set(self, x, y, back_r, back_g, back_b, fore_r, fore_g, fore_b, char):
        # set the background color, foreground color and character of one cell.
        i = self.width * y + x
        self.back_r[i] = int(back_r)
        self.back_g[i] = int(back_g)
        self.back_b[i] = int(back_b)
        self.fore_r[i] = int(fore_r)
        self.fore_g[i] = int(fore_g)
        self.fore_b[i] = int(fore_b)
        self.char[i] = ord(char)

    def blit(self, dest, fill_fore=True, fill_back=True):
        # use libtcod's "fill" functions to write the buffer to a console.
        if (console_get_width(dest) != self.width or
            console_get_height(dest) != self.height):
            raise ValueError('ConsoleBuffer.blit: Destination console has an incorrect size.')

        s = struct.Struct('%di' % len(self.back_r))

        if fill_back:
            _lib.TCOD_console_fill_background(c_void_p(dest), (c_int * len(self.back_r))(*self.back_r), (c_int * len(self.back_g))(*self.back_g), (c_int * len(self.back_b))(*self.back_b))

        if fill_fore:
            _lib.TCOD_console_fill_foreground(c_void_p(dest), (c_int * len(self.fore_r))(*self.fore_r), (c_int * len(self.fore_g))(*self.fore_g), (c_int * len(self.fore_b))(*self.fore_b))
            _lib.TCOD_console_fill_char(c_void_p(dest), (c_int * len(self.char))(*self.char))

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
KEY_TEXT = 66
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

_lib.TCOD_console_init_root.restype=c_void
_lib.TCOD_console_init_root.argtypes=[c_int, c_int, c_char_p , c_bool , c_uint ]
def console_init_root(w, h, title, fullscreen=False, renderer=RENDERER_SDL):
    _lib.TCOD_console_init_root(w, h, convert_to_ascii(title), fullscreen, renderer)

_lib.TCOD_console_set_custom_font.restype=c_void
_lib.TCOD_console_set_custom_font.argtypes=[c_char_p, c_int,c_int, c_int]
def console_set_custom_font(fontFile, flags=FONT_LAYOUT_ASCII_INCOL, nb_char_horiz=0, nb_char_vertic=0):
    _lib.TCOD_console_set_custom_font(convert_to_ascii(fontFile), flags, nb_char_horiz, nb_char_vertic)

_lib.TCOD_console_map_ascii_code_to_font.restype=c_void
_lib.TCOD_console_map_ascii_code_to_font.argtypes=[c_int, c_int, c_int]
def console_map_ascii_code_to_font(asciiCode, fontCharX, fontCharY):
    asciiCode = convert_to_ascii(asciiCode)
    if type(asciiCode) is bytes:
        _lib.TCOD_console_map_ascii_code_to_font(ord(asciiCode), fontCharX, fontCharY)
    else:
        _lib.TCOD_console_map_ascii_code_to_font(asciiCode, fontCharX, fontCharY)


_lib.TCOD_console_map_ascii_codes_to_font.restype=c_void
_lib.TCOD_console_map_ascii_codes_to_font.argtypes=[c_int, c_int, c_int, c_int]
def console_map_ascii_codes_to_font(firstAsciiCode, nbCodes, fontCharX,
                                    fontCharY):
    if type(firstAsciiCode) == str or type(firstAsciiCode) == bytes:
        _lib.TCOD_console_map_ascii_codes_to_font(ord(firstAsciiCode), nbCodes, fontCharX, fontCharY)
    else:
        _lib.TCOD_console_map_ascii_codes_to_font(firstAsciiCode, nbCodes, fontCharX, fontCharY)

_lib.TCOD_console_map_string_to_font.argtypes=[c_char_p, c_int, c_int]
_lib.TCOD_console_map_string_to_font_utf.argtypes=[c_wchar_p, c_int, c_int]
def console_map_string_to_font(s, fontCharX, fontCharY):
    # Python 3, utf is normal, so if they want utf behaviour call the other function.
    if type(s) is bytes or is_python_3:
        _lib.TCOD_console_map_string_to_font(convert_to_ascii(s), fontCharX, fontCharY)
    else:
        _lib.TCOD_console_map_string_to_font_utf(s, fontCharX, fontCharY)

def console_map_string_to_font_utf(s, fontCharX, fontCharY):
    _lib.TCOD_console_map_string_to_font_utf(s, fontCharX, fontCharY)

_lib.TCOD_console_is_fullscreen.restype=c_bool
_lib.TCOD_console_is_fullscreen.argtypes=[]
def console_is_fullscreen():
    return _lib.TCOD_console_is_fullscreen()

_lib.TCOD_console_set_fullscreen.restype=c_void
_lib.TCOD_console_set_fullscreen.argtypes=[c_bool ]
def console_set_fullscreen(fullscreen):
    _lib.TCOD_console_set_fullscreen(c_int(fullscreen))

_lib.TCOD_console_is_window_closed.restype=c_bool
_lib.TCOD_console_is_window_closed.argtypes=[]
def console_is_window_closed():
    return _lib.TCOD_console_is_window_closed()

_lib.TCOD_console_has_mouse_focus.restype=c_bool
_lib.TCOD_console_has_mouse_focus.argtypes=[]
def console_has_mouse_focus():
    return _lib.TCOD_console_has_mouse_focus()

_lib.TCOD_console_is_active.restype=c_bool
_lib.TCOD_console_is_active.argtypes=[]
def console_is_active():
    return _lib.TCOD_console_is_active()

_lib.TCOD_console_set_window_title.restype=c_void
_lib.TCOD_console_set_window_title.argtypes=[c_char_p]
def console_set_window_title(title):
    _lib.TCOD_console_set_window_title(convert_to_ascii(title))

_lib.TCOD_console_credits_render.restype = c_bool
def console_credits():
    _lib.TCOD_console_credits()

_lib.TCOD_console_credits_reset.restype=c_void
_lib.TCOD_console_credits_reset.argtypes=[]
def console_credits_reset():
    _lib.TCOD_console_credits_reset()

_lib.TCOD_console_credits_render.restype=c_bool
_lib.TCOD_console_credits_render.argtypes=[c_int, c_int, c_bool ]
def console_credits_render(x, y, alpha):
    return _lib.TCOD_console_credits_render(x, y, c_int(alpha))

_lib.TCOD_console_flush.restype=c_void
_lib.TCOD_console_flush.argtypes=[]
def console_flush():
    _lib.TCOD_console_flush()

# drawing on a console

_lib.TCOD_console_set_default_background.restype=c_void
_lib.TCOD_console_set_default_background.argtypes=[c_void_p ,Color ]
def console_set_default_background(con, col):
    _lib.TCOD_console_set_default_background(con, col)


_lib.TCOD_console_set_default_foreground.restype=c_void
_lib.TCOD_console_set_default_foreground.argtypes=[c_void_p ,Color ]
def console_set_default_foreground(con, col):
    _lib.TCOD_console_set_default_foreground(con, col)

_lib.TCOD_console_clear.restype=c_void
_lib.TCOD_console_clear.argtypes=[c_void_p ]
def console_clear(con):
    return _lib.TCOD_console_clear(con)

_lib.TCOD_console_put_char.restype=c_void
_lib.TCOD_console_put_char.argtypes=[c_void_p ,c_int, c_int, c_int, c_int]
def console_put_char(con, x, y, c, flag=BKGND_DEFAULT):
    if type(c) == str or type(c) == bytes:
        _lib.TCOD_console_put_char(c_void_p(con), x, y, ord(c), flag)
    else:
        _lib.TCOD_console_put_char(c_void_p(con), x, y, c, flag)

_lib.TCOD_console_put_char_ex.restype=c_void
_lib.TCOD_console_put_char_ex.argtypes=[c_void_p ,c_int, c_int, c_int, Color, Color]
def console_put_char_ex(con, x, y, c, fore, back):
    if type(c) == str or type(c) == bytes:
        _lib.TCOD_console_put_char_ex(c_void_p(con), x, y, ord(c), fore, back)
    else:
        _lib.TCOD_console_put_char_ex(c_void_p(con), x, y, c, fore, back)

_lib.TCOD_console_set_char_background.restype=c_void
_lib.TCOD_console_set_char_background.argtypes=[c_void_p ,c_int, c_int, Color , c_int ]
def console_set_char_background(con, x, y, col, flag=BKGND_SET):
    _lib.TCOD_console_set_char_background(con, x, y, col, flag)

_lib.TCOD_console_set_char_foreground.restype=c_void
_lib.TCOD_console_set_char_foreground.argtypes=[c_void_p ,c_int, c_int, Color ]
def console_set_char_foreground(con, x, y, col):
    _lib.TCOD_console_set_char_foreground(con, x, y, col)

_lib.TCOD_console_set_char.restype=c_void
_lib.TCOD_console_set_char.argtypes=[c_void_p ,c_int, c_int, c_int]
def console_set_char(con, x, y, c):
    if type(c) == str or type(c) == bytes:
        _lib.TCOD_console_set_char(con, x, y, ord(c))
    else:
        _lib.TCOD_console_set_char(con, x, y, c)

_lib.TCOD_console_set_background_flag.restype=c_void
_lib.TCOD_console_set_background_flag.argtypes=[c_void_p ,c_int ]
def console_set_background_flag(con, flag):
    _lib.TCOD_console_set_background_flag(con, flag)

_lib.TCOD_console_get_background_flag.restype=c_int
_lib.TCOD_console_get_background_flag.argtypes=[c_void_p ]
def console_get_background_flag(con):
    return _lib.TCOD_console_get_background_flag(con)

_lib.TCOD_console_set_alignment.restype=c_void
_lib.TCOD_console_set_alignment.argtypes=[c_void_p ,c_int ]
def console_set_alignment(con, alignment):
    _lib.TCOD_console_set_alignment(con, alignment)

_lib.TCOD_console_get_alignment.restype=c_int
_lib.TCOD_console_get_alignment.argtypes=[c_void_p ]
def console_get_alignment(con):
    return _lib.TCOD_console_get_alignment(con)

_lib.TCOD_console_print.argtypes=[c_void_p,c_int,c_int,c_char_p]
_lib.TCOD_console_print_utf.argtypes=[c_void_p,c_int,c_int,c_wchar_p]
def console_print(con, x, y, fmt):
    _lib.TCOD_console_print_utf(con, x, y, _fmt_unicode(fmt))

_lib.TCOD_console_print_ex.argtypes=[c_void_p,c_int,c_int,c_int,c_int,c_char_p]
_lib.TCOD_console_print_ex_utf.argtypes=[c_void_p, c_int, c_int, c_int, c_int, c_wchar_p]
def console_print_ex(con, x, y, flag, alignment, fmt):
    _lib.TCOD_console_print_ex_utf(con, x, y, flag, alignment,
                                   _fmt_unicode(fmt))

_lib.TCOD_console_print_rect.argtypes=[c_void_p, c_int, c_int, c_int, c_int, c_char_p]
_lib.TCOD_console_print_rect_utf.argtypes=[c_void_p, c_int, c_int, c_int, c_int, c_wchar_p]
def console_print_rect(con, x, y, w, h, fmt):
    return _lib.TCOD_console_print_rect_utf(con, x, y, w, h, _fmt_unicode(fmt))

_lib.TCOD_console_print_rect_ex.argtypes=[c_void_p, c_int, c_int, c_int, c_int, c_int, c_int, c_char_p]
_lib.TCOD_console_print_rect_ex_utf.argtypes=[c_void_p, c_int, c_int, c_int, c_int, c_int, c_int, c_wchar_p]
def console_print_rect_ex(con, x, y, w, h, flag, alignment, fmt):
    return _lib.TCOD_console_print_rect_ex_utf(con, x, y, w, h, flag, alignment, _fmt_unicode(fmt))

_lib.TCOD_console_get_height_rect.argtypes=[c_void_p, c_int, c_int, c_int, c_int, c_char_p]
_lib.TCOD_console_get_height_rect_utf.argtypes=[c_void_p, c_int, c_int, c_int, c_int, c_wchar_p]
def console_get_height_rect(con, x, y, w, h, fmt):
    return _lib.TCOD_console_get_height_rect_utf(con, x, y, w, h,
                                                 _fmt_unicode(fmt))

_lib.TCOD_console_rect.argtypes=[ c_void_p, c_int, c_int, c_int, c_int, c_bool, c_int ]
def console_rect(con, x, y, w, h, clr, flag=BKGND_DEFAULT):
    _lib.TCOD_console_rect(con, x, y, w, h, c_int(clr), flag)

_lib.TCOD_console_hline.argtypes=[ c_void_p, c_int, c_int, c_int, c_int ]
def console_hline(con, x, y, l, flag=BKGND_DEFAULT):
    _lib.TCOD_console_hline( con, x, y, l, flag)

_lib.TCOD_console_vline.argtypes=[ c_void_p, c_int, c_int, c_int, c_int ]
def console_vline(con, x, y, l, flag=BKGND_DEFAULT):
    _lib.TCOD_console_vline( con, x, y, l, flag)

_lib.TCOD_console_print_frame.argtypes=[c_void_p,c_int,c_int,c_int,c_int,c_int,c_int,c_char_p]
def console_print_frame(con, x, y, w, h, clear=True, flag=BKGND_DEFAULT, fmt=''):
    _lib.TCOD_console_print_frame(con, x, y, w, h, clear, flag,
                                  _fmt_bytes(fmt))

_lib.TCOD_console_get_foreground_color_image.restype=c_void_p
_lib.TCOD_console_get_foreground_color_image.argtypes=[c_void_p]
def console_get_foreground_image(con):
    return _lib.TCOD_console_get_foreground_color_image(con)

_lib.TCOD_console_get_background_color_image.restype=c_void_p
_lib.TCOD_console_get_background_color_image.argtypes=[c_void_p]
def console_get_background_image(con):
    return _lib.TCOD_console_get_background_color_image(con)

_lib.TCOD_console_set_color_control.restype=c_void
_lib.TCOD_console_set_color_control.argtypes=[c_void_p, Color, Color ]
def console_set_color_control(con,fore,back) :
    _lib.TCOD_console_set_color_control(con,fore,back)

_lib.TCOD_console_get_default_background.restype=Color
_lib.TCOD_console_get_default_background.argtypes=[c_void_p]
def console_get_default_background(con):
    return _lib.TCOD_console_get_default_background(con)

_lib.TCOD_console_get_default_foreground.restype=Color
_lib.TCOD_console_get_default_foreground.argtypes=[c_void_p]
def console_get_default_foreground(con):
    return _lib.TCOD_console_get_default_foreground(con)

_lib.TCOD_console_get_char_background.restype=Color
_lib.TCOD_console_get_char_background.argtypes=[c_void_p, c_int, c_int]
def console_get_char_background(con, x, y):
    return _lib.TCOD_console_get_char_background(con, x, y)

_lib.TCOD_console_get_char_foreground.restype=Color
_lib.TCOD_console_get_char_foreground.argtypes=[c_void_p, c_int, c_int]
def console_get_char_foreground(con, x, y):
    return _lib.TCOD_console_get_char_foreground(con, x, y)

_lib.TCOD_console_get_char.restype=c_int
_lib.TCOD_console_get_char.argtypes=[c_void_p, c_int, c_int]
def console_get_char(con, x, y):
    return _lib.TCOD_console_get_char(con, x, y)

_lib.TCOD_console_set_fade.restype=c_void
_lib.TCOD_console_set_fade.argtypes=[c_byte, Color]
def console_set_fade(fade, fadingColor):
    _lib.TCOD_console_set_fade(fade, fadingColor)

_lib.TCOD_console_get_fade.restype=c_byte
_lib.TCOD_console_get_fade.argtypes=[]
def console_get_fade():
    return _lib.TCOD_console_get_fade()

_lib.TCOD_console_get_fading_color.restype=Color
_lib.TCOD_console_get_fading_color.argtypes=[]
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

_lib.TCOD_console_is_key_pressed.restype=c_bool
_lib.TCOD_console_is_key_pressed.argtypes=[c_int ]
def console_is_key_pressed(key):
    return _lib.TCOD_console_is_key_pressed(key)

# using offscreen consoles
_lib.TCOD_console_new.restype=c_void_p
_lib.TCOD_console_new.argtypes=[c_int, c_int]
def console_new(w, h):
    return _lib.TCOD_console_new(w, h)

_lib.TCOD_console_from_file.restype=c_void_p
_lib.TCOD_console_from_file.argtypes=[c_char_p]
def console_from_file(filename):
    return _lib.TCOD_console_from_file(convert_to_ascii(filename))

_lib.TCOD_console_get_width.restype=c_int
_lib.TCOD_console_get_width.argtypes=[c_void_p ]
def console_get_width(con):
    return _lib.TCOD_console_get_width(con)

_lib.TCOD_console_get_height.restype=c_int
_lib.TCOD_console_get_height.argtypes=[c_void_p ]
def console_get_height(con):
    return _lib.TCOD_console_get_height(con)

_lib.TCOD_console_blit.argtypes=[c_void_p ,c_int, c_int, c_int, c_int, c_void_p , c_int, c_int, c_float, c_float]
def console_blit(src, x, y, w, h, dst, xdst, ydst, ffade=1.0,bfade=1.0):
    _lib.TCOD_console_blit(src, x, y, w, h, dst, xdst, ydst, c_float(ffade), c_float(bfade))

_lib.TCOD_console_set_key_color.argtypes=[c_void_p ,Color ]
def console_set_key_color(con, col):
    _lib.TCOD_console_set_key_color(c_void_p(con), col)

_lib.TCOD_console_set_dirty.restype=c_void
_lib.TCOD_console_set_dirty.argtypes=[c_int, c_int, c_int, c_int]
def console_set_dirty(x, y, w, h):
    return _lib.TCOD_console_set_dirty(x, y, w, h)

_lib.TCOD_console_delete.argtypes=[c_void_p ]
def console_delete(con):
    _lib.TCOD_console_delete(con)

# fast color filling
_lib.TCOD_console_fill_foreground.restype=c_void
_lib.TCOD_console_fill_foreground.argtypes=[c_void_p , POINTER(c_int), POINTER(c_int), POINTER(c_int)]
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

    _lib.TCOD_console_fill_foreground(c_void_p(con), cr, cg, cb)

_lib.TCOD_console_fill_background.restype=c_void
_lib.TCOD_console_fill_background.argtypes=[c_void_p , POINTER(c_int), POINTER(c_int), POINTER(c_int)]

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

    _lib.TCOD_console_fill_background(c_void_p(con), cr, cg, cb)


_lib.TCOD_console_fill_char.restype=c_void
_lib.TCOD_console_fill_char.argtypes=[c_void_p , POINTER(c_int)]
def console_fill_char(con,arr) :
    if (numpy_available and isinstance(arr, numpy.ndarray) ):
        #numpy arrays, use numpy's ctypes functions
        arr = numpy.ascontiguousarray(arr, dtype=numpy.int32)
        carr = arr.ctypes.data_as(POINTER(c_int))
    else:
        carr = (c_int * len(arr))(*arr)

    _lib.TCOD_console_fill_char(c_void_p(con), carr)

_lib.TCOD_console_load_asc.restype=c_bool
_lib.TCOD_console_load_asc.argtypes=[c_void_p , c_char_p]
def console_load_asc(con, filename) :
    return _lib.TCOD_console_load_asc(con,convert_to_ascii(filename))

_lib.TCOD_console_save_asc.restype=c_bool
_lib.TCOD_console_save_asc.argtypes=[c_void_p , c_char_p]
def console_save_asc(con, filename) :
    return _lib.TCOD_console_save_asc(con,convert_to_ascii(filename))

_lib.TCOD_console_load_apf.restype=c_bool
_lib.TCOD_console_load_apf.argtypes=[c_void_p , c_char_p]
def console_load_apf(con, filename) :
    return _lib.TCOD_console_load_apf(con,convert_to_ascii(filename))

_lib.TCOD_console_save_apf.restype=c_bool
_lib.TCOD_console_save_apf.argtypes=[c_void_p , c_char_p]
def console_save_apf(con, filename) :
    return _lib.TCOD_console_save_apf(con,convert_to_ascii(filename))

_lib.TCOD_console_from_xp.restype = c_void_p
_lib.TCOD_console_from_xp.argtypes = [c_char_p]
def console_from_xp(filename):
    return _lib.TCOD_console_from_xp(filename.encode('utf-8'))

_lib.TCOD_console_load_xp.restype = c_bool
_lib.TCOD_console_load_xp.argtypes = [c_void_p, c_char_p]
def console_load_xp(con, filename):
    return _lib.TCOD_console_load_xp(con, filename.encode('utf-8'))

_lib.TCOD_console_save_xp.restype = c_bool
_lib.TCOD_console_save_xp.argtypes = [c_void_p, c_char_p, c_int]
def console_save_xp(con, filename, compress_level=9):
    return _lib.TCOD_console_save_xp(con, filename.encode('utf-8'),
                                     compress_level)

_lib.TCOD_console_list_from_xp.restype = c_void_p
_lib.TCOD_console_list_from_xp.argtypes = [c_char_p]
def console_list_load_xp(filename):
    tcod_list = _lib.TCOD_console_list_from_xp(filename.encode('utf-8'))
    if not tcod_list:
        return None
    try:
        python_list = []
        _lib.TCOD_list_reverse(tcod_list)
        while not _lib.TCOD_list_is_empty(tcod_list):
            python_list.append(_lib.TCOD_list_pop(tcod_list))
        return python_list
    finally:
        _lib.TCOD_list_delete(tcod_list)

_lib.TCOD_console_list_save_xp.restype = c_bool
_lib.TCOD_console_list_save_xp.argtypes = [c_void_p, c_char_p, c_int]
def console_list_save_xp(console_list, filename, compress_level=9):
    tcod_list = _lib.TCOD_list_new()
    try:
        for console in console_list:
            _lib.TCOD_list_push(tcod_list, console)
        return _lib.TCOD_console_list_save_xp(
            tcod_list, filename.encode('utf-8'), compress_level
            )
    finally:
        _lib.TCOD_list_delete(tcod_list)


############################
# sys module
############################

_lib.TCOD_sys_startup.restype=c_void
_lib.TCOD_sys_startup.argtypes=[]
def sys_startup():
    _lib.TCOD_sys_startup()

_lib.TCOD_sys_shutdown.restype=c_void
_lib.TCOD_sys_shutdown.argtypes=[]
def sys_shutdown():
    _lib.TCOD_sys_shutdown()

_lib.TCOD_sys_get_last_frame_length.restype = c_float
_lib.TCOD_sys_elapsed_seconds.restype = c_float

# high precision time functions
_lib.TCOD_sys_set_fps.restype=c_void
_lib.TCOD_sys_set_fps.argtypes=[c_int]
def sys_set_fps(fps):
    _lib.TCOD_sys_set_fps(fps)

_lib.TCOD_sys_get_fps.restype=c_int
_lib.TCOD_sys_get_fps.argtypes=[]
def sys_get_fps():
    return _lib.TCOD_sys_get_fps()

_lib.TCOD_sys_get_last_frame_length.restype=c_float
_lib.TCOD_sys_get_last_frame_length.argtypes=[]
def sys_get_last_frame_length():
    return _lib.TCOD_sys_get_last_frame_length()

_lib.TCOD_sys_sleep_milli.restype=c_void
_lib.TCOD_sys_sleep_milli.argtypes=[c_uint ]
def sys_sleep_milli(val):
    _lib.TCOD_sys_sleep_milli(val)

_lib.TCOD_sys_elapsed_milli.restype=c_int
_lib.TCOD_sys_elapsed_milli.argtypes=[]
def sys_elapsed_milli():
    return _lib.TCOD_sys_elapsed_milli()

_lib.TCOD_sys_elapsed_seconds.restype=c_float
_lib.TCOD_sys_elapsed_seconds.argtypes=[]
def sys_elapsed_seconds():
    return _lib.TCOD_sys_elapsed_seconds()

_lib.TCOD_sys_set_renderer.restype=c_void
_lib.TCOD_sys_set_renderer.argtypes=[c_int ]
def sys_set_renderer(renderer):
    _lib.TCOD_sys_set_renderer(renderer)

_lib.TCOD_sys_get_renderer.restype=c_int
_lib.TCOD_sys_get_renderer.argtypes=[]
def sys_get_renderer():
    return _lib.TCOD_sys_get_renderer()

# easy screenshots

_lib.TCOD_sys_save_screenshot.restype=c_void
_lib.TCOD_sys_save_screenshot.argtypes=[c_char_p]
def sys_save_screenshot(name=0):
    _lib.TCOD_sys_save_screenshot(convert_to_ascii(name))

# clipboard support
# This maps to the SDL2 API, so only uses utf-8 for both Python 2 and 3.

_lib.TCOD_sys_clipboard_set.restype=c_bool
_lib.TCOD_sys_clipboard_set.argtypes=[c_char_p]
def sys_clipboard_set(text):
    return _lib.TCOD_sys_clipboard_set(text.encode("utf-8"))

_lib.TCOD_sys_clipboard_get.restype=c_char_p
_lib.TCOD_sys_clipboard_get.argtypes=[]
def sys_clipboard_get():
    return _lib.TCOD_sys_clipboard_get().decode("utf-8")

# custom fullscreen resolution

_lib.TCOD_sys_force_fullscreen_resolution.restype=c_void
_lib.TCOD_sys_force_fullscreen_resolution.argtypes=[c_int, c_int]
def sys_force_fullscreen_resolution(width, height):
    _lib.TCOD_sys_force_fullscreen_resolution(width, height)

_lib.TCOD_sys_get_current_resolution.restype=c_void
_lib.TCOD_sys_get_current_resolution.argtypes=[POINTER(c_int), POINTER(c_int)]
def sys_get_current_resolution():
    w = c_int()
    h = c_int()
    _lib.TCOD_sys_get_current_resolution(byref(w), byref(h))
    return w.value, h.value

_lib.TCOD_sys_get_fullscreen_offsets.restype=c_void
_lib.TCOD_sys_get_fullscreen_offsets.argtypes=[POINTER(c_int), POINTER(c_int)]

_lib.TCOD_sys_get_char_size.restype=c_void
_lib.TCOD_sys_get_char_size.argtypes=[POINTER(c_int), POINTER(c_int)]
def sys_get_char_size():
    w = c_int()
    h = c_int()
    _lib.TCOD_sys_get_char_size(byref(w), byref(h))
    return w.value, h.value

# update font bitmap
_lib.TCOD_sys_update_char.restype=c_void
_lib.TCOD_sys_update_char.argtypes=[c_int, c_int, c_int, c_void_p , c_int, c_int]
def sys_update_char(asciiCode, fontx, fonty, img, x, y) :
    _lib.TCOD_sys_update_char(asciiCode,fontx,fonty,img,x,y)

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

_lib.TCOD_sys_check_for_event.restype=c_int
_lib.TCOD_sys_check_for_event.argtypes=[c_int, c_void_p, c_void_p]
def sys_check_for_event(mask,k,m) :
    return _lib.TCOD_sys_check_for_event(mask,byref(k),byref(m))

_lib.TCOD_sys_wait_for_event.restype=c_int
_lib.TCOD_sys_wait_for_event.argtypes=[c_int, c_void_p, c_void_p, c_bool ]
def sys_wait_for_event(mask,k,m,flush) :
    return _lib.TCOD_sys_wait_for_event(mask,byref(k),byref(m),flush)

############################
# line module
############################

def line_init(xo, yo, xd, yd):
    _lib.TCOD_line_init(xo, yo, xd, yd)

_lib.TCOD_line_step.restype = c_bool
_lib.TCOD_line_step.argtypes=[POINTER(c_int), POINTER(c_int)]
def line_step():
    x = c_int()
    y = c_int()
    ret = _lib.TCOD_line_step(byref(x), byref(y))
    if not ret:
        return x.value, y.value
    return None,None

_lib.TCOD_line.restype=c_bool
def line(xo,yo,xd,yd,py_callback) :
    LINE_CBK_FUNC=CFUNCTYPE(c_bool,c_int,c_int)
    c_callback=LINE_CBK_FUNC(py_callback)
    return _lib.TCOD_line(xo,yo,xd,yd,c_callback)

_lib.TCOD_line_init_mt.restype=c_void
_lib.TCOD_line_init_mt.argtypes=[c_int, c_int, c_int, c_int, c_void_p]
_lib.TCOD_line_step_mt.restype = c_bool
_lib.TCOD_line_step_mt.argtypes=[POINTER(c_int), POINTER(c_int), c_void_p]
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

_lib.TCOD_image_new.restype=c_void_p
_lib.TCOD_image_new.argtypes=[c_int, c_int]
def image_new(width, height):
    return _lib.TCOD_image_new(width, height)

_lib.TCOD_image_clear.restype=c_void
_lib.TCOD_image_clear.argtypes=[c_void_p , Color ]
def image_clear(image,col) :
    _lib.TCOD_image_clear(image,col)

_lib.TCOD_image_invert.restype=c_void
_lib.TCOD_image_invert.argtypes=[c_void_p ]
def image_invert(image) :
    _lib.TCOD_image_invert(image)

_lib.TCOD_image_hflip.restype=c_void
_lib.TCOD_image_hflip.argtypes=[c_void_p ]
def image_hflip(image) :
    _lib.TCOD_image_hflip(image)

_lib.TCOD_image_rotate90.restype=c_void
_lib.TCOD_image_rotate90.argtypes=[c_void_p , c_int]
def image_rotate90(image, num=1) :
    _lib.TCOD_image_rotate90(image,num)

_lib.TCOD_image_vflip.restype=c_void
_lib.TCOD_image_vflip.argtypes=[c_void_p ]
def image_vflip(image) :
    _lib.TCOD_image_vflip(image)

_lib.TCOD_image_scale.restype=c_void
_lib.TCOD_image_scale.argtypes=[c_void_p , c_int, c_int]
def image_scale(image, neww, newh) :
    _lib.TCOD_image_scale(image,neww,newh)

_lib.TCOD_image_set_key_color.restype=c_void
_lib.TCOD_image_set_key_color.argtypes=[c_void_p , Color]
def image_set_key_color(image,col) :
    _lib.TCOD_image_set_key_color(image,col)

_lib.TCOD_image_get_alpha.restype=c_int
_lib.TCOD_image_get_alpha.argtypes=[c_void_p ,c_int, c_int]
def image_get_alpha(image,x,y) :
    return _lib.TCOD_image_get_alpha(image,c_int(x),c_int(y))

_lib.TCOD_image_is_pixel_transparent.restype = c_bool
_lib.TCOD_image_is_pixel_transparent.argtypes=[c_void_p , c_int, c_int]
def image_is_pixel_transparent(image,x,y) :
    return _lib.TCOD_image_is_pixel_transparent(image,c_int(x),c_int(y))

_lib.TCOD_image_load.restype=c_void_p
_lib.TCOD_image_load.argtypes=[c_char_p]
def image_load(filename):
    return _lib.TCOD_image_load(convert_to_ascii(filename))

_lib.TCOD_image_from_console.restype=c_void_p
_lib.TCOD_image_from_console.argtypes=[c_void_p ]
def image_from_console(console):
    return _lib.TCOD_image_from_console(console)

_lib.TCOD_image_refresh_console.restype=c_void
_lib.TCOD_image_refresh_console.argtypes=[c_void_p , c_void_p ]
def image_refresh_console(image, console):
    _lib.TCOD_image_refresh_console(image, console)

_lib.TCOD_image_get_size.restype=c_void
_lib.TCOD_image_get_size.argtypes=[c_void_p , POINTER(c_int),POINTER(c_int)]
def image_get_size(image):
    w=c_int()
    h=c_int()
    _lib.TCOD_image_get_size(image, byref(w), byref(h))
    return w.value, h.value

_lib.TCOD_image_get_pixel.restype = Color
_lib.TCOD_image_get_pixel.argtypes=[c_void_p ,c_int, c_int]
def image_get_pixel(image, x, y):
    return _lib.TCOD_image_get_pixel(image, x, y)

_lib.TCOD_image_get_mipmap_pixel.restype = Color
_lib.TCOD_image_get_mipmap_pixel.argtypes=[c_void_p ,c_float,c_float, c_float, c_float]
def image_get_mipmap_pixel(image, x0, y0, x1, y1):
    return _lib.TCOD_image_get_mipmap_pixel(image, c_float(x0), c_float(y0), c_float(x1), c_float(y1))

_lib.TCOD_image_put_pixel.restype=c_void
_lib.TCOD_image_put_pixel.argtypes=[ c_void_p ,c_int, c_int, Color ]
def image_put_pixel(image, x, y, col):
    _lib.TCOD_image_put_pixel(image, x, y, col)

_lib.TCOD_image_blit.restype=c_void
_lib.TCOD_image_blit.argtypes=[c_void_p, c_void_p, c_float, c_float, c_int, c_float, c_float, c_float]
def image_blit(image, console, x, y, bkgnd_flag, scalex, scaley, angle):
    _lib.TCOD_image_blit(image, console, x, y, bkgnd_flag, scalex, scaley, angle)

_lib.TCOD_image_blit_rect.restype=c_void
_lib.TCOD_image_blit_rect.argtypes=[c_void_p , c_void_p , c_int, c_int, c_int, c_int,]
def image_blit_rect(image, console, x, y, w, h, bkgnd_flag):
    _lib.TCOD_image_blit_rect(image, console, x, y, w, h, bkgnd_flag)

_lib.TCOD_image_blit_2x.restype=c_void
_lib.TCOD_image_blit_2x.argtypes=[c_void_p , c_void_p , c_int, c_int, c_int, c_int, c_int, c_int]
def image_blit_2x(image, console, dx, dy, sx=0, sy=0, w=-1, h=-1):
    _lib.TCOD_image_blit_2x(image, console, dx,dy,sx,sy,w,h)

_lib.TCOD_image_save.restype=c_void
_lib.TCOD_image_save.argtypes=[c_void_p, c_char_p]
def image_save(image, filename):
    _lib.TCOD_image_save(image, convert_to_ascii(filename))

_lib.TCOD_image_delete.restype=c_void
_lib.TCOD_image_delete.argtypes=[c_void_p]
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

_lib.TCOD_mouse_show_cursor.restype=c_void
_lib.TCOD_mouse_show_cursor.argtypes=[c_bool ]
def mouse_show_cursor(visible):
    _lib.TCOD_mouse_show_cursor(c_int(visible))

_lib.TCOD_mouse_is_cursor_visible.restype=c_bool
_lib.TCOD_mouse_is_cursor_visible.argtypes=[]
def mouse_is_cursor_visible():
    return _lib.TCOD_mouse_is_cursor_visible()

_lib.TCOD_mouse_move.restype=c_void
_lib.TCOD_mouse_move.argtypes=[c_int, c_int]
def mouse_move(x, y):
    _lib.TCOD_mouse_move(x, y)

_lib.TCOD_mouse_get_status_wrapper.restype=c_void
_lib.TCOD_mouse_get_status_wrapper.argtypes=[c_void_p]
def mouse_get_status():
    mouse=Mouse()
    _lib.TCOD_mouse_get_status_wrapper(byref(mouse))
    return mouse

############################
# parser module
############################

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

_lib.TCOD_list_get.restype = c_void_p
def _convert_TCODList(clist, typ):
    res = list()
    for i in range(_lib.TCOD_list_size(c_void_p(clist))):
        elt = _lib.TCOD_list_get(c_void_p(clist), i)
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

_lib.TCOD_parser_new.restype=c_void_p
_lib.TCOD_parser_new.argtypes=[]
def parser_new():
    return _lib.TCOD_parser_new()

_lib.TCOD_parser_new_struct.restype=c_void_p
_lib.TCOD_parser_new_struct.argtypes=[c_void_p , c_char_p]
def parser_new_struct(parser, name):
    return _lib.TCOD_parser_new_struct(parser, convert_to_ascii(name))

_lib.TCOD_struct_add_flag.restype=c_void
_lib.TCOD_struct_add_flag.argtypes=[c_void_p ,c_char_p]
def struct_add_flag(struct, name):
    _lib.TCOD_struct_add_flag(struct, convert_to_ascii(name))

_lib.TCOD_struct_add_property.restype=c_void
_lib.TCOD_struct_add_property.argtypes=[c_void_p , c_char_p,c_int , c_bool ]
def struct_add_property(struct, name, typ, mandatory):
    _lib.TCOD_struct_add_property(struct, convert_to_ascii(name), typ, mandatory)

_lib.TCOD_struct_add_value_list.restype=c_void
_lib.TCOD_struct_add_value_list.argtypes=[c_void_p ,c_char_p, POINTER(c_char_p), c_bool ]
def struct_add_value_list(struct, name, value_list, mandatory):
    CARRAY = c_char_p * (len(value_list) + 1)
    cvalue_list = CARRAY()
    for i in range(len(value_list)):
        cvalue_list[i] = cast(convert_to_ascii(value_list[i]), c_char_p)
    cvalue_list[len(value_list)] = 0
    _lib.TCOD_struct_add_value_list(struct, convert_to_ascii(name), cvalue_list, mandatory)

_lib.TCOD_struct_add_value_list_sized.restype=c_void
_lib.TCOD_struct_add_value_list_sized.argtypes=[c_void_p ,c_char_p, POINTER(c_char_p), c_int, c_bool ]

_lib.TCOD_struct_add_list_property.restype=c_void
_lib.TCOD_struct_add_list_property.argtypes=[c_void_p , c_char_p,c_int , c_bool ]
def struct_add_list_property(struct, name, typ, mandatory):
    _lib.TCOD_struct_add_list_property(struct, convert_to_ascii(name), typ, mandatory)

_lib.TCOD_struct_add_structure.restype=c_void
_lib.TCOD_struct_add_structure.argtypes=[c_void_p ,c_void_p]
def struct_add_structure(struct, sub_struct):
    _lib.TCOD_struct_add_structure(struct, sub_struct)

_lib.TCOD_struct_get_name.restype=c_char_p
_lib.TCOD_struct_get_name.argtypes=[c_void_p ]
def struct_get_name(struct):
    ret = _lib.TCOD_struct_get_name(struct)
    if is_python_3:
        return ret.decode("utf-8")
    return ret

_lib.TCOD_struct_is_mandatory.restype=c_bool
_lib.TCOD_struct_is_mandatory.argtypes=[c_void_p ,c_char_p]
def struct_is_mandatory(struct, name):
    return _lib.TCOD_struct_is_mandatory(struct, convert_to_ascii(name))

_lib.TCOD_struct_get_type.restype=c_int
_lib.TCOD_struct_get_type.argtypes=[c_void_p , c_char_p]
def struct_get_type(struct, name):
    return _lib.TCOD_struct_get_type(struct, convert_to_ascii(name))

_lib.TCOD_parser_run.restype=c_void
_lib.TCOD_parser_run.argtypes=[c_void_p , c_char_p, c_void_p]
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
        _lib.TCOD_parser_run(parser, convert_to_ascii(filename), byref(clistener))
    else:
        _lib.TCOD_parser_run(parser, convert_to_ascii(filename), 0)

_lib.TCOD_parser_delete.restype=c_void
_lib.TCOD_parser_delete.argtypes=[c_void_p ]
def parser_delete(parser):
    _lib.TCOD_parser_delete(parser)

_lib.TCOD_parser_has_property.restype = c_bool
_lib.TCOD_parser_has_property.argtypes=[c_void_p, c_char_p]
def parser_has_property(parser, name):
    return _lib.TCOD_parser_has_property(parser, convert_to_ascii(name))

_lib.TCOD_parser_get_bool_property.restype=c_bool
_lib.TCOD_parser_get_bool_property.argtypes=[c_void_p , c_char_p]
def parser_get_bool_property(parser, name):
    return _lib.TCOD_parser_get_bool_property(parser, convert_to_ascii(name))

_lib.TCOD_parser_get_int_property.restype=c_int
_lib.TCOD_parser_get_int_property.argtypes=[c_void_p , c_char_p]
def parser_get_int_property(parser, name):
    return _lib.TCOD_parser_get_int_property(parser, convert_to_ascii(name))

_lib.TCOD_parser_get_char_property.restype=c_int
_lib.TCOD_parser_get_char_property.argtypes=[c_void_p , c_char_p]
def parser_get_char_property(parser, name):
    return '%c' % _lib.TCOD_parser_get_char_property(parser, convert_to_ascii(name))

_lib.TCOD_parser_get_float_property.restype=c_float
_lib.TCOD_parser_get_float_property.argtypes=[c_void_p , c_char_p]
def parser_get_float_property(parser, name):
    return _lib.TCOD_parser_get_float_property(parser, convert_to_ascii(name))

_lib.TCOD_parser_get_string_property.restype=c_char_p
_lib.TCOD_parser_get_string_property.argtypes=[c_void_p , c_char_p]
def parser_get_string_property(parser, name):
    ret = _lib.TCOD_parser_get_string_property(parser, convert_to_ascii(name))
    if is_python_3:
        return ret.decode("utf-8")
    return ret

_lib.TCOD_parser_get_color_property.restype = Color
_lib.TCOD_parser_get_color_property.argtypes=[c_void_p , c_char_p]
def parser_get_color_property(parser, name):
    return _lib.TCOD_parser_get_color_property(parser, convert_to_ascii(name))

_lib.TCOD_parser_get_dice_property_py.argtypes=[c_void_p,c_char_p,POINTER(Dice)]
def parser_get_dice_property(parser, name):
    d = Dice()
    _lib.TCOD_parser_get_dice_property_py(parser, convert_to_ascii(name), byref(d))
    return d

_lib.TCOD_parser_get_list_property.restype=c_void_p
_lib.TCOD_parser_get_list_property.argtypes=[c_void_p , c_char_p, c_int ]
def parser_get_list_property(parser, name, typ):
    clist = _lib.TCOD_parser_get_list_property(parser, convert_to_ascii(name), typ)
    return _convert_TCODList(clist, typ)

_lib.TCOD_parser_get_custom_property.restype=c_void_p
_lib.TCOD_parser_get_custom_property.argtypes=[c_void_p , c_char_p]

############################
# random module
############################

RNG_MT = 0
RNG_CMWC = 1

DISTRIBUTION_LINEAR = 0
DISTRIBUTION_GAUSSIAN = 1
DISTRIBUTION_GAUSSIAN_RANGE = 2
DISTRIBUTION_GAUSSIAN_INVERSE = 3
DISTRIBUTION_GAUSSIAN_RANGE_INVERSE = 4

_lib.TCOD_random_get_instance.restype=c_void_p
_lib.TCOD_random_get_instance.argtypes=[]
def random_get_instance():
    return _lib.TCOD_random_get_instance()

_lib.TCOD_random_new.restype=c_void_p
_lib.TCOD_random_new.argtypes=[c_int ]
def random_new(algo=RNG_CMWC):
    return _lib.TCOD_random_new(algo)

_lib.TCOD_random_new_from_seed.restype=c_void_p
_lib.TCOD_random_new_from_seed.argtypes=[c_int, c_uint]
def random_new_from_seed(seed, algo=RNG_CMWC):
    return _lib.TCOD_random_new_from_seed(algo, seed)

_lib.TCOD_random_set_distribution.restype=c_void
_lib.TCOD_random_set_distribution.argtypes=[c_void_p , c_int ]
def random_set_distribution(rnd, dist) :
	_lib.TCOD_random_set_distribution(rnd, dist)

_lib.TCOD_random_get_int.restype=c_int
_lib.TCOD_random_get_int.argtypes=[c_void_p , c_int, c_int]
def random_get_int(rnd, mi, ma):
    return _lib.TCOD_random_get_int(rnd, mi, ma)

_lib.TCOD_random_get_float.restype=c_float
_lib.TCOD_random_get_float.argtypes=[c_void_p , c_float , c_float ]
def random_get_float(rnd, mi, ma):
    return _lib.TCOD_random_get_float(rnd, mi, ma)

_lib.TCOD_random_get_double.restype=c_double
_lib.TCOD_random_get_double.argtypes=[c_void_p , c_double , c_double ]
def random_get_double(rnd, mi, ma):
    return _lib.TCOD_random_get_double(rnd, mi, ma)

_lib.TCOD_random_get_int_mean.restype=c_int
_lib.TCOD_random_get_int_mean.argtypes=[c_void_p , c_int, c_int, c_int]
def random_get_int_mean(rnd, mi, ma, mean):
    return _lib.TCOD_random_get_int_mean(rnd, mi, ma, mean)

_lib.TCOD_random_get_float_mean.restype=c_float
_lib.TCOD_random_get_float_mean.argtypes=[c_void_p , c_float , c_float , c_float ]
def random_get_float_mean(rnd, mi, ma, mean):
    return _lib.TCOD_random_get_float_mean(rnd, mi, ma, mean)

_lib.TCOD_random_get_double_mean.restype=c_double
_lib.TCOD_random_get_double_mean.argtypes=[c_void_p , c_double , c_double , c_double ]
def random_get_double_mean(rnd, mi, ma, mean):
    return _lib.TCOD_random_get_double_mean(rnd, mi, ma, mean)

_lib.TCOD_random_dice_roll_s.restype=c_int
_lib.TCOD_random_dice_roll_s.argtypes=[c_void_p , c_char_p ]
def random_dice_roll_s(rnd, s):
    return _lib.TCOD_random_dice_roll_s(rnd, convert_to_ascii(s))

_lib.TCOD_random_save.restype=c_void_p
_lib.TCOD_random_save.argtypes=[c_void_p ]
def random_save(rnd):
    return _lib.TCOD_random_save(rnd)

_lib.TCOD_random_restore.restype=c_void
_lib.TCOD_random_restore.argtypes=[c_void_p , c_void_p ]
def random_restore(rnd, backup):
    _lib.TCOD_random_restore(rnd, backup)

_lib.TCOD_random_delete.restype=c_void
_lib.TCOD_random_delete.argtypes=[c_void_p ]
def random_delete(rnd):
    _lib.TCOD_random_delete(rnd)


############################
# noise module
############################

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

_lib.TCOD_noise_new.restype=c_void_p
_lib.TCOD_noise_new.argtypes=[c_int, c_float , c_float , c_void_p ]
def noise_new(dim, h=NOISE_DEFAULT_HURST, l=NOISE_DEFAULT_LACUNARITY, random=0):
    return _lib.TCOD_noise_new(dim, h, l, random)

_lib.TCOD_noise_set_type.restype=c_void
_lib.TCOD_noise_set_type.argtypes=[c_void_p , c_int ]
def noise_set_type(n, typ) :
    _lib.TCOD_noise_set_type(n,typ)

_lib.TCOD_noise_get.restype=c_float
_lib.TCOD_noise_get.argtypes=[c_void_p , POINTER(c_float)]

_lib.TCOD_noise_get_ex.restype=c_float
_lib.TCOD_noise_get_ex.argtypes=[c_void_p , POINTER(c_float), c_int ]
def noise_get(n, f, typ=NOISE_DEFAULT):
    return _lib.TCOD_noise_get_ex(n, _NOISE_PACKER_FUNC[len(f)](*f), typ)

_lib.TCOD_noise_get_fbm.restype=c_float
_lib.TCOD_noise_get_fbm.argtypes=[c_void_p , POINTER(c_float), c_float ]

_lib.TCOD_noise_get_fbm_ex.restype=c_float
_lib.TCOD_noise_get_fbm_ex.argtypes=[c_void_p , POINTER(c_float), c_float , c_int ]
def noise_get_fbm(n, f, oc, typ=NOISE_DEFAULT):
    return _lib.TCOD_noise_get_fbm_ex(n, _NOISE_PACKER_FUNC[len(f)](*f), oc, typ)

_lib.TCOD_noise_get_turbulence.restype=c_float
_lib.TCOD_noise_get_turbulence.argtypes=[c_void_p , POINTER(c_float), c_float ]

_lib.TCOD_noise_get_turbulence_ex.restype=c_float
_lib.TCOD_noise_get_turbulence_ex.argtypes=[c_void_p , POINTER(c_float), c_float , c_int ]
def noise_get_turbulence(n, f, oc, typ=NOISE_DEFAULT):
    return _lib.TCOD_noise_get_turbulence_ex(n, _NOISE_PACKER_FUNC[len(f)](*f), oc, typ)

_lib.TCOD_noise_delete.restype=c_void
_lib.TCOD_noise_delete.argtypes=[c_void_p ]
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

_lib.TCOD_map_new.restype=c_void_p
_lib.TCOD_map_new.argtypes=[c_int, c_int]
def map_new(w, h):
    return _lib.TCOD_map_new(w, h)

_lib.TCOD_map_copy.restype=c_void
_lib.TCOD_map_copy.argtypes=[c_void_p , c_void_p ]
def map_copy(source, dest):
    return _lib.TCOD_map_copy(source, dest)

_lib.TCOD_map_set_properties.restype=c_void
_lib.TCOD_map_set_properties.argtypes=[c_void_p , c_int, c_int, c_bool, c_bool]
def map_set_properties(m, x, y, isTrans, isWalk):
    _lib.TCOD_map_set_properties(m, x, y, c_int(isTrans), c_int(isWalk))

_lib.TCOD_map_clear.restype=c_void
_lib.TCOD_map_clear.argtypes=[c_void_p , c_bool , c_bool ]
def map_clear(m,walkable=False,transparent=False):
    _lib.TCOD_map_clear(m,c_int(walkable),c_int(transparent))

_lib.TCOD_map_compute_fov.restype=c_void
_lib.TCOD_map_compute_fov.argtypes=[c_void_p , c_int, c_int, c_int, c_bool, c_int ]
def map_compute_fov(m, x, y, radius=0, light_walls=True, algo=FOV_RESTRICTIVE ):
    _lib.TCOD_map_compute_fov(m, x, y, c_int(radius), c_bool(light_walls), c_int(algo))

_lib.TCOD_map_set_in_fov.restype=c_void
_lib.TCOD_map_set_in_fov.argtypes=[c_void_p , c_int, c_int, c_bool ]
def map_set_in_fov(m, x, y, fov):
    return _lib.TCOD_map_set_in_fov(m, x, y, fov)

_lib.TCOD_map_is_in_fov.restype=c_bool
_lib.TCOD_map_is_in_fov.argtypes=[c_void_p , c_int, c_int]
def map_is_in_fov(m, x, y):
    return _lib.TCOD_map_is_in_fov(m, x, y)

_lib.TCOD_map_is_transparent.restype=c_bool
_lib.TCOD_map_is_transparent.argtypes=[c_void_p , c_int, c_int]
def map_is_transparent(m, x, y):
    return _lib.TCOD_map_is_transparent(m, x, y)

_lib.TCOD_map_is_walkable.restype=c_bool
_lib.TCOD_map_is_walkable.argtypes=[c_void_p , c_int, c_int]
def map_is_walkable(m, x, y):
    return _lib.TCOD_map_is_walkable(m, x, y)

_lib.TCOD_map_delete.restype=c_void
_lib.TCOD_map_delete.argtypes=[c_void_p ]
def map_delete(m):
    return _lib.TCOD_map_delete(m)

_lib.TCOD_map_get_width.restype=c_int
_lib.TCOD_map_get_width.argtypes=[c_void_p ]
def map_get_width(map):
    return _lib.TCOD_map_get_width(map)

_lib.TCOD_map_get_height.restype=c_int
_lib.TCOD_map_get_height.argtypes=[c_void_p ]
def map_get_height(map):
    return _lib.TCOD_map_get_height(map)

_lib.TCOD_map_get_nb_cells.restype=c_int
_lib.TCOD_map_get_nb_cells.argtypes=[c_void_p ]
def map_get_nb_cells(map):
    return TCOD_map_get_nb_cells(map)

############################
# pathfinding module
############################

PATH_CBK_FUNC = CFUNCTYPE(c_float, c_int, c_int, c_int, c_int, py_object)

_lib.TCOD_path_new_using_map.restype=c_void_p
_lib.TCOD_path_new_using_map.argtypes=[c_void_p , c_float ]
def path_new_using_map(m, dcost=1.41):
    return (_lib.TCOD_path_new_using_map(m, dcost), None)

_lib.TCOD_path_new_using_function.restype=c_void_p
_lib.TCOD_path_new_using_function.argtypes=[c_int, c_int, PATH_CBK_FUNC,
                                            py_object, c_float]
def path_new_using_function(w, h, func, userdata=0, dcost=1.41):
    cbk_func = PATH_CBK_FUNC(func)
    return (_lib.TCOD_path_new_using_function(w, h, cbk_func,
                                              userdata, dcost), cbk_func)

_lib.TCOD_path_compute.restype = c_bool
_lib.TCOD_path_compute.argtypes=[c_void_p , c_int,c_int, c_int, c_int]
def path_compute(p, ox, oy, dx, dy):
    return _lib.TCOD_path_compute(p[0], ox, oy, dx, dy)

_lib.TCOD_path_get_origin.restype=c_void
_lib.TCOD_path_get_origin.argtypes=[c_void_p , POINTER(c_int), POINTER(c_int)]
def path_get_origin(p):
    x = c_int()
    y = c_int()
    _lib.TCOD_path_get_origin(p[0], byref(x), byref(y))
    return x.value, y.value

_lib.TCOD_path_get_destination.restype=c_void
_lib.TCOD_path_get_destination.argtypes=[c_void_p , POINTER(c_int), POINTER(c_int)]
def path_get_destination(p):
    x = c_int()
    y = c_int()
    _lib.TCOD_path_get_destination(p[0], byref(x), byref(y))
    return x.value, y.value

_lib.TCOD_path_size.restype=c_int
_lib.TCOD_path_size.argtypes=[c_void_p ]
def path_size(p):
    return _lib.TCOD_path_size(p[0])

_lib.TCOD_path_reverse.restype=c_void
_lib.TCOD_path_reverse.argtypes=[c_void_p ]
def path_reverse(p):
    _lib.TCOD_path_reverse(p[0])

_lib.TCOD_path_get.restype=c_void
_lib.TCOD_path_get.argtypes=[c_void_p , c_int, POINTER(c_int), POINTER(c_int)]
def path_get(p, idx):
    x = c_int()
    y = c_int()
    _lib.TCOD_path_get(p[0], idx, byref(x), byref(y))
    return x.value, y.value

_lib.TCOD_path_is_empty.restype = c_bool
_lib.TCOD_path_is_empty.argtypes=[c_void_p ]
def path_is_empty(p):
    return _lib.TCOD_path_is_empty(p[0])

_lib.TCOD_path_walk.restype = c_bool
_lib.TCOD_path_walk.argtypes=[c_void_p , POINTER(c_int), POINTER(c_int), c_bool]
def path_walk(p, recompute):
    x = c_int()
    y = c_int()
    if _lib.TCOD_path_walk(p[0], byref(x), byref(y), c_int(recompute)):
        return x.value, y.value
    return None,None

_lib.TCOD_path_delete.restype=c_void
_lib.TCOD_path_delete.argtypes=[c_void_p ]
def path_delete(p):
    _lib.TCOD_path_delete(p[0])



_lib.TCOD_dijkstra_new .restype=c_void_p
_lib.TCOD_dijkstra_new .argtypes=[c_void_p , c_float ]
def dijkstra_new(m, dcost=1.41):
    return (_lib.TCOD_dijkstra_new(c_void_p(m), c_float(dcost)), None)

_lib.TCOD_dijkstra_new_using_function.restype=c_void_p
_lib.TCOD_dijkstra_new_using_function.argtypes=[c_int, c_int, PATH_CBK_FUNC,
                                                py_object, c_float]
def dijkstra_new_using_function(w, h, func, userdata=0, dcost=1.41):
    cbk_func = PATH_CBK_FUNC(func)
    return (_lib.TCOD_dijkstra_new_using_function(w, h, cbk_func,
                                                  userdata, dcost), cbk_func)

_lib.TCOD_dijkstra_compute.restype=c_void
_lib.TCOD_dijkstra_compute.argtypes=[c_void_p , c_int, c_int]
def dijkstra_compute(p, ox, oy):
    _lib.TCOD_dijkstra_compute(p[0], c_int(ox), c_int(oy))

_lib.TCOD_dijkstra_path_set.restype = c_bool
_lib.TCOD_dijkstra_path_set .argtypes=[c_void_p , c_int, c_int]
def dijkstra_path_set(p, x, y):
    return _lib.TCOD_dijkstra_path_set(p[0], c_int(x), c_int(y))

_lib.TCOD_dijkstra_get_distance.restype = c_float
_lib.TCOD_dijkstra_get_distance.argtypes=[c_void_p , c_int, c_int]
def dijkstra_get_distance(p, x, y):
    return _lib.TCOD_dijkstra_get_distance(p[0], c_int(x), c_int(y))

_lib.TCOD_dijkstra_size.restype=c_int
_lib.TCOD_dijkstra_size.argtypes=[c_void_p ]
def dijkstra_size(p):
    return _lib.TCOD_dijkstra_size(p[0])

_lib.TCOD_dijkstra_reverse.restype=c_void
_lib.TCOD_dijkstra_reverse.argtypes=[c_void_p ]
def dijkstra_reverse(p):
    _lib.TCOD_dijkstra_reverse(p[0])

_lib.TCOD_dijkstra_get.restype=c_void
_lib.TCOD_dijkstra_get.argtypes=[c_void_p , c_int, POINTER(c_int), POINTER(c_int)]
def dijkstra_get(p, idx):
    x = c_int()
    y = c_int()
    _lib.TCOD_dijkstra_get(p[0], c_int(idx), byref(x), byref(y))
    return x.value, y.value

_lib.TCOD_dijkstra_is_empty.restype = c_bool
_lib.TCOD_dijkstra_is_empty.argtypes=[c_void_p ]
def dijkstra_is_empty(p):
    return _lib.TCOD_dijkstra_is_empty(p[0])

_lib.TCOD_dijkstra_path_walk.restype = c_bool
_lib.TCOD_dijkstra_path_walk.argtypes=[c_void_p , POINTER(c_int), POINTER(c_int)]
def dijkstra_path_walk(p):
    x = c_int()
    y = c_int()
    if _lib.TCOD_dijkstra_path_walk(p[0], byref(x), byref(y)):
        return x.value, y.value
    return None,None

_lib.TCOD_dijkstra_delete .restype=c_void
_lib.TCOD_dijkstra_delete.argtypes=[c_void_p ]
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

BSP_CBK_FUNC = CFUNCTYPE(c_int, c_void_p, c_void_p)

# Python class encapsulating the _CBsp pointer
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


_lib.TCOD_bsp_new.restype=c_void_p
_lib.TCOD_bsp_new.argtypes=[c_int, c_int, c_int, c_int]
def bsp_new(x, y, w, h):
    return _lib.TCOD_bsp_new(x, y, w, h)

_lib.TCOD_bsp_new_with_size.restype = POINTER(_CBsp)
_lib.TCOD_bsp_new_with_size.argtypes=[c_int,c_int,c_int, c_int]
def bsp_new_with_size(x, y, w, h):
    return Bsp(_lib.TCOD_bsp_new_with_size(x, y, w, h))

_lib.TCOD_bsp_split_once.restype=c_void
_lib.TCOD_bsp_split_once.argtypes=[c_void_p, c_bool , c_int]
def bsp_split_once(node, horizontal, position):
    _lib.TCOD_bsp_split_once(node.p, c_int(horizontal), position)

_lib.TCOD_bsp_split_recursive.restype=c_void
_lib.TCOD_bsp_split_recursive.argtypes=[c_void_p, c_void_p , c_int, ]
def bsp_split_recursive(node, randomizer, nb, minHSize, minVSize, maxHRatio,
                        maxVRatio):
    _lib.TCOD_bsp_split_recursive(node.p, randomizer, nb, minHSize, minVSize,
                                  c_float(maxHRatio), c_float(maxVRatio))

_lib.TCOD_bsp_resize.restype=c_void
_lib.TCOD_bsp_resize.argtypes=[c_void_p, c_int,c_int, c_int, c_int]
def bsp_resize(node, x, y, w, h):
    _lib.TCOD_bsp_resize(node.p, x, y, w, h)

_lib.TCOD_bsp_left.restype = POINTER(_CBsp)
_lib.TCOD_bsp_left.argtypes=[c_void_p]
def bsp_left(node):
    return Bsp(_lib.TCOD_bsp_left(node.p))

_lib.TCOD_bsp_right.restype = POINTER(_CBsp)
_lib.TCOD_bsp_right.argtypes=[c_void_p]
def bsp_right(node):
    return Bsp(_lib.TCOD_bsp_right(node.p))

_lib.TCOD_bsp_father.restype = POINTER(_CBsp)
_lib.TCOD_bsp_father.argtypes=[c_void_p]
def bsp_father(node):
    return Bsp(_lib.TCOD_bsp_father(node.p))

_lib.TCOD_bsp_is_leaf.restype = c_bool
_lib.TCOD_bsp_is_leaf.argtypes=[c_void_p]
def bsp_is_leaf(node):
    return _lib.TCOD_bsp_is_leaf(node.p)

_lib.TCOD_bsp_contains.restype = c_bool
_lib.TCOD_bsp_contains.argtypes=[c_void_p, c_int, c_int]
def bsp_contains(node, cx, cy):
    return _lib.TCOD_bsp_contains(node.p, cx, cy)

_lib.TCOD_bsp_find_node.restype = POINTER(_CBsp)
_lib.TCOD_bsp_find_node.argtypes=[c_void_p, c_int, c_int]
def bsp_find_node(node, cx, cy):
    return Bsp(_lib.TCOD_bsp_find_node(node.p, cx, cy))

def _bsp_traverse(node, callback, userData, func):
    # convert the c node into a Python node
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

_lib.TCOD_bsp_remove_sons.restype=c_void
_lib.TCOD_bsp_remove_sons.argtypes=[c_void_p]
def bsp_remove_sons(node):
    _lib.TCOD_bsp_remove_sons(node.p)

_lib.TCOD_bsp_delete.restype=c_void
_lib.TCOD_bsp_delete.argtypes=[c_void_p]
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

_lib.TCOD_heightmap_new.restype = POINTER(_CHeightMap)
_lib.TCOD_heightmap_new.argtypes=[c_int,c_int]
def heightmap_new(w, h):
    phm = _lib.TCOD_heightmap_new(w, h)
    return HeightMap(phm)

_lib.TCOD_heightmap_set_value.restype=c_void
_lib.TCOD_heightmap_set_value.argtypes=[c_void_p, c_int, c_int, c_float ]
def heightmap_set_value(hm, x, y, value):
    _lib.TCOD_heightmap_set_value(hm.p, x, y, c_float(value))

_lib.TCOD_heightmap_add.restype=c_void
_lib.TCOD_heightmap_add.argtypes=[c_void_p, c_float ]
def heightmap_add(hm, value):
    _lib.TCOD_heightmap_add(hm.p, c_float(value))

_lib.TCOD_heightmap_scale.restype=c_void
_lib.TCOD_heightmap_scale.argtypes=[c_void_p, c_float ]
def heightmap_scale(hm, value):
    _lib.TCOD_heightmap_scale(hm.p, c_float(value))

_lib.TCOD_heightmap_clear.restype=c_void
_lib.TCOD_heightmap_clear.argtypes=[c_void_p]
def heightmap_clear(hm):
    _lib.TCOD_heightmap_clear(hm.p)

_lib.TCOD_heightmap_clamp.restype=c_void
_lib.TCOD_heightmap_clamp.argtypes=[c_void_p, c_float , c_float ]
def heightmap_clamp(hm, mi, ma):
    _lib.TCOD_heightmap_clamp(hm.p, c_float(mi),c_float(ma))

_lib.TCOD_heightmap_copy.restype=c_void
_lib.TCOD_heightmap_copy.argtypes=[c_void_p,c_void_p]
def heightmap_copy(hm1, hm2):
    _lib.TCOD_heightmap_copy(hm1.p, hm2.p)

_lib.TCOD_heightmap_normalize.restype=c_void
_lib.TCOD_heightmap_normalize.argtypes=[c_void_p, c_float , c_float ]
def heightmap_normalize(hm,  mi=0.0, ma=1.0):
    _lib.TCOD_heightmap_normalize(hm.p, c_float(mi), c_float(ma))

_lib.TCOD_heightmap_lerp_hm.restype=c_void
_lib.TCOD_heightmap_lerp_hm.argtypes=[c_void_p, c_void_p, c_void_p, c_float ]
def heightmap_lerp_hm(hm1, hm2, hm3, coef):
    _lib.TCOD_heightmap_lerp_hm(hm1.p, hm2.p, hm3.p, c_float(coef))

_lib.TCOD_heightmap_add_hm.restype=c_void
_lib.TCOD_heightmap_add_hm.argtypes=[c_void_p, c_void_p, c_void_p]
def heightmap_add_hm(hm1, hm2, hm3):
    _lib.TCOD_heightmap_add_hm(hm1.p, hm2.p, hm3.p)

_lib.TCOD_heightmap_multiply_hm.restype=c_void
_lib.TCOD_heightmap_multiply_hm.argtypes=[c_void_p, c_void_p, c_void_p]
def heightmap_multiply_hm(hm1, hm2, hm3):
    _lib.TCOD_heightmap_multiply_hm(hm1.p, hm2.p, hm3.p)

_lib.TCOD_heightmap_add_hill.restype=c_void
_lib.TCOD_heightmap_add_hill.argtypes=[c_void_p, c_float , c_float , c_float , c_float ]
def heightmap_add_hill(hm, x, y, radius, height):
    _lib.TCOD_heightmap_add_hill(hm.p, c_float( x), c_float( y),
                                 c_float( radius), c_float( height))

_lib.TCOD_heightmap_dig_hill.restype=c_void
_lib.TCOD_heightmap_dig_hill.argtypes=[c_void_p, c_float , c_float , c_float , c_float ]
def heightmap_dig_hill(hm, x, y, radius, height):
    _lib.TCOD_heightmap_dig_hill(hm.p, c_float( x), c_float( y),
                                 c_float( radius), c_float( height))

_lib.TCOD_heightmap_mid_point_displacement.restype = c_void
_lib.TCOD_heightmap_mid_point_displacement.argtypes = [c_void_p, c_void_p,
                                                       c_float]
def heightmap_mid_point_displacement(hm, rng, roughness):
    _lib.TCOD_heightmap_mid_point_displacement(hm.p, rng, roughness)

_lib.TCOD_heightmap_rain_erosion.restype=c_void
_lib.TCOD_heightmap_rain_erosion.argtypes=[c_void_p, c_int,c_float ,c_float ,c_void_p ]
def heightmap_rain_erosion(hm, nbDrops, erosionCoef, sedimentationCoef, rnd=0):
    _lib.TCOD_heightmap_rain_erosion(hm.p, nbDrops, c_float( erosionCoef),
                                     c_float( sedimentationCoef), rnd)

_lib.TCOD_heightmap_kernel_transform.restype=c_void
_lib.TCOD_heightmap_kernel_transform.argtypes=[c_void_p, c_int, POINTER(c_int), POINTER(c_int), POINTER(c_float), c_float ,c_float ]
def heightmap_kernel_transform(hm, kernelsize, dx, dy, weight, minLevel,
                               maxLevel):
    FARRAY = c_float * kernelsize
    IARRAY = c_int * kernelsize
    cdx = IARRAY(*dx)
    cdy = IARRAY(*dy)
    cweight = FARRAY(*weight)
    _lib.TCOD_heightmap_kernel_transform(hm.p, kernelsize, cdx, cdy, cweight,
                                         c_float(minLevel), c_float(maxLevel))

_lib.TCOD_heightmap_add_voronoi.restype=c_void
_lib.TCOD_heightmap_add_voronoi.argtypes=[c_void_p, c_int, c_int, POINTER(c_float),c_void_p ]
def heightmap_add_voronoi(hm, nbPoints, nbCoef, coef, rnd=0):
    FARRAY = c_float * nbCoef
    ccoef = FARRAY(*coef)
    _lib.TCOD_heightmap_add_voronoi(hm.p, nbPoints, nbCoef, ccoef, rnd)

_lib.TCOD_heightmap_add_fbm.restype=c_void
_lib.TCOD_heightmap_add_fbm.argtypes=[c_void_p, c_void_p, c_float, c_float, c_float, c_float, c_float, c_float, c_float]
def heightmap_add_fbm(hm, noise, mulx, muly, addx, addy, octaves, delta, scale):
    _lib.TCOD_heightmap_add_fbm(hm.p, noise, c_float(mulx), c_float(muly),
                                c_float(addx), c_float(addy),
                                c_float(octaves), c_float(delta),
                                c_float(scale))

_lib.TCOD_heightmap_scale_fbm.restype=c_void
_lib.TCOD_heightmap_scale_fbm.argtypes=[c_void_p, c_void_p, c_float, c_float, c_float, c_float, c_float, c_float, c_float]
def heightmap_scale_fbm(hm, noise, mulx, muly, addx, addy, octaves, delta,
                        scale):
    _lib.TCOD_heightmap_scale_fbm(hm.p, noise, c_float(mulx), c_float(muly),
                                  c_float(addx), c_float(addy),
                                  c_float(octaves), c_float(delta),
                                  c_float(scale))

_lib.TCOD_heightmap_islandify.restype=c_void
_lib.TCOD_heightmap_islandify.argtypes=[c_void_p, c_float ,c_void_p ]
def heightmap_islandify(hm, sealevel, rnd):
    return TCOD_heightmap_islandify(hm, sealevel, rnd)

_lib.TCOD_heightmap_dig_bezier.restype=c_void
_lib.TCOD_heightmap_dig_bezier.argtypes=[c_void_p, POINTER(c_int), POINTER(c_int), c_float , c_float , c_float , c_float ]
def heightmap_dig_bezier(hm, px, py, startRadius, startDepth, endRadius,
                         endDepth):
    IARRAY = c_int * 4
    cpx = IARRAY(*px)
    cpy = IARRAY(*py)
    _lib.TCOD_heightmap_dig_bezier(hm.p, cpx, cpy, c_float(startRadius),
                                   c_float(startDepth), c_float(endRadius),
                                   c_float(endDepth))

_lib.TCOD_heightmap_get_value.restype = c_float
_lib.TCOD_heightmap_get_value.argtypes=[c_void_p, c_int, c_int]
def heightmap_get_value(hm, x, y):
    return _lib.TCOD_heightmap_get_value(hm.p, x, y)

_lib.TCOD_heightmap_get_interpolated_value.restype=c_float
_lib.TCOD_heightmap_get_interpolated_value.argtypes=[c_void_p, c_float , c_float ]
def heightmap_get_interpolated_value(hm, x, y):
    return _lib.TCOD_heightmap_get_interpolated_value(hm.p, c_float(x),
                                                     c_float(y))

_lib.TCOD_heightmap_get_slope.restype=c_float
_lib.TCOD_heightmap_get_slope.argtypes=[c_void_p, c_int, c_int]
def heightmap_get_slope(hm, x, y):
    return _lib.TCOD_heightmap_get_slope(hm.p, x, y)

_lib.TCOD_heightmap_get_normal.restype=c_void
_lib.TCOD_heightmap_get_normal.argtypes=[c_void_p, c_float , c_float , POINTER(c_float), c_float ]
def heightmap_get_normal(hm, x, y, waterLevel):
    FARRAY = c_float * 3
    cn = FARRAY()
    _lib.TCOD_heightmap_get_normal(hm.p, c_float(x), c_float(y), cn, c_float(waterLevel))
    return cn[0], cn[1], cn[2]

_lib.TCOD_heightmap_count_cells.restype=c_int
_lib.TCOD_heightmap_count_cells.argtypes=[c_void_p, c_float , c_float ]
def heightmap_count_cells(hm, mi, ma):
    return _lib.TCOD_heightmap_count_cells(hm.p, c_float(mi), c_float(ma))

_lib.TCOD_heightmap_has_land_on_border.restype = c_bool
_lib.TCOD_heightmap_has_land_on_border.argtypes=[c_void_p, c_float ]
def heightmap_has_land_on_border(hm, waterlevel):
    return _lib.TCOD_heightmap_has_land_on_border(hm.p, c_float(waterlevel))

_lib.TCOD_heightmap_get_minmax.restype=c_void
_lib.TCOD_heightmap_get_minmax.argtypes=[c_void_p, POINTER(c_float), POINTER(c_float)]
def heightmap_get_minmax(hm):
    mi = c_float()
    ma = c_float()
    _lib.TCOD_heightmap_get_minmax(hm.p, byref(mi), byref(ma))
    return mi.value, ma.value

_lib.TCOD_heightmap_delete.restype=c_void
_lib.TCOD_heightmap_delete.argtypes=[c_void_p]
def heightmap_delete(hm):
    _lib.TCOD_heightmap_delete(hm.p)


############################
# name generator module
############################

_lib.TCOD_namegen_parse.restype=c_void
_lib.TCOD_namegen_parse.argtypes=[c_char_p , c_void_p ]
def namegen_parse(filename,rnd=0) :
    _lib.TCOD_namegen_parse(convert_to_ascii(filename),rnd)

_lib.TCOD_namegen_generate.restype=c_char_p
_lib.TCOD_namegen_generate.argtypes=[c_char_p , c_bool ]
def namegen_generate(name) :
    ret = _lib.TCOD_namegen_generate(convert_to_ascii(name), 0)
    if is_python_3:
        return ret.decode("utf-8")
    return ret

_lib.TCOD_namegen_generate_custom.restype=c_char_p
_lib.TCOD_namegen_generate_custom.argtypes=[c_char_p , c_char_p , c_bool ]
def namegen_generate_custom(name, rule) :
    ret = _lib.TCOD_namegen_generate(convert_to_ascii(name), convert_to_ascii(rule), 0)
    if is_python_3:
        return ret.decode("utf-8")
    return ret

_lib.TCOD_namegen_get_sets.restype=c_void_p
_lib.TCOD_namegen_get_sets.argtypes=[]

def namegen_get_sets():
    nb=_lib.TCOD_namegen_get_nb_sets_wrapper()
    SARRAY = c_char_p * nb;
    setsa = SARRAY()
    _lib.TCOD_namegen_get_sets_wrapper(setsa)
    if is_python_3:
        return list(v.decode("utf-8") for v in setsa)
    return list(setsa)

_lib.TCOD_namegen_destroy.restype=c_void
_lib.TCOD_namegen_destroy.argtypes=[]
def namegen_destroy() :
    _lib.TCOD_namegen_destroy()


_lib.TCOD_lex_new_intern.restype=c_void_p
_lib.TCOD_lex_new_intern.argtypes=[]

_lib.TCOD_lex_new.restype=c_void_p
_lib.TCOD_lex_new.argtypes=[POINTER(c_char_p), POINTER(c_char_p), c_char_p, ]

_lib.TCOD_lex_delete.restype=c_void
_lib.TCOD_lex_delete.argtypes=[c_void_p]

_lib.TCOD_lex_set_data_buffer.restype=c_void
_lib.TCOD_lex_set_data_buffer.argtypes=[c_void_p,c_char_p]

_lib.TCOD_lex_set_data_file.restype=c_bool
_lib.TCOD_lex_set_data_file.argtypes=[c_void_p,c_char_p]

_lib.TCOD_lex_parse.restype=c_int
_lib.TCOD_lex_parse.argtypes=[c_void_p]

_lib.TCOD_lex_parse_until_token_type.restype=c_int
_lib.TCOD_lex_parse_until_token_type.argtypes=[c_void_p,c_int]

_lib.TCOD_lex_parse_until_token_value.restype=c_int
_lib.TCOD_lex_parse_until_token_value.argtypes=[c_void_p,c_char_p]

_lib.TCOD_lex_expect_token_type.restype=c_bool
_lib.TCOD_lex_expect_token_type.argtypes=[c_void_p,c_int]

_lib.TCOD_lex_expect_token_value.restype=c_bool
_lib.TCOD_lex_expect_token_value.argtypes=[c_void_p,c_int,c_char_p]

_lib.TCOD_lex_savepoint.restype=c_void
_lib.TCOD_lex_savepoint.argtypes=[c_void_p,c_void_p]

_lib.TCOD_lex_restore.restype=c_void
_lib.TCOD_lex_restore.argtypes=[c_void_p,c_void_p]

_lib.TCOD_lex_get_last_javadoc.restype=c_char_p
_lib.TCOD_lex_get_last_javadoc.argtypes=[c_void_p]

_lib.TCOD_lex_get_token_name.restype=c_char_p
_lib.TCOD_lex_get_token_name.argtypes=[c_int]

_lib.TCOD_lex_get_last_error.restype=c_char_p
_lib.TCOD_lex_get_last_error.argtypes=[]

_lib.TCOD_lex_hextoint.restype=c_int
_lib.TCOD_lex_hextoint.argtypes=[c_char]

_lib.TCOD_sys_get_surface.restype=c_void_p
_lib.TCOD_sys_get_surface.argtypes=[c_int, c_int, c_bool ]

_lib.TCOD_sys_load_image.restype=c_void_p
_lib.TCOD_sys_load_image.argtypes=[c_char_p]

_lib.TCOD_list_new.restype=c_void_p
_lib.TCOD_list_new.argtypes=[]

_lib.TCOD_list_allocate.restype=c_void_p
_lib.TCOD_list_allocate.argtypes=[c_int]

_lib.TCOD_list_duplicate.restype=c_void_p
_lib.TCOD_list_duplicate.argtypes=[c_void_p ]

_lib.TCOD_list_delete.restype=c_void
_lib.TCOD_list_delete.argtypes=[c_void_p ]

_lib.TCOD_list_push.restype=c_void
_lib.TCOD_list_push.argtypes=[c_void_p ,c_void_p ]

_lib.TCOD_list_pop.restype=c_void_p
_lib.TCOD_list_pop.argtypes=[c_void_p ]

_lib.TCOD_list_peek.restype=c_void_p
_lib.TCOD_list_peek.argtypes=[c_void_p ]

_lib.TCOD_list_add_all.restype=c_void
_lib.TCOD_list_add_all.argtypes=[c_void_p , c_void_p ]

_lib.TCOD_list_get.restype=c_void_p
_lib.TCOD_list_get.argtypes=[c_void_p ,c_int]

_lib.TCOD_list_set.restype=c_void
_lib.TCOD_list_set.argtypes=[c_void_p ,c_void_p, c_int]

_lib.TCOD_list_begin.restype=POINTER(c_void_p)
_lib.TCOD_list_begin.argtypes=[c_void_p ]

_lib.TCOD_list_end.restype=POINTER(c_void_p)
_lib.TCOD_list_end.argtypes=[c_void_p ]

_lib.TCOD_list_reverse.restype=c_void
_lib.TCOD_list_reverse.argtypes=[c_void_p ]

_lib.TCOD_list_remove_iterator.restype=POINTER(c_void_p)
_lib.TCOD_list_remove_iterator.argtypes=[c_void_p , POINTER(c_void_p)]

_lib.TCOD_list_remove.restype=c_void
_lib.TCOD_list_remove.argtypes=[c_void_p ,c_void_p ]

_lib.TCOD_list_remove_iterator_fast.restype=POINTER(c_void_p)
_lib.TCOD_list_remove_iterator_fast.argtypes=[c_void_p , POINTER(c_void_p)]

_lib.TCOD_list_remove_fast.restype=c_void
_lib.TCOD_list_remove_fast.argtypes=[c_void_p ,c_void_p ]

_lib.TCOD_list_contains.restype=c_bool
_lib.TCOD_list_contains.argtypes=[c_void_p ,c_void_p ]

_lib.TCOD_list_clear.restype=c_void
_lib.TCOD_list_clear.argtypes=[c_void_p ]

_lib.TCOD_list_clear_and_delete.restype=c_void
_lib.TCOD_list_clear_and_delete.argtypes=[c_void_p ]

_lib.TCOD_list_size.restype=c_int
_lib.TCOD_list_size.argtypes=[c_void_p ]

_lib.TCOD_list_insert_before.restype=POINTER(c_void_p)
_lib.TCOD_list_insert_before.argtypes=[c_void_p ,c_void_p,c_int]

_lib.TCOD_list_is_empty.restype=c_bool
_lib.TCOD_list_is_empty.argtypes=[c_void_p ]

_lib.TCOD_sys_create_directory.restype=c_bool
_lib.TCOD_sys_create_directory.argtypes=[c_char_p]

_lib.TCOD_sys_delete_file.restype=c_bool
_lib.TCOD_sys_delete_file.argtypes=[c_char_p]

_lib.TCOD_sys_delete_directory.restype=c_bool
_lib.TCOD_sys_delete_directory.argtypes=[c_char_p]

_lib.TCOD_sys_is_directory.restype=c_bool
_lib.TCOD_sys_is_directory.argtypes=[c_char_p]

_lib.TCOD_sys_get_directory_content.restype=c_void_p
_lib.TCOD_sys_get_directory_content.argtypes=[c_char_p, c_char_p]

_lib.TCOD_sys_file_exists.restype=c_bool
#    lib.TCOD_sys_file_exists.argtypes=[c_char_p , ...]

_lib.TCOD_sys_get_num_cores.restype=c_int
_lib.TCOD_sys_get_num_cores.argtypes=[]

_lib.TCOD_thread_wait.restype=c_void
_lib.TCOD_thread_wait.argtypes=[c_void_p ]

_lib.TCOD_mutex_new.restype=c_void_p
_lib.TCOD_mutex_new.argtypes=[]

_lib.TCOD_mutex_in.restype=c_void
_lib.TCOD_mutex_in.argtypes=[c_void_p ]

_lib.TCOD_mutex_out.restype=c_void
_lib.TCOD_mutex_out.argtypes=[c_void_p ]

_lib.TCOD_mutex_delete.restype=c_void
_lib.TCOD_mutex_delete.argtypes=[c_void_p ]

_lib.TCOD_semaphore_new.restype=c_void_p
_lib.TCOD_semaphore_new.argtypes=[c_int]

_lib.TCOD_semaphore_lock.restype=c_void
_lib.TCOD_semaphore_lock.argtypes=[c_void_p ]

_lib.TCOD_semaphore_unlock.restype=c_void
_lib.TCOD_semaphore_unlock.argtypes=[c_void_p ]

_lib.TCOD_semaphore_delete.restype=c_void
_lib.TCOD_semaphore_delete.argtypes=[ c_void_p ]

_lib.TCOD_condition_new.restype=c_void_p
_lib.TCOD_condition_new.argtypes=[]

_lib.TCOD_condition_signal.restype=c_void
_lib.TCOD_condition_signal.argtypes=[c_void_p ]

_lib.TCOD_condition_broadcast.restype=c_void
_lib.TCOD_condition_broadcast.argtypes=[c_void_p ]

_lib.TCOD_condition_wait.restype=c_void
_lib.TCOD_condition_wait.argtypes=[c_void_p , c_void_p ]

_lib.TCOD_condition_delete.restype=c_void
_lib.TCOD_condition_delete.argtypes=[ c_void_p ]

_lib.TCOD_tree_new.restype=c_void_p
_lib.TCOD_tree_new.argtypes=[]

_lib.TCOD_tree_add_son.restype=c_void
_lib.TCOD_tree_add_son.argtypes=[c_void_p, c_void_p]

_lib.TCOD_text_init.restype=c_void_p
_lib.TCOD_text_init.argtypes=[c_int, c_int, c_int, c_int, c_int]

_lib.TCOD_text_set_properties.restype=c_void
_lib.TCOD_text_set_properties.argtypes=[c_void_p , c_int, c_int, c_char_p , c_int]

_lib.TCOD_text_set_colors.restype=c_void
_lib.TCOD_text_set_colors.argtypes=[c_void_p , c_int , c_int , c_float]

_lib.TCOD_text_update.restype=c_bool
_lib.TCOD_text_update.argtypes=[c_void_p , c_int ]

_lib.TCOD_text_render.restype=c_void
_lib.TCOD_text_render.argtypes=[c_void_p , c_void_p ]

_lib.TCOD_text_get.restype=c_char_p
_lib.TCOD_text_get.argtypes=[c_void_p ]

_lib.TCOD_text_reset.restype=c_void
_lib.TCOD_text_reset.argtypes=[c_void_p ]

_lib.TCOD_text_delete.restype=c_void
_lib.TCOD_text_delete.argtypes=[c_void_p ]

_lib.TCOD_zip_new.restype=c_void_p
_lib.TCOD_zip_new.argtypes=[]

_lib.TCOD_zip_delete.restype=c_void
_lib.TCOD_zip_delete.argtypes=[c_void_p ]

_lib.TCOD_zip_put_char.restype=c_void
_lib.TCOD_zip_put_char.argtypes=[c_void_p , c_char ]

_lib.TCOD_zip_put_int.restype=c_void
_lib.TCOD_zip_put_int.argtypes=[c_void_p , c_int]

_lib.TCOD_zip_put_float.restype=c_void
_lib.TCOD_zip_put_float.argtypes=[c_void_p , c_float ]

_lib.TCOD_zip_put_string.restype=c_void
_lib.TCOD_zip_put_string.argtypes=[c_void_p , c_char_p]

_lib.TCOD_zip_put_color.restype=c_void
_lib.TCOD_zip_put_color.argtypes=[c_void_p , c_int ]

_lib.TCOD_zip_put_image.restype=c_void
_lib.TCOD_zip_put_image.argtypes=[c_void_p , c_void_p ]

_lib.TCOD_zip_put_console.restype=c_void
_lib.TCOD_zip_put_console.argtypes=[c_void_p , c_void_p ]

_lib.TCOD_zip_put_data.restype=c_void
_lib.TCOD_zip_put_data.argtypes=[c_void_p , c_int,c_void_p]

_lib.TCOD_zip_get_current_bytes.restype=c_int
_lib.TCOD_zip_get_current_bytes.argtypes=[c_void_p ]

_lib.TCOD_zip_save_to_file.restype=c_int
_lib.TCOD_zip_save_to_file.argtypes=[c_void_p , c_char_p]

_lib.TCOD_zip_load_from_file.restype=c_int
_lib.TCOD_zip_load_from_file.argtypes=[c_void_p , c_char_p]

_lib.TCOD_zip_get_char.restype=c_char
_lib.TCOD_zip_get_char.argtypes=[c_void_p ]

_lib.TCOD_zip_get_int.restype=c_int
_lib.TCOD_zip_get_int.argtypes=[c_void_p ]

_lib.TCOD_zip_get_float.restype=c_float
_lib.TCOD_zip_get_float.argtypes=[c_void_p ]

_lib.TCOD_zip_get_string.restype=c_char_p
_lib.TCOD_zip_get_string.argtypes=[c_void_p ]

_lib.TCOD_zip_get_color.restype=c_int
_lib.TCOD_zip_get_color.argtypes=[c_void_p ]

_lib.TCOD_zip_get_image.restype=c_void_p
_lib.TCOD_zip_get_image.argtypes=[c_void_p ]

_lib.TCOD_zip_get_console.restype=c_void_p
_lib.TCOD_zip_get_console.argtypes=[c_void_p ]

_lib.TCOD_zip_get_data.restype=c_int
_lib.TCOD_zip_get_data.argtypes=[c_void_p , c_int,c_void_p]

_lib.TCOD_zip_get_remaining_bytes.restype=c_int
_lib.TCOD_zip_get_remaining_bytes.argtypes=[c_void_p ]

_lib.TCOD_zip_skip_bytes.restype=c_void
_lib.TCOD_zip_skip_bytes.argtypes=[c_void_p ,c_int ]
