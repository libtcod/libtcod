/*
* libtcod 1.6.3
* Copyright (c) 2008,2009,2010,2012,2013,2016,2017 Jice & Mingos & rmtew
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice or Mingos may not be used to endorse or promote
*       products derived from this software without specific prior written
*       permission.
*
* THIS SOFTWARE IS PROVIDED BY JICE, MINGOS AND RMTEW ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE, MINGOS OR RMTEW BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <console.h>

#ifdef TCOD_CONSOLE_SUPPORT

#include <zlib.h>

/* Needed only for TCOD_fatal */
#include <libtcod_int.h>

#include <console_types.h>
#include <color.h>

/* Confirm that the current types are the same as what this code expects. */
#if UINT_MAX != 0xffffffff
#error int type must be 32-bit!
#endif

#ifdef TCOD_SDL2
#include <SDL_endian.h>
#if SDL_BYTEORDER != SDL_LIL_ENDIAN
#error byte-order must be little endian!
#endif
#endif

struct RexPaintHeader {
	int version;
	int layer_count;
};
struct RexPaintLayerChunk {
	int width;
	int height;
};
struct RexPaintTile {
	int ch;
	TCOD_color_t fg;
	TCOD_color_t bg;
};
/* Read data from a gz file, returns 0 on success, or -1 on any error. */
static int load_gz_confirm(gzFile gz_file, void *data, size_t length) {
	if (gzread(gz_file, data, (int)length) != length) { return -1; }
	return 0;
}
static int load_header(gzFile gz_file, struct RexPaintHeader *xp_header) {
	return load_gz_confirm(gz_file, xp_header, sizeof(xp_header[0]));
}
static int load_layer(gzFile gz_file, struct RexPaintLayerChunk *xp_layer) {
	return load_gz_confirm(gz_file, xp_layer, sizeof(xp_layer[0]));
}
/* Read a single REXPaint tile, return 0 on success, or -1 on error. */
static int load_tile(gzFile gz_file, struct RexPaintTile *tile) {
	return (
		load_gz_confirm(gz_file, &tile->ch, sizeof(tile->ch)) ||
		load_gz_confirm(gz_file, &tile->fg, sizeof(tile->fg)) ||
		load_gz_confirm(gz_file, &tile->bg, sizeof(tile->bg))
		);
}
/* Read a layer of REXPaint tiles onto a console.
	 If transparent is true, then follow REXPaint's rules for transparency. */
static int load_tiles(
		gzFile gz_file, TCOD_console_t console, int transparent) {
	int x, y;
	const int width = TCOD_console_get_width(console);
	const int height = TCOD_console_get_height(console);
	/* REXPaint tiles are in column-major order. */
	for (x = 0; x < width; ++x) {
		for (y = 0; y < height; ++y) {
			struct RexPaintTile tile;
			if (load_tile(gz_file, &tile)) {
				return -1;
			}
			/* REXPaint uses a magic pink background to mark transparency. */
			if (transparent &&
				tile.bg.r == 0xff &&
				tile.bg.g == 0x00 &&
				tile.bg.b == 0xff) { continue; }
			TCOD_console_set_char(console, x, y, tile.ch);
			TCOD_console_set_char_foreground(console, x, y, tile.fg);
			TCOD_console_set_char_background(console, x, y, tile.bg, TCOD_BKGND_SET);
		}
	}
	return 0;
}
/* Return the next REXPaint layer as a console.  After reading the header you
	could just keep calling this function until it returns NULL. */
static TCOD_console_t load_console(gzFile gz_file) {
	struct RexPaintLayerChunk xp_layer;
	TCOD_console_t console;
	if (load_layer(gz_file, &xp_layer)) { return NULL; }
	console = TCOD_console_new(xp_layer.width, xp_layer.height);
	if (!console) { return NULL; }
	if (load_tiles(gz_file, console, 0)) {
		TCOD_console_delete(console);
		return NULL;
	}
	return console;
}
/* Load all the contents of a REXPaint file into a list of consoles. */
static TCOD_list_t load_consoleList(gzFile gz_file) {
	struct RexPaintHeader xp_header;
	TCOD_list_t console_list;
	int i;
	if (load_header(gz_file, &xp_header)) { return NULL; }
	console_list = TCOD_list_allocate(xp_header.layer_count);
	if (!console_list) { return NULL; }
	for (i = 0; i < xp_header.layer_count; ++i) {
		TCOD_console_t console = load_console(gz_file);
		if (!console) {
			/* There was an issue then delete everything so far and return NULL */
			while (!TCOD_list_is_empty(console_list)) {
				TCOD_console_delete(TCOD_list_pop(console_list));
			}
			TCOD_list_delete(console_list);
			return NULL;
		}
		TCOD_list_push(console_list, console);
	}
	return console_list;
}
/* Convert a list of consoles into a single console, deleting the list.
	Follows REXPaint's rules for transparency. */
