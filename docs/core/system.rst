************
System layer
************

.. include:: ../old_docs_notice.inc.rst

Time functions
==============

Deprecated C functions
----------------------
.. note::
    These are deprecated because they are not compatible with libtcod contexts or are straight ports of SDL calls.
    You should use the `SDL timing API <https://wiki.libsdl.org/CategoryTimer>`_ or :any:`tcod::Timer` to track time.

.. doxygenfunction:: TCOD_sys_elapsed_milli
.. doxygenfunction:: TCOD_sys_elapsed_seconds
.. doxygenfunction:: TCOD_sys_sleep_milli
.. doxygenfunction:: TCOD_sys_set_fps
.. doxygenfunction:: TCOD_sys_get_fps
.. doxygenfunction:: TCOD_sys_get_last_frame_length

Timer class
-----------

.. doxygenclass:: tcod::Timer
    :members:

Easy screenshots
================

.. doxygenfunction:: TCOD_sys_save_screenshot

Miscellaneous utilities
=======================

.. doxygenfunction:: TCOD_sys_get_sdl_window
.. doxygenfunction:: TCOD_sys_get_sdl_renderer
.. doxygenfunction:: TCOD_sys_get_internal_context
.. doxygenfunction:: TCOD_sys_get_internal_console
