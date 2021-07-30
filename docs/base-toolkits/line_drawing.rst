Line Drawing Toolkit
====================

Iterator-based line drawing
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    #include <libtcod.h>

    void main() {
      TCOD_bresenham_data_t bresenham_data;
      int x=5, y=8;
      TCOD_line_init_mt(x, y, 13, 14, &bresenham_data);
      do {
        printf("%d %d\n", x, y);
      } while (!TCOD_line_step_mt(&x, &y, &bresenham_data));
    }

.. doxygenstruct:: TCOD_bresenham_data_t

.. doxygenfunction:: TCOD_line_init_mt
.. doxygenfunction:: TCOD_line_step_mt

Callback-based line drawing
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    #include <libtcod.h>

    void main() {
      bool my_listener(int x,int y) {
        printf("%d %d\n", x, y);
        return true;
      }
      TCOD_line(5, 8, 13, 4, my_listener);
    }

.. doxygentypedef:: TCOD_line_listener_t

.. doxygenfunction:: TCOD_line

.. doxygenclass:: tcod::BresenhamLine
    :members:

Deprecated functions
^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: TCOD_line_mt
.. doxygenfunction:: TCOD_line_init
.. doxygenfunction:: TCOD_line_step
