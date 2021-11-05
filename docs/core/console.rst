*******
Console
*******

.. include:: ../old_docs_notice.inc.rst

tcod::Console
=============

.. doxygenclass:: tcod::Console
    :members:
    :undoc-members:

TCOD_Console
============

.. doxygenstruct:: TCOD_Console
    :members:
    :undoc-members:

.. doxygentypedef:: ConsolePtr

.. doxygenstruct:: TCOD_ConsoleTile
    :members:

Initializing the console
========================

Creating the game window
^^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenenum:: TCOD_renderer_t
.. doxygenfunction:: TCOD_console_init_root
.. doxygenfunction:: TCOD_quit

Using a custom bitmap font
^^^^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenenum:: TCOD_font_flags_t
.. doxygenfunction:: TCOD_console_set_custom_font

Using custom characters mapping
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_map_ascii_code_to_font
.. doxygenfunction:: TCOD_console_map_ascii_codes_to_font
.. doxygenfunction:: TCOD_console_map_string_to_font

Fullscreen mode
^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_set_fullscreen
.. doxygenfunction:: TCOD_console_is_fullscreen

Communicate with the window manager
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_is_active
.. doxygenfunction:: TCOD_console_has_mouse_focus
.. doxygenfunction:: TCOD_console_is_window_closed
.. doxygenfunction:: TCOD_console_set_window_title

libtcod's Credits
^^^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_credits
.. doxygenfunction:: TCOD_console_credits_reset
.. doxygenfunction:: TCOD_console_credits_render

.. doxygenfunction:: TCOD_console_credits_render_ex

Drawing on the console
======================

Basic drawing functions
^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_set_default_foreground
.. doxygenfunction:: TCOD_console_set_default_background
.. doxygenfunction:: TCOD_console_set_background_flag

.. doxygenfunction:: TCOD_console_clear

.. doxygenfunction:: TCOD_console_put_char
.. doxygenfunction:: TCOD_console_put_char_ex

.. doxygenfunction:: TCOD_console_set_char
.. doxygenfunction:: TCOD_console_set_char_foreground
.. doxygenfunction:: TCOD_console_set_char_background

.. doxygenfunction:: TCOD_console_rect

.. doxygenfunction:: TCOD_console_draw_rect_rgb
.. doxygenfunction:: TCOD_console_draw_frame_rgb

.. doxygenfunction:: TCOD_console_hline
.. doxygenfunction:: TCOD_console_vline
.. doxygenfunction:: TCOD_console_print_frame


Drawing functions (C++)
^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: tcod::draw_rect(TCOD_Console& console, const std::array<int, 4>& rect, int ch, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg, TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET)
.. doxygenfunction:: tcod::draw_frame(TCOD_Console& console, const std::array<int, 4>& rect, const std::array<int, 9>& decoration, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg, TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET, bool clear = true)

Background effect flags
^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenenum:: TCOD_bkgnd_flag_t

String printing alignment
^^^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenenum:: TCOD_alignment_t
.. doxygenfunction:: TCOD_console_set_alignment
.. doxygenfunction:: TCOD_console_get_alignment

Reading the content of the console
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_get_width
.. doxygenfunction:: TCOD_console_get_height

.. doxygenfunction:: TCOD_console_get_char
.. doxygenfunction:: TCOD_console_get_char_foreground
.. doxygenfunction:: TCOD_console_get_char_background

.. doxygenfunction:: TCOD_console_get_default_foreground
.. doxygenfunction:: TCOD_console_get_default_background
.. doxygenfunction:: TCOD_console_get_background_flag

Screen fading functions
^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_set_fade
.. doxygenfunction:: TCOD_console_get_fade
.. doxygenfunction:: TCOD_console_get_fading_color

ASCII constants
^^^^^^^^^^^^^^^
.. doxygenenum:: TCOD_chars_t

Printing to the console
=======================

Printing functions using UTF-8
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_printf
.. doxygenfunction:: TCOD_console_printf_ex
.. doxygenfunction:: TCOD_console_printf_rect
.. doxygenfunction:: TCOD_console_printf_rect_ex
.. doxygenfunction:: TCOD_console_get_height_rect_fmt
.. doxygenfunction:: TCOD_console_printf_frame
.. doxygenfunction:: TCOD_console_printn
.. doxygenfunction:: TCOD_console_printn_rect
.. doxygenfunction:: TCOD_console_vprintf
.. doxygenfunction:: TCOD_console_vprintf_rect
.. doxygenfunction:: TCOD_console_get_height_rect_n
.. doxygenfunction:: TCOD_console_get_height_rect_wn

