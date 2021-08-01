Contexts
========

Context parameters
------------------

.. doxygenstruct:: TCOD_ContextParams
    :members:
    :undoc-members:


Context object
--------------

.. doxygenstruct:: TCOD_Context
    :members:

C++ API
-------

.. doxygenfunction:: new_context(const TCOD_ContextParams& params, TCOD_Error& out_code) -> ContextPtr
.. doxygenfunction:: new_context(const TCOD_ContextParams& params) -> ContextPtr

C API
-----

.. doxygenfunction:: TCOD_context_new

.. doxygenfunction:: TCOD_context_present

.. doxygenfunction:: TCOD_context_recommended_console_size
.. doxygenfunction:: TCOD_context_change_tileset

.. doxygenfunction:: TCOD_context_get_renderer_type

.. doxygenfunction:: TCOD_context_screen_pixel_to_tile_d
.. doxygenfunction:: TCOD_context_screen_pixel_to_tile_i
.. doxygenfunction:: TCOD_context_convert_event_coordinates

.. doxygenfunction:: TCOD_context_save_screenshot

.. doxygenfunction:: TCOD_context_get_sdl_window
.. doxygenfunction:: TCOD_context_get_sdl_renderer

