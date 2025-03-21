/* BSD 3-Clause License
 *
 * Copyright © 2008-2025, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "tileset_bdf.h"

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

TCOD_NODISCARD unsigned char* TCOD_load_binary_file_(const char* path, size_t* size);

TCOD_Tileset* TCOD_load_bdf(const char* path) {
  size_t fsize;
  unsigned char* buffer = TCOD_load_binary_file_(path, &fsize);
  if (!buffer) {
    return NULL;
  }
  TCOD_Tileset* tileset = TCOD_load_bdf_memory((int)fsize, buffer);
  free(buffer);
  return tileset;
}
struct BBox {
  int width;
  int height;
  int xoffset;
  int yoffset;
};
struct BDFLoader {
  /** The BDF data for this loader. */
  const char* const buffer;
  /** A pointer to the end of buffer. */
  const char* const end;
  /** The current position of the BDF parser. */
  const char* cursor;
  /** The current line number under the cursor. */
  int line_number;
  /** The tileset being setup by this loader. */
  TCOD_Tileset* tileset;
  /** Font bounding box. */
  struct BBox bbox;
};
/**
    Check for a keyword.

    Return 0 and move the cursor past the keyword if it is found.

    Otherwise return -1 and do nothing.
 */
static int check_keyword(struct BDFLoader* loader, const char* keyword) {
  ptrdiff_t key_length = (ptrdiff_t)strlen(keyword);
  if (loader->end - loader->cursor < key_length) {
    return -1;
  }
  if (strncmp(loader->cursor, keyword, (size_t)key_length) != 0) {
    return -1;  // No match.
  }
  if (!(loader->end - loader->cursor == key_length || loader->cursor[key_length] == ' ' ||
        loader->cursor[key_length] == '\r' || loader->cursor[key_length] == '\n')) {
    return -1;
  }
  loader->cursor += key_length;
  while (loader->cursor < loader->end && loader->cursor[0] == ' ') {
    ++loader->cursor;
  }
  return 0;
}
/***************************************************************************
    Move the cursor pass any newlines it is currently at.  Return the number of newlines passed.
 */
static int bdf_handle_newlines(struct BDFLoader* loader) {
  int newlines_count = 0;
  while (loader->cursor < loader->end) {
    if (loader->cursor[0] == '\r') {
      ++loader->cursor;
      if (loader->cursor < loader->end && loader->cursor[0] == '\n') {
        ++loader->cursor;
      }
    } else if (loader->cursor[0] == '\n') {
      ++loader->cursor;
    } else {
      break;  // No more newlines at cursor.
    }
    ++newlines_count;
  }
  loader->line_number += newlines_count;
  return newlines_count;
}
/**
    Advance the cursor to the next line.  Returns -1 on error.
 */
static int goto_next_line(struct BDFLoader* loader) {
  while (loader->cursor < loader->end) {
    if (bdf_handle_newlines(loader) > 0) {
      return 0;
    }
    ++loader->cursor;
  }
  TCOD_set_errorv("Unexpected end of data stream.");
  return -1;
}
/**
    Return a number under the cursor and advance.

    This doesn't have any error handing currently.
 */
static int read_next_int(struct BDFLoader* loader) {
  long int number = strtol(loader->cursor, (char**)&loader->cursor, 0);
  return (int)number;
}
/**
    Read the next two hexadecimal numbers in a bitmap line.

    Returns a byte in the uint8_t range or -1 on failure.
 */
static int read_next_bitmap_byte(struct BDFLoader* loader) {
  if (loader->end - loader->cursor < 2) {
    return -1;
  }
  // Fetch and verify the hexadecimal data so that strtoul can't fail.
  char hexstring[3] = {loader->cursor[0], loader->cursor[1], 0};
  if (!isxdigit((int)hexstring[0]) || !isxdigit((int)hexstring[1])) {
    TCOD_set_errorvf("Failed to unpack bitmap on line %d", loader->line_number);
    return -1;
  }
  loader->cursor += 2;
  return (int)strtoul(hexstring, NULL, 16);
}
/**
    Handle BITMAP, parse the bitmap data into a tile for the tileset.
 */
