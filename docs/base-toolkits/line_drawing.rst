Line Drawing Toolkit
====================

Iterator-based line drawing
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    #include <libtcod.h>

    void main() {
      TCODFOV_bresenham_data_t bresenham_data;
      int x=5, y=8;
      TCODFOV_line_init_mt(x, y, 13, 14, &bresenham_data);
      do {
        printf("%d %d\n", x, y);
      } while (!TCODFOV_line_step_mt(&x, &y, &bresenham_data));
    }

.. doxygenstruct:: TCODFOV_bresenham_data_t

.. doxygenfunction:: TCODFOV_line_init_mt
.. doxygenfunction:: TCODFOV_line_step_mt

Callback-based line drawing
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    #include <libtcod.h>

    void main() {
      bool my_listener(int x,int y) {
        printf("%d %d\n", x, y);
        return true;
      }
      TCODFOV_line(5, 8, 13, 4, my_listener);
    }

.. doxygentypedef:: TCODFOV_line_listener_t

.. doxygenfunction:: TCODFOV_line

.. doxygenclass:: tcod::BresenhamLine
    :members:

Deprecated functions
^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: TCODFOV_line_mt
.. doxygenfunction:: TCODFOV_line_init
.. doxygenfunction:: TCODFOV_line_step
