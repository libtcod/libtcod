*******
Console
*******

Initializing the console
========================

Creating the game window
^^^^^^^^^^^^^^^^^^^^^^^^

Using a custom bitmap font
^^^^^^^^^^^^^^^^^^^^^^^^^^

Using custom characters mapping
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Fullscreen mode
^^^^^^^^^^^^^^^

Communicate with the window manager
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

libtcod's Credits
^^^^^^^^^^^^^^^^^


Drawing on the root console
===========================

Basic printing functions
^^^^^^^^^^^^^^^^^^^^^^^^

Background effect flags
^^^^^^^^^^^^^^^^^^^^^^^

String drawing functions
^^^^^^^^^^^^^^^^^^^^^^^^

Advanced printing functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Reading the content of the console
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Screen fading functions
^^^^^^^^^^^^^^^^^^^^^^^

ASCII constants
^^^^^^^^^^^^^^^

Flushing the root console
=========================

ddddd

Handling user input
===================

Blocking user input
^^^^^^^^^^^^^^^^^^^

Non blocking user input
^^^^^^^^^^^^^^^^^^^^^^^

Keyboard event structure
^^^^^^^^^^^^^^^^^^^^^^^^

Mouse event structure
^^^^^^^^^^^^^^^^^^^^^

Key codes
^^^^^^^^^

Using off-screen consoles
=========================

Creating an offscreen console
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Creating an offscreen console from a .asc or .apf file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Loading an offscreen console from a .asc file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Loading an offscreen console from a .apf file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Saving a console to a .asc file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Saving a console to a .apf file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Working with REXPaint ``.xp`` files
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
REXPaint gives special treatment to tiles with a magic pink ``{255, 0, 255}``
background color.  You can processes this effect manually or by setting
:any:`TCOD_console_set_key_color` to :any:`TCOD_fuchsia`.

.. py:function:: libtcodpy.console_from_xp(filename)
.. c:function:: TCOD_console_t TCOD_console_from_xp(const char *filename)
    Return a new console from a REXPaint file.

    In C++, you can pass the filepath directly to the :any:`TCODConsole`
    constructor to load a REXPaint file.

.. py:function:: libtcodpy.console_load_xp(con, filename)
.. cpp:function:: bool TCODConsole::loadXp(const char *filename)
.. c:function:: bool TCOD_console_load_xp(TCOD_console_t con, const char *filename)
    Update the contents of a console from a REXPaint file.

    Returns true on success, or false on a failure such as when the consoles
    shape doesn't match the file being loaded.

.. py:function:: libtcodpy.console_save_xp(con, filename, compress_level=-1)
.. cpp:function:: bool TCODConsole::saveXp(const char *filename, int compress_level)
.. c:function:: bool TCOD_console_save_xp(TCOD_console_t con, const char *filename, int compress_level)
    Save a console as a REXPaint file.

    `compress_level` is a zlib compression level, from 0 to 9.
    Typical values are 0=uncompressed, 1=fast, 6=balanced, 9=slowest.

    Returns true on success, or false on a failure such as when the filepath
    provided can't be written to.

.. py:function:: libtcodpy.console_list_from_xp(filename)
.. c:function:: TCOD_list_t TCOD_console_list_from_xp(const char *filename)
    Return a list of new consoles from a REXPaint file.

    This can load any file made by :any:`TCOD_console_list_save_xp`.

.. py:function:: libtcodpy.console_list_save_xp(console_list, filename, compress_level)
.. c:function:: bool TCOD_console_list_save_xp(TCOD_list_t console_list, const char *filename, int compress_level)
    Save a list of consoles to a REXPaint file.

    The REXPaint tool can only work on files with up to 4 layers where all
    layers have the same size.

    This function can save any number of layers with multiple different sizes,
    these files can be loaded without issues as long you use
    :any:`TCOD_console_list_from_xp` for loading.

    Returns true if successful.

Blitting a console on another one
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Define a blit-transparent color
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Destroying an offscreen console
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