static int parse_bitmap(struct BDFLoader* loader, int codepoint, struct BBox* glyph_bbox) {
  int offset_x = -loader->bbox.xoffset + glyph_bbox->xoffset;
  int offset_y = (loader->bbox.height - glyph_bbox->height + loader->bbox.yoffset - glyph_bbox->yoffset);
  TCOD_ColorRGBA* pixels = malloc(loader->tileset->tile_length * sizeof(*pixels));
  for (int i = 0; i < loader->tileset->tile_length; ++i) {
    pixels[i] = (TCOD_ColorRGBA){255, 255, 255, 0};
  }
  for (int bitmap_y = 0; bitmap_y < glyph_bbox->height; ++bitmap_y) {
    goto_next_line(loader);
    int bitmask = 0;
    for (int bitmap_x = 0; bitmap_x < glyph_bbox->width; ++bitmap_x) {
      if (bitmap_x % 8 == 0) {
        bitmask = read_next_bitmap_byte(loader);
        if (bitmask < 0) {
          free(pixels);
          return -1;
        }
      }
      bool bit = bitmask & (1 << (7 - bitmap_x % 8));
      int target_x = bitmap_x + offset_x;
      int target_y = bitmap_y + offset_y;
      if (0 <= target_x && target_x < loader->tileset->tile_width && 0 <= target_y &&
          target_y < loader->tileset->tile_height) {
        pixels[target_y * loader->tileset->tile_width + target_x].a = bit * 255;
      }
    }
  }
  int err = 0;
  if (codepoint >= 0) {  // Ignore "ENCODING -1".
    err = TCOD_tileset_set_tile_(loader->tileset, codepoint, pixels);
  }
  free(pixels);
  return err;
}
/**
    Handle STARCHAR.
 */
static int parse_char(struct BDFLoader* loader) {
  int codepoint = 0;
  struct BBox glyph_bbox = {0, 0, 0, 0};
  while (goto_next_line(loader) == 0) {
    if (check_keyword(loader, "ENDCHAR") == 0) {
      return 0;
    } else if (check_keyword(loader, "ENCODING") == 0) {
      codepoint = read_next_int(loader);
    } else if (check_keyword(loader, "BBX") == 0) {
      glyph_bbox.width = read_next_int(loader);
      glyph_bbox.height = read_next_int(loader);
      glyph_bbox.xoffset = read_next_int(loader);
      glyph_bbox.yoffset = read_next_int(loader);
    } else if (check_keyword(loader, "BITMAP") == 0) {
      if (parse_bitmap(loader, codepoint, &glyph_bbox) < 0) {
        return -1;
      }
    } else if (check_keyword(loader, "SWIDTH") == 0) {
      // Ignore.
    } else if (check_keyword(loader, "DWIDTH") == 0) {
      // Ignore.
    } else if (check_keyword(loader, "SWIDTH1") == 0) {
      // Ignore.
    } else if (check_keyword(loader, "DWIDTH1") == 0) {
      // Ignore.
    } else if (check_keyword(loader, "VVECTOR") == 0) {
      // Ignore.
    } else if (check_keyword(loader, "") == 0) {  // Ignore empty lines.
    } else {
      TCOD_set_errorvf("Unknown keyword on line %d", loader->line_number + 1);
      return -1;
    }
  }
  TCOD_set_errorv("Unexpected end of data stream.");
  return -1;
}
/**
    Handle CHARS, start converting bitmaps into tiles.
 */
