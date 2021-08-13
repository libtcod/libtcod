Tilesets
========

.. doxygenstruct:: TCOD_Tileset
.. doxygentypedef:: TilesetPtr

Tilesheets
----------

.. doxygenvariable:: CHARMAP_CP437
.. doxygenvariable:: CHARMAP_TCOD

.. doxygenfunction:: load_tilesheet(const char *path, const std::array<int, 2> &columns_rows, const ArrayType &charmap) -> TilesetPtr
.. doxygenfunction:: load_tilesheet(const std::string& path, const std::array<int, 2> &columns_rows, const ArrayType &charmap) -> TilesetPtr


BDF
---

.. doxygenfunction:: load_bdf(const char *path) -> TilesetPtr
.. doxygenfunction:: load_bdf(const std::string &path) -> TilesetPtr


C API
-----

.. doxygenfunction:: TCOD_tileset_new
.. doxygenfunction:: TCOD_tileset_delete

.. doxygenvariable:: TCOD_CHARMAP_CP437
.. doxygenvariable:: TCOD_CHARMAP_TCOD

.. doxygenfunction:: TCOD_tileset_load
.. doxygenfunction:: TCOD_tileset_load_mem
.. doxygenfunction:: TCOD_tileset_load_raw

.. doxygenfunction:: TCOD_load_bdf
.. doxygenfunction:: TCOD_load_bdf_memory

.. doxygenfunction:: TCOD_tileset_render_to_surface

.. doxygenfunction:: TCOD_get_default_tileset
.. doxygenfunction:: TCOD_set_default_tileset