static TCOD_console_t combine_console_list(TCOD_list_t console_list) {
	TCOD_console_t main_console;
	if (!console_list) { return NULL; }
	/* Reverse the list so that elements will be dequeued. */
	TCOD_list_reverse(console_list);
	main_console = TCOD_list_pop(console_list);
	while (!TCOD_list_is_empty(console_list)) {
		TCOD_console_t console = TCOD_list_pop(console_list);
		/* Set key color to {255, 0, 255} before blit. */
		TCOD_console_set_key_color(console, TCOD_fuchsia);
		/* This blit may fail if the consoles do not match shapes. */
		TCOD_console_blit(console, 0, 0, 0, 0, main_console, 0, 0, 1.0f, 1.0f);
		TCOD_console_delete(console);
	}
	TCOD_list_delete(console_list);
	return main_console;
}
/**
 *  \brief Return a list of consoles from a REXPaint file.
 *
 *  \param [in] A path to the REXPaint file.
 *  \return Returns a TCOD_list_t of TCOD_console_t objects.  Or NULL on an
 *  error.  You will need to delete this list and each console individually.
 *
 *  \details This function can load a REXPaint file with variable layer shapes,
 *  which would cause issues for a function like TCOD_console_list_from_xp.
 */
TCOD_list_t TCOD_console_list_from_xp(const char *filename) {
	int z_errno = Z_ERRNO;
	TCOD_list_t console_list;
	gzFile gz_file = gzopen(filename, "rb");
	if (!gz_file) {
		TCOD_fatal("Could not open file: '%s'", filename);
		return NULL;
	}
	console_list = load_consoleList(gz_file);
	if (!console_list){
		TCOD_fatal("Error parsing '%s'\n%s", filename, gzerror(gz_file, &z_errno));
		/* Could fall-through here and return NULL. */
	}
	gzclose(gz_file);
	return console_list;
}
/**
 *  \brief Return a new console loaded from a REXPaint ``.xp`` file.
 *
 *  \param [in] filename A path to the REXPaint file.
 *  \param [in] layer The specific layer to return, or ``0`` to return a
 *  console with all layers overlaid using REXPaint's rules for transparency.
 *  \return A new TCOD_console_t object.  New consoles will need
 *  to be deleted with a call to :any:`TCOD_console_delete`.
 *  Returns NULL on an error.
 *
 *  \details REXPaint gives special treatment to tiles with a magic pink
 *  ``{255, 0, 255}`` background color.  You can processes this color
 *  manually or set :any:`TCOD_console_set_key_color` to `TCOD_fuchsia`.
 */
TCOD_console_t TCOD_console_from_xp(const char *filename) {
	return combine_console_list(TCOD_console_list_from_xp(filename));
}
/**
 *  \brief Update a console from a REXPaint ``.xp`` file.
 *
 *  \param [out] con A console instance to update from the REXPaint file.
 *  \param [in] filename A path to the REXPaint file.
 *
 *  \details REXPaint gives special treatment to tiles with a magic pink
 *  ``{255, 0, 255}`` background color.  You can processes this color
 *  manually or set :any:`TCOD_console_set_key_color` to `TCOD_fuchsia`.
 */