static int parse_bdf_chars(struct BDFLoader* loader) {
  if (!loader->tileset) {
    TCOD_set_errorv("Missing FONTBOUNDINGBOX keyword.");
    return -1;
  }
  int font_glyphs = read_next_int(loader);
  int processed_glyphs = 0;
  while (goto_next_line(loader) == 0) {
    if (check_keyword(loader, "ENDFONT") == 0) {
      if (font_glyphs != processed_glyphs) {
        TCOD_set_errorvf("Expected %d glyphs, but processed %d.", font_glyphs, processed_glyphs);
        return -1;
      }
      return 0;
    } else if (check_keyword(loader, "STARTCHAR") == 0) {
      if (parse_char(loader) < 0) {
        return -1;
      }
      ++processed_glyphs;
    } else {
      TCOD_set_errorvf("Unknown keyword on line %d", loader->line_number);
      return -1;
    }
  }
  TCOD_set_errorv("Unexpected end of data stream.");
  return -1;
}
/**
    Begins parsing the BDF data.
 */
static int parse_bdf(struct BDFLoader* loader) {
  bdf_handle_newlines(loader);
  if (check_keyword(loader, "STARTFONT") < 0) {
    TCOD_set_errorv("BDF files must begin with the STARTFONT keyword.");
    return -1;
  }
  while (goto_next_line(loader) == 0) {
    if (check_keyword(loader, "COMMENT") == 0) {
      // Ignore.
    } else if (check_keyword(loader, "CONTENTVERSION") == 0) {
      // Ignore.
    } else if (check_keyword(loader, "FONT") == 0) {
      // Ignore.
    } else if (check_keyword(loader, "SIZE") == 0) {
      // Ignore.
    } else if (check_keyword(loader, "FONTBOUNDINGBOX") == 0) {
      if (loader->tileset) {
        TCOD_set_errorv("Invalid multiple FONTBOUNDINGBOX keywords found.");
        return -1;
      }
      loader->bbox.width = read_next_int(loader);
      loader->bbox.height = read_next_int(loader);
      loader->bbox.xoffset = read_next_int(loader);
      loader->bbox.yoffset = read_next_int(loader);
      loader->tileset = TCOD_tileset_new(loader->bbox.width, loader->bbox.height);
      if (!loader->tileset) {
        return -1;
      }
    } else if (check_keyword(loader, "METRICSSET") == 0) {
      // Ignore.
    } else if (check_keyword(loader, "STARTPROPERTIES") == 0) {
      int n_properties = read_next_int(loader);
      goto_next_line(loader);
      while (n_properties--) {
        goto_next_line(loader);
      }
      if (check_keyword(loader, "ENDPROPERTIES") < 0) {
        TCOD_set_errorv("Incorrect number of properties.");
        return -1;
      }
    } else if (check_keyword(loader, "CHARS") == 0) {
      return parse_bdf_chars(loader);
    } else if (check_keyword(loader, "SWIDTH") == 0) {
      // Ignore.
    } else if (check_keyword(loader, "DWIDTH") == 0) {
      // Ignore.
    } else if (check_keyword(loader, "SWIDTH1") == 0) {
      // Ignore.
    } else if (check_keyword(loader, "DWIDTH1") == 0) {
      // Ignore.
    } else if (check_keyword(loader, "VVECTOR") == 0) {
      // Ignore.
    } else {
      TCOD_set_errorvf("Unknown keyword on line %d", loader->line_number);
      return -1;
    }
  }
  TCOD_set_errorv("Unexpected end of data stream.");
  return -1;
}
TCOD_Tileset* TCOD_load_bdf_memory(int size, const unsigned char* buffer) {
  struct BDFLoader loader = {
      .buffer = (const char*)buffer,
      .end = (const char*)buffer + size,
      .cursor = (const char*)buffer,
      .line_number = 0,
      .tileset = NULL,
  };
  if (parse_bdf(&loader) < 0) {
    TCOD_tileset_delete(loader.tileset);
    loader.tileset = NULL;
  }
  return loader.tileset;
}
