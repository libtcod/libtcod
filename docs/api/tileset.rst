Tilesets
========

.. doxygenstruct:: TCOD_Tileset

.. doxygenclass:: tcod::Tileset
  :members:

.. doxygentypedef:: TilesetPtr

Tilesheets
----------

.. doxygenvariable:: CHARMAP_CP437
.. doxygenvariable:: CHARMAP_TCOD

.. doxygenfunction:: load_tilesheet(const std::filesystem::path& path, const std::array<int, 2>& columns_rows, const ArrayType& charmap) -> TilesetPtr


BDF
---

.. doxygengroup:: BDF
    :content-only:

C API
-----

.. doxygenfunction:: TCOD_tileset_new
.. doxygenfunction:: TCOD_tileset_delete

.. doxygenvariable:: TCOD_CHARMAP_CP437
.. doxygenvariable:: TCOD_CHARMAP_TCOD

.. doxygenfunction:: TCOD_tileset_load
.. doxygenfunction:: TCOD_tileset_load_mem
.. doxygenfunction:: TCOD_tileset_load_raw

.. doxygenfunction:: TCOD_tileset_render_to_surface

.. doxygenfunction:: TCOD_get_default_tileset
.. doxygenfunction:: TCOD_set_default_tileset
