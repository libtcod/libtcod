
import random

import pytest

import libtcodpy

FONT_FILE = '../terminal.png'
WIDTH = 12
HEIGHT = 10
TITLE = 'libtcodpy tests'
FULLSCREEN = False
RENDERER = libtcodpy.RENDERER_SDL

@pytest.fixture(scope="module")
def session_console():
    libtcodpy.console_set_custom_font(FONT_FILE)
    console = libtcodpy.console_init_root(WIDTH, HEIGHT, TITLE, FULLSCREEN)

    assert libtcodpy.console_get_width(console) == WIDTH
    assert libtcodpy.console_get_height(console) == HEIGHT
    assert libtcodpy.console_is_fullscreen() == FULLSCREEN
    libtcodpy.console_set_window_title(TITLE)
    assert not libtcodpy.console_is_window_closed()

    libtcodpy.sys_get_current_resolution()
    libtcodpy.sys_get_char_size()
    libtcodpy.sys_set_renderer(RENDERER)
    libtcodpy.sys_get_renderer()

    yield console
    libtcodpy.console_delete(console)

@pytest.fixture(scope="function")
def console(session_console):
    """Return a root console.

    Be sure to use this fixture if the GUI needs to be initialized for a test.
    """
    console = session_console
    libtcodpy.console_flush()
    libtcodpy.console_set_default_foreground(console, libtcodpy.white)
    libtcodpy.console_set_default_background(console, libtcodpy.black)
    libtcodpy.console_set_background_flag(console, libtcodpy.BKGND_SET)
    libtcodpy.console_set_alignment(console, libtcodpy.LEFT)
    libtcodpy.console_clear(console)
    return console

@pytest.fixture()
def offscreen(console):
    """Return an off-screen console with the same size as the root console."""
    offscreen = libtcodpy.console_new(
        libtcodpy.console_get_width(console),
        libtcodpy.console_get_height(console),
        )
    yield offscreen
    libtcodpy.console_delete(offscreen)

@pytest.fixture()
def fg():
    return libtcodpy.Color(random.randint(0, 255),
                           random.randint(0, 255),
                           random.randint(0, 255))

@pytest.fixture()
def bg():
    return libtcodpy.Color(random.randint(0, 255),
                           random.randint(0, 255),
                           random.randint(0, 255))

try:
    unichr
except NameError:
    unichr = chr

def ch_ascii_int():
    return random.randint(0x21, 0x7f)

def ch_ascii_str():
    return chr(ch_ascii_int())

def ch_latin1_int():
    return random.randint(0x80, 0xff)

def ch_latin1_str():
    return chr(ch_latin1_int())

def ch_bmp_int():
    # Basic Multilingual Plane, before surrogates
    return random.randint(0x100, 0xd7ff)

def ch_bmp_str():
    return unichr(ch_bmp_int())

def ch_smp_int():
    return random.randint(0x10000, 0x1f9ff)

def ch_smp_str():
    return unichr(ch_bmp_int())

@pytest.fixture(params=['ascii_int', 'ascii_str',
                        'latin1_int', 'latin1_str',
                        #'bmp_int', 'bmp_str', # causes crashes
                        ])
def ch(request):
    """Test with multiple types of ascii/latin1 characters"""
    return globals()['ch_%s' % request.param]()
