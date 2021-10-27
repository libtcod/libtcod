
******
Colors
******

.. include:: ../old_docs_notice.inc.rst

libtcod uses 32-bit color, therefore your OS desktop must also use 32-bit color.  A color is defined by its red, green and blue component between 0 and 255.

You can use the following predefined colors (hover over a color to see its full name and R,G,B values):

INSERT COLOUR TABLE IN A PAINLESS MANNER

Create your own colors
^^^^^^^^^^^^^^^^^^^^^^

You can create your own colours using a set of constructors, both for RGB and HSV values.

.. code-block:: c

    /* RGB */
    TCOD_color_t my_color= { 24, 64, 255 };
    TCOD_color_t my_other_color = TCOD_color_RGB(24, 64, 255);
    /* HSV */
    TCOD_color_t my_yet_another_color = TCOD_color_HSV(321.0f, 0.7f, 1.0f);

.. code-block:: cpp

    // RGB
    TCODColor myColor(24, 64, 255);
    // HSV
    TCODColor myOtherColor(321.0f, 0.7f, 1.0f);

.. doxygenstruct:: tcod::ColorRGB
  :members:

.. doxygenstruct:: tcod::ColorRGBA
  :members:

Compare two colors
^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: TCOD_color_equals

Add and subtract Colors
^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: TCOD_color_add

.. doxygenfunction:: TCOD_color_subtract

Multiply Colors together
^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: TCOD_color_multiply

.. doxygenfunction:: TCOD_color_multiply_scalar

Interpolate between two colors
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: TCOD_color_lerp

Define a color by its hue, saturation and value
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

After this function is called, the r,g,b fields of the color are calculated according to the h,s,v parameters.

.. doxygenfunction:: TCOD_color_set_HSV

These functions set only a single component in the HSV color space.

.. doxygenfunction:: TCOD_color_set_hue
.. doxygenfunction:: TCOD_color_set_saturation
.. doxygenfunction:: TCOD_color_set_value

Get a color hue, saturation and value components
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: TCOD_color_get_HSV

Should you need to extract only one of the HSV components, these functions are what you should call. Note that if you need all three values, it's way less burdensome for the CPU to call `TCODColor::getHSV()`.

.. doxygenfunction:: TCOD_color_get_hue
.. doxygenfunction:: TCOD_color_get_saturation
.. doxygenfunction:: TCOD_color_get_value

Shift a color's hue up or down
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The hue shift value is the number of grades the color's hue will be shifted. The value can be negative for shift left, or positive for shift right.
Resulting values H < 0 and H >= 360 are handled automatically.

.. doxygenfunction:: TCOD_color_shift_hue

Scale a color's saturation and value
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: TCOD_color_scale_HSV

Generate a smooth color map
^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can define a color map from an array of color keys. Colors will be interpolated between the keys.
0 -> black
4 -> red
8 -> white
Result:

INSERT TABLE.

.. doxygenfunction:: TCOD_color_gen_map
