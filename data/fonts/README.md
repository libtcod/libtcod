# Libtcod font examples

This directory contains some example fonts for libtcod.
These fonts are generally free to use, but the license for the `terminal` tilesets has not been accounted for correctly.

## Tilesets

Tilesets are a smaller collection of glyphs arranged in a specific layout, usually [Code Page 437](https://en.wikipedia.org/wiki/Code_page_437).

All standard Code Page 437 tilesets are loaded with the following call:
```cpp
auto tileset = tcod::load_tilesheet("path/to/tileset.png", {32, 32}, tcod::CHARMAP_CP437);
```

Libtcod is able to load tilesets from the [Dwarf Fortress Tileset Repository](https://dwarffortresswiki.org/Tileset_repository) with the above extra configuration.
It also supports all the tilesets from [this tileset browser](https://extended-ascii-viewer.herokuapp.com/).

The file names in this directory are composed with:
```
<font_name><font_size>_<type>_<layout>.png
<type> : gs 8-bit or less greyscale PNG
<layout> : ro standard Code Page 437 layout in standard row-major order
           tc Deprecated TCOD layout
```

## BDF

Libtcod also supports BDF files.
These fonts usually have decent Unicode support.

BDF files can be loaded in libtcod with the following call:
```cpp
auto tileset = tcod::load_bdf("path/to/font.bdf");
```

You can find BDF's in various places.
Recommended fonts are:
[Unicode fonts for X11](https://www.cl.cam.ac.uk/~mgk25/ucs-fonts.html),
[Cozette](https://github.com/slavfox/Cozette),
[Tamzen](https://github.com/sunaku/tamzen-font),
and [Envypn](https://github.com/Sorixelle/envypn-powerline).

## TrueType Font

TrueType font files are aliased and have good Unicode support.

Tilesets can be generated from TTF files using a tool like [Typesheet](https://github.com/graysonchao/typesheet).

There is limited support for loading TTF files in libtcod.
*This experimental function is provisional and may change in the future:*
```cpp
TCOD_Tileset* tileset = TCOD_load_truetype_font_("path/to/font.ttf", 20, 10);  // 20 pixels high, half-width glyphs.
TCOD_Tileset* tileset = TCOD_load_truetype_font_("path/to/font.ttf", 20, 20);  // 20 pixels high, monospaced glyphs.
```
To get the most of a TTF you should regenerate the tileset with a size closely matching your tile size whenever the window is resized.
Otherwise you'll get scaling artifacts from the font being rendered at a low resolution and then scaled up.
