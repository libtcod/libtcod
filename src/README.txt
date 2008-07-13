This is the Doryen library 1.3.2 source code.

libtcod.h and libtcod.hpp are the main headers, included by the programs that use libtcod. All what they do is to include the toolkit headers.

Each "toolkit" is in a separate file set. For each toolkit, you may have :
*  include/<toolkit>.h : the C header
*  include/<toolkit>.hpp : the C++ header
*  src/<toolkit>_c.c : the C source code that contains the actual implementation
*  src/<toolkit>.cpp : the C++ wrapper

Current toolkits :
bresenham : line drawing using the Bresenham algorithm.
color     : 24 bits colors operations.
console   : true color console emulator. It relies on some private functions of the sys toolkit.
fov       : field of view calculator.
image     : image manipulation. It relies on some private functions of the sys toolkit.
mersenne  : pseudorandom number generator using the Mersenne twister algorithm.
mouse     : mouse support.
perlin    : perlin noise generator.
parser    : config file parser.
sys       : system specific functions. 
list      : a fast and lightweight generic container (faster than STL).

Note that the mouse and sys toolkits rely on system dependant functions. They have currently a single implementation relying on SDL :
mouse_sdl_c.c and sys_sdl_c.c.

libtcod_int.h contains everything that is exported by the toolkits (implemented in a toolkit and used in another one) but should not be seen by the user of the library.

These are the private (or undocumented) modules :
lex       : a generic lexical parser.
tree      : a minimalist tree toolkit.