bool TCOD_console_load_xp(TCOD_console_t con, const char *filename) {
	TCOD_console_t xp_console = TCOD_console_from_xp(filename);
	if (!xp_console) { return false; }
	if (TCOD_console_get_width(con) != TCOD_console_get_width(xp_console) ||
			TCOD_console_get_height(con) != TCOD_console_get_height(xp_console)) {
		TCOD_console_delete(xp_console);
		return false;
	}
	TCOD_console_blit(xp_console, 0, 0, 0, 0, con, 0, 0, 1.0f, 1.0f);
	TCOD_console_delete(xp_console);
	return true;
}
static int write_header(gzFile gz_file, struct RexPaintHeader *xp_header) {
	if (!gzwrite(gz_file, xp_header, sizeof(xp_header[0]))) {
		return -1;
	}
	return 0;
}
static int write_layer(gzFile gz_file, struct RexPaintLayerChunk *xp_layer) {
	if (!gzwrite(gz_file, xp_layer, sizeof(xp_layer[0]))) {
		return -1;
	}
	return 0;
}
static int write_tile(gzFile gz_file, struct RexPaintTile *tile) {
	if (!gzwrite(gz_file, &tile->ch, sizeof(tile->ch)) ||
			!gzwrite(gz_file, &tile->fg, sizeof(tile->fg)) ||
			!gzwrite(gz_file, &tile->bg, sizeof(tile->bg))) {
		return -1;
	}
	return 0;
}
static int write_console(gzFile gz_file, TCOD_console_t console) {
	int x, y;
	struct RexPaintLayerChunk xp_layer;
	xp_layer.width = TCOD_console_get_width(console);
	xp_layer.height = TCOD_console_get_height(console);
	if (write_layer(gz_file, &xp_layer)) {
		return -1; /* error writing layer */
	}
	/* Write console data out in column-major order. */
	for (x = 0; x < xp_layer.width; ++x) {
		for (y = 0; y < xp_layer.height; ++y) {
			struct RexPaintTile tile;
			tile.ch = TCOD_console_get_char(console, x, y);
			tile.fg = TCOD_console_get_char_foreground(console, x, y);
			tile.bg = TCOD_console_get_char_background(console, x, y);
			if (write_tile(gz_file, &tile)) {
				return -1; /* error writing tile data */
			}
		}
	}
	return 0;
}
/**
 *  \brief Save a console as a REXPaint ``.xp`` file.
 *
 *  \param [in] con The console instance to save.
 *  \param [in] filename The filepath to save to.
 *  \param [in] compress_level A zlib compression level, from 0 to 9.
 *  1=fast, 6=balanced, 9=slowest, 0=uncompressed.
 *  \return ``true`` when the file is saved succesfully, or ``false`` when an
 *  issue is detected.
 *
 *  \details REXPaint has enough fidelity to make a 1:1 copy of a libtcod
 *  console.
 */
bool TCOD_console_save_xp(
		TCOD_console_t con, const char *filename, int compress_level) {
	struct RexPaintHeader xp_header;
	gzFile gz_file = gzopen(filename, "wb");
	if (!gz_file) { return false; /* could not open file */ }
	gzsetparams(gz_file, compress_level, Z_DEFAULT_STRATEGY);
	xp_header.version = -1; /* REXPaint uses this version. */
	xp_header.layer_count = 1;
	if (write_header(gz_file, &xp_header) || write_console(gz_file, con)) {
		gzclose(gz_file);
		return false; /* error writing data */
	}
	if (gzclose(gz_file)) { return false; /* error writing to file */ }
	return true;
}
/**
 *  \brief Save a list of consoles to a REXPaint file.
 *
 *  \param [in] A TCOD_list_t of TCOD_console_t objects.
 *  \param [in] Path to save to.
 *  \param [in] zlib compression level.
 *  \return true on success, false on a failure such as not being able to write
 *  to the path provided.
 *
 *  \details The REXPaint tool can only work on files with up to 4 layers where
 *  all layers are the same size.
 *
 *  This function can save any number of layers with multiple different sizes,
 *  these files can be loaded without issues as long you use
 *  :any:`TCOD_console_list_from_xp` for loading.
 */
bool TCOD_console_list_save_xp(
		TCOD_list_t console_list, const char *filename, int compress_level) {
	int i;
	struct RexPaintHeader xp_header;
	gzFile gz_file = gzopen(filename, "wb");
	if (!gz_file) { return false; /* could not open file */ }
	gzsetparams(gz_file, compress_level, Z_DEFAULT_STRATEGY);
	xp_header.version = -1;
	xp_header.layer_count = TCOD_list_size(console_list);
	if (write_header(gz_file, &xp_header)) {
		gzclose(gz_file);
		return false; /* error writing metadata */
	}
	for (i = 0; i < xp_header.layer_count; ++i){
		if (write_console(gz_file, TCOD_list_get(console_list, i))) {
			gzclose(gz_file);
			return false; /* error writing out console data */
		}
	}
	if (gzclose(gz_file)) { return false; /* error writing to file */ }
	return true;
}

#endif /* TCOD_CONSOLE_SUPPORT */
