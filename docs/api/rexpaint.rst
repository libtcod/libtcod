REXPaint Files
==============

These functions allow the saving and loading of
`REXPaint <https://www.gridsagegames.com/rexpaint/>`_ files.

Keep in mind that REXPaint files are typically encoded as
`Code Page 437 <https://en.wikipedia.org/wiki/Code_page_437>`_
which will need to be converted into Unicode to be understood by libtcod.

C++ API
-------

.. doxygenfunction:: tcod::load_xp

.. doxygenfunction:: tcod::save_xp

C API
-----

.. doxygenfunction:: TCOD_load_xp

.. doxygenfunction:: TCOD_save_xp

.. doxygenfunction:: TCOD_load_xp_from_memory

.. doxygenfunction:: TCOD_save_xp_to_memory