Printing functions using UTF-8 (C++)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: tcod::print(TCOD_Console& console, const std::array<int, 2>& xy, std::string_view str, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg, TCOD_alignment_t alignment = TCOD_LEFT, TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET)
.. doxygenfunction:: tcod::print(TCOD_Console& console, const std::array<int, 4>& rect, std::string_view str, std::optional<TCOD_ColorRGB> fg, std::optional<TCOD_ColorRGB> bg, TCOD_alignment_t alignment = TCOD_LEFT, TCOD_bkgnd_flag_t flag = TCOD_BKGND_SET)
.. doxygenfunction:: tcod::get_height_rect(int width, const std::string& str)

.. doxygenfunction:: tcod::stringf

Printing functions using 8-bit encodings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. note::
    These functions use EASCII encoded strings which are not compatible with Unicode.
    They are deprecated for this reason.

.. doxygenfunction:: TCOD_console_print
.. doxygenfunction:: TCOD_console_print_ex
.. doxygenfunction:: TCOD_console_print_rect
.. doxygenfunction:: TCOD_console_print_rect_ex
.. doxygenfunction:: TCOD_console_get_height_rect

Printing functions using wchar_t
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. note::
    These functions say they are UTF, however they will behave as UCS2 or UCS4 depending on the platform.
    They are deprecated for this reason.

.. doxygenfunction:: TCOD_console_print_utf
.. doxygenfunction:: TCOD_console_print_ex_utf
.. doxygenfunction:: TCOD_console_print_rect_utf
.. doxygenfunction:: TCOD_console_print_rect_ex_utf
.. doxygenfunction:: TCOD_console_get_height_rect_utf

Flushing the root console
=========================
.. doxygenfunction:: TCOD_console_flush
.. doxygenfunction:: TCOD_sys_accumulate_console

Handling user input
===================

Blocking user input
^^^^^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_wait_for_keypress
.. doxygenfunction:: TCOD_sys_wait_for_event

Non blocking user input
^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_check_for_keypress
.. doxygenfunction:: TCOD_console_is_key_pressed
.. doxygenfunction:: TCOD_sys_check_for_event
.. doxygenfunction:: TCOD_mouse_get_status

Keyboard event structure
^^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenenum:: TCOD_key_status_t
.. doxygenstruct:: TCOD_key_t

Key codes
^^^^^^^^^
.. doxygenenum:: TCOD_keycode_t

Mouse event structure
^^^^^^^^^^^^^^^^^^^^^
.. doxygenstruct:: TCOD_mouse_t

Events from SDL2
^^^^^^^^^^^^^^^^
.. doxygenfunction:: tcod::sdl2::process_event(const union SDL_Event &in, TCOD_key_t &out) noexcept
.. doxygenfunction:: tcod::sdl2::process_event(const union SDL_Event &in, TCOD_mouse_t &out) noexcept
.. doxygenfunction:: TCOD_sys_process_key_event
.. doxygenfunction:: TCOD_sys_process_mouse_event

Using off-screen consoles
=========================

Creating and deleting off-screen consoles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_new
.. doxygenfunction:: TCOD_console_delete

Creating an off-screen console from any .asc/.apf/.xp file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_from_file

Loading an offscreen console from a .asc file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_load_asc

Loading an offscreen console from a .apf file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_load_apf

Saving a console to a .asc file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_save_asc

Saving a console to a .apf file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_save_apf

Working with REXPaint ``.xp`` files
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
REXPaint gives special treatment to tiles with a magic pink ``{255, 0, 255}``
background color.  You can processes this effect manually or by setting
:any:`TCOD_console_set_key_color` to `TCOD_fuchsia`.

.. py:function:: libtcodpy.console_from_xp(filename)
.. doxygenfunction:: TCOD_console_from_xp

.. py:function:: libtcodpy.console_load_xp(con, filename)
.. cpp:function:: bool TCODConsole::loadXp(const char *filename)
.. doxygenfunction:: TCOD_console_load_xp

.. py:function:: libtcodpy.console_save_xp(con, filename, compress_level=-1)
.. cpp:function:: bool TCODConsole::saveXp(const char *filename, int compress_level)
.. doxygenfunction:: TCOD_console_save_xp

.. py:function:: libtcodpy.console_list_from_xp(filename)
.. doxygenfunction:: TCOD_console_list_from_xp

.. py:function:: libtcodpy.console_list_save_xp(console_list, filename, compress_level)
.. doxygenfunction:: TCOD_console_list_save_xp

Blitting a console on another one
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_blit

Define a blit-transparent color
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. doxygenfunction:: TCOD_console_set_key_color
