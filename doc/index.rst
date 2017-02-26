.. libtcod documentation master file, created by
   sphinx-quickstart on Sun Feb 26 16:53:45 2017.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

libtcod
=======

Contents:

.. toctree::
   :maxdepth: 2

Colours
-------

libtcod uses 32-bit color, therefore your OS desktop must also use 32-bit color.  A color is defined by its red, green and blue component between 0 and 255.

You can use the following predefined colors (hover over a color to see its full name and R,G,B values):

INSERT COLOUR TABLE IN A PAINLESS MANNER

Create your own colors
^^^^^^^^^^^^^^^^^^^^^^

You can create your own colours using a set of constructors, both for RGB and HSV values.

Compare two colors
^^^^^^^^^^^^^^^^^^

Multiply two colors
^^^^^^^^^^^^^^^^^^^

Multiply a color by a float
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Adding two colors
^^^^^^^^^^^^^^^^^

Subtract two colors
^^^^^^^^^^^^^^^^^^^

Interpolate between two colors
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Define a color by its hue, saturation and value
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

After this function is called, the r,g,b fields of the color are calculated according to the h,s,v parameters.

Define a color's hue, saturation or lightness
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These functions set only a single component in the HSV color space.

Get a color hue, saturation and value components
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Get a color's hue, saturation or value
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Should you need to extract only one of the HSV components, these functions are what you should call. Note that if you need all three values, it's way less burdensome for the CPU to call `TCODColor::getHSV()`.

Shift a color's hue up or down
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The hue shift value is the number of grades the color's hue will be shifted. The value can be negative for shift left, or positive for shift right.
Resulting values H < 0 and H >= 360 are handled automatically.

Scale a color's saturation and value
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Generate a smooth color map
^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can define a color map from an array of color keys. Colors will be interpolated between the keys.
0 -> black
4 -> red
8 -> white
Result:

INSERT TABLE.

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

