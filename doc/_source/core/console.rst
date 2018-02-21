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

Define a blit-transparent color
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Destroying an offscreen console
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

