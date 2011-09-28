This directory contains antialiased fonts for libtcod.
These fonts are in public domain. 

The file names are composed with :
<font_name><font_size>_<type>_<layout>.png
<type> : aa 32 bits png with alpha channel
         gs 24 bits or greyscale PNG
<layout> : as standard ASCII layout
           ro standard ASCII layout in row
           tc TCOD layout

The terminal8x8 font is provided is every possible format as en example.
You can try them with the provided samples :

./samples_c -font fonts/terminal8x8_aa_as.png -font-nb-char 16 16
./samples_c -font fonts/terminal8x8_aa_ro.png -font-nb-char 16 16 -font-in-row
./samples_c -font fonts/terminal8x8_aa_tc.png -font-nb-char 32 8 -font-tcod
./samples_c -font fonts/terminal8x8_gs_as.png -font-nb-char 16 16 -font-greyscale
./samples_c -font fonts/terminal8x8_gs_ro.png -font-nb-char 16 16 -font-greyscale -font-in-row
./samples_c -font fonts/terminal8x8_gs_tc.png -font-nb-char 32 8 -font-greyscale -font-tcod

The libtcod 1.3.2 (non antialiased) terminal font is still there and still works :
./samples_c -font terminal.png -font-nb-char 16 16

All other fonts are provided only in gs_tc format (greyscale, TCOD layout).
To try them :

Terminal fonts with different size (you can use them as template to create new fonts) :
./samples_c -font fonts/terminal7x7_gs_tc.png -font-nb-char 32 8 -font-greyscale -font-tcod
./samples_c -font fonts/terminal10x10_gs_tc.png -font-nb-char 32 8 -font-greyscale -font-tcod

Custom fonts :
./samples_c -font fonts/caeldera8x8_gs_tc.png -font-nb-char 32 8 -font-greyscale -font-tcod
./samples_c -font fonts/lucida8x8_gs_tc.png -font-nb-char 32 8 -font-greyscale -font-tcod
./samples_c -font fonts/celtic_garamond_10x10_gs_tc.png -font-nb-char 32 8 -font-greyscale -font-tcod
./samples_c -font fonts/dundalk12x12_gs_tc.png -font-nb-char 32 8 -font-greyscale -font-tcod
