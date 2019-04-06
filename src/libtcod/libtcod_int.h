/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
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
#ifndef TCODLIB_INT_H_
#define TCODLIB_INT_H_
#include <stdarg.h>
#include <assert.h>
#if defined(__ANDROID__)
#include <android/log.h>
#endif
#ifdef __cplusplus
#include <stdexcept>
#include <string>
#endif // __cplusplus

#include "portability.h"
#include "color.h"
#include "console_types.h"
#include "fov.h"
#include "fov_types.h"
#include "mersenne_types.h"
#include "sys.h"

/* tcodlib internal stuff */
#ifdef __cplusplus
extern "C" {
#endif

/* SDL2 forward declarations */
struct SDL_Surface;
struct SDL_Window;
struct SDL_Renderer;

/* pseudorandom number generator toolkit */
typedef struct TCOD_Random {
	/* algorithm identifier */
	TCOD_random_algo_t algo;
	/* distribution */
	TCOD_distribution_t distribution;
	/* Mersenne Twister stuff */
	uint32_t mt[624];
	int cur_mt;
	/* Complementary-Multiply-With-Carry stuff */
	/* shared with Generalised Feedback Shift Register */
	uint32_t Q[4096], c;
    int cur;
} mersenne_data_t;

typedef struct {
	/* number of characters in the bitmap font */
	int fontNbCharHoriz;
	int fontNbCharVertic;
	/* font type and layout */
	bool font_tcod_layout;
	bool font_in_row;
	bool font_greyscale;
  int font_flags;
	/* character size in font */
	int font_width;
	int font_height;
	char font_file[512];
	char window_title[512];
	/* ascii code to tcod layout converter */
	int *ascii_to_tcod;
	/* whether each character in the font is a colored tile */
	bool *colored;
#ifdef TCOD_CONSOLE_SUPPORT
	/* the root console */
	struct TCOD_Console *root;
	/* nb chars in the font */
	int max_font_chars;
	/* fullscreen data */
	bool fullscreen;
	int fullscreen_offsetx;
	int fullscreen_offsety;
	/* asked by the user */
	int fullscreen_width;
	int fullscreen_height;
	/* actual resolution */
	int actual_fullscreen_width;
	int actual_fullscreen_height;
#endif
#ifndef TCOD_BARE
	/* renderer to use */
	TCOD_renderer_t renderer;
	/* user post-processing callback */
	SDL_renderer_t sdl_cbk;
#endif
	/* fading data */
	TCOD_color_t fading_color;
	uint8_t fade;
#ifdef TCOD_CONSOLE_SUPPORT
	TCOD_key_t key_state;
#endif
#ifndef TCOD_BARE
	/* application window was closed */
	bool is_window_closed;
	/* application has mouse focus */
	bool app_has_mouse_focus;
	/* application is active (not iconified) */
	bool app_is_active;
#endif
} TCOD_internal_context_t;

extern TCOD_internal_context_t TCOD_ctx;

#if defined(__ANDROID__) && !defined(NDEBUG)
#include <android/log.h>
#ifdef printf
#undef printf
#endif
#ifdef vprintf
#undef vprintf
#endif
#define printf(args...) __android_log_print(ANDROID_LOG_INFO, "libtcod", ## args)
#define vprintf(args...) __android_log_vprint(ANDROID_LOG_INFO, "libtcod", ## args)

#ifdef assert
#undef assert
#endif
#define assert(cond) if(!(cond)) __android_log_assert(#cond, "libtcod", "assertion failed: %s", #cond)
#endif

#ifdef NDEBUG
#define TCOD_IF(x) if (x)
#define TCOD_IFNOT(x) if (!(x))
#define TCOD_ASSERT(x)
#define TCOD_LOG(x)
#else
#define TCOD_IF(x) assert(x);
#define TCOD_IFNOT(x) assert(x); if (0)
#define TCOD_ASSERT(x) assert(x)
#define TCOD_LOG(x) printf x
#endif

#if !defined(TCOD_BARE) && !defined(NO_OPENGL)
/* opengl utilities */
void TCOD_opengl_init_attributes(void);
bool TCOD_opengl_init_state(int conw, int conh, struct SDL_Surface* font_tex);
void TCOD_opengl_uninit_state(void);
bool TCOD_opengl_init_shaders(void);
bool TCOD_opengl_render(int oldFade, bool *ascii_updated, struct TCOD_Console *console, struct TCOD_Console *cache);
void TCOD_opengl_swap(void);
struct SDL_Surface* TCOD_opengl_get_screen(void);
#endif

#ifdef TCOD_IMAGE_SUPPORT
/* image internal stuff */
bool TCOD_image_mipmap_copy_internal(const TCOD_Image* srcImage,
                                     TCOD_Image* dstImage);
TCOD_color_t *TCOD_image_get_colors(TCOD_Image* image);
void TCOD_image_invalidate_mipmaps(TCOD_Image* image);
void TCOD_image_get_key_data(const TCOD_Image* image,
                             bool *has_key_color, TCOD_color_t *key_color);
#endif

/* fov internal stuff */
void TCOD_map_compute_fov_circular_raycasting(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls);
void TCOD_map_compute_fov_diamond_raycasting(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls);
void TCOD_map_compute_fov_recursive_shadowcasting(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls);
void TCOD_map_compute_fov_permissive2(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls, int fovType);
void TCOD_map_compute_fov_restrictive_shadowcasting(TCOD_map_t map, int player_x, int player_y, int max_radius, bool light_walls);
void TCOD_map_postproc(TCOD_map_t map,int x0,int y0, int x1, int y1, int dx, int dy);

/* fatal errors */
void TCOD_fatal(const char *fmt, ...);
void TCOD_fatal_nopar(const char *msg);

/* switch fullscreen mode */
void TCOD_sys_set_fullscreen(bool fullscreen);
void TCOD_sys_set_clear_screen(void);
void TCOD_sys_set_scale_factor(float value);
void TCOD_sys_convert_console_to_screen_coords(int cx, int cy, int *sx, int *sy);
void TCOD_sys_convert_screen_to_console_coords(int sx, int sy, int *cx, int *cy);
void TCOD_sys_flush(bool render);
TCOD_key_t TCOD_sys_check_for_keypress(int flags);
TCOD_key_t TCOD_sys_wait_for_keypress(bool flush);
bool TCOD_sys_is_key_pressed(TCOD_keycode_t key);
void TCOD_sys_set_window_title(const char *title);
void TCOD_sys_pixel_to_tile(double* x, double* y);

int TCOD_console_print_internal(
    TCOD_Console* con, int x,int y, int w, int h, TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t align, char* msg, bool can_split, bool count_only
);
char* TCOD_console_vsprint(const char* fmt, va_list ap);
/* UTF-8 stuff */
#ifndef NO_UNICODE
wchar_t *TCOD_console_vsprint_utf(const wchar_t *fmt, va_list ap);
int TCOD_console_print_internal_utf(TCOD_console_t con,int x,int y, int rw, int rh, TCOD_bkgnd_flag_t flag,
	TCOD_alignment_t align, wchar_t *msg, bool can_split, bool count_only);
#endif

#ifdef TCOD_IMAGE_SUPPORT
/* image manipulation */
TCODLIB_API struct SDL_Surface* TCOD_sys_load_image(const char *filename);
void TCOD_sys_get_image_size(const struct SDL_Surface *image, int *w,int *h);
TCOD_color_t TCOD_sys_get_image_pixel(const struct SDL_Surface *image,
                                      int x, int y);
int TCOD_sys_get_image_alpha(const struct SDL_Surface* image, int x, int y);
bool TCOD_sys_check_magic_number(const char *filename, size_t size, uint8_t *data);
#endif

/* TCOD_list nonpublic methods */
void TCOD_list_set_size(TCOD_list_t l, int size);

#ifndef TCOD_BARE
/*
	SDL12/SDL2 abstraction layer
*/
typedef struct TCOD_SDL_driver_t {
    float scale_xc;
    float scale_yc;

	/* get a fullscreen mode suitable for the console */
	void (*get_closest_mode)(int *w, int *h);
	/* render the console on a surface/texture */
	void (*render)(struct TCOD_SDL_driver_t *sdl, void *vbitmap, struct TCOD_Console *console);
	/* create a new surface */
	struct SDL_Surface *(*create_surface) (int width, int height, bool with_alpha);
	/* create the game window */
	void (*create_window)(int w, int h, bool fullscreen);
	/* destroy the game window */
	void (*destroy_window)(void);
	/* switch fullscreen on/off */
	void (*set_fullscreen)(bool fullscreen);
	/* change the game window title */
	void (*set_window_title)(const char *title);
	/* save game screenshot */
	void (*save_screenshot)(const char *filename);
	/* get desktop resolution */
	void (*get_current_resolution)(int *w, int *h);
	/* change the mouse cursor position */
	void (*set_mouse_position)(int x, int y);
	/* clipboard */
	const char *(*get_clipboard_text)(void);
	bool (*set_clipboard_text)(const char *text);
	/* android compatible file access functions */
	bool (*file_read)(const char *filename, unsigned char **buf, size_t *size);
	bool (*file_exists)(const char * filename);
	bool (*file_write)(const char *filename, unsigned char *buf, uint32_t size);
	/* clean stuff */
	void (*shutdown)(void);
	/* get root cache */
	struct TCOD_Console *(*get_root_console_cache)(void);
} TCOD_SDL_driver_t;

/* defined in TCOD_sys_sdl12_c.c and TCOD_sys_sdl2_c.c */
TCOD_SDL_driver_t *SDL_implementation_factory(void);

void find_resolution(void);
void TCOD_sys_init_screen_offset(void);
typedef struct {
	float force_recalc;
	float last_scale_xc, last_scale_yc;
	float last_scale_factor;
	float last_fullscreen;

	float min_scale_factor;

	float src_height_width_ratio;
	float dst_height_width_ratio;
	int src_x0, src_y0;
	int src_copy_width, src_copy_height;
	int src_proportionate_width, src_proportionate_height;
	int dst_display_width, dst_display_height;
	int dst_offset_x, dst_offset_y;
	int surface_width, surface_height;
} scale_data_t;
extern scale_data_t scale_data;

extern float scale_factor;
extern struct SDL_Surface* charmap;
extern struct SDL_Window* window;
extern struct SDL_Renderer* renderer;
extern char *last_clipboard_text;
#endif

/* SDL & OpenGL */
extern int oldFade;

/* color values */
#define TCOD_BLACK 0,0,0
#define TCOD_DARKEST_GREY 31,31,31
#define TCOD_DARKER_GREY 63,63,63
#define TCOD_DARK_GREY 95,95,95
#define TCOD_GREY 127,127,127
#define TCOD_LIGHT_GREY 159,159,159
#define TCOD_LIGHTER_GREY 191,191,191
#define TCOD_LIGHTEST_GREY 223,223,223
#define TCOD_WHITE 255,255,255

#define TCOD_DARKEST_SEPIA 31,24,15
#define TCOD_DARKER_SEPIA 63,50,31
#define TCOD_DARK_SEPIA 94,75,47
#define TCOD_SEPIA 127,101,63
#define TCOD_LIGHT_SEPIA 158,134,100
#define TCOD_LIGHTER_SEPIA 191,171,143
#define TCOD_LIGHTEST_SEPIA 222,211,195

/* desaturated */
#define TCOD_DESATURATED_RED 127,63,63
#define TCOD_DESATURATED_FLAME 127,79,63
#define TCOD_DESATURATED_ORANGE 127,95,63
#define TCOD_DESATURATED_AMBER 127,111,63
#define TCOD_DESATURATED_YELLOW 127,127,63
#define TCOD_DESATURATED_LIME 111,127,63
#define TCOD_DESATURATED_CHARTREUSE 95,127,63
#define TCOD_DESATURATED_GREEN 63,127,63
#define TCOD_DESATURATED_SEA 63,127,95
#define TCOD_DESATURATED_TURQUOISE 63,127,111
#define TCOD_DESATURATED_CYAN 63,127,127
#define TCOD_DESATURATED_SKY 63,111,127
#define TCOD_DESATURATED_AZURE 63,95,127
#define TCOD_DESATURATED_BLUE 63,63,127
#define TCOD_DESATURATED_HAN 79,63,127
#define TCOD_DESATURATED_VIOLET 95,63,127
#define TCOD_DESATURATED_PURPLE 111,63,127
#define TCOD_DESATURATED_FUCHSIA 127,63,127
#define TCOD_DESATURATED_MAGENTA 127,63,111
#define TCOD_DESATURATED_PINK 127,63,95
#define TCOD_DESATURATED_CRIMSON 127,63,79

/* lightest */
#define TCOD_LIGHTEST_RED 255,191,191
#define TCOD_LIGHTEST_FLAME 255,207,191
#define TCOD_LIGHTEST_ORANGE 255,223,191
#define TCOD_LIGHTEST_AMBER 255,239,191
#define TCOD_LIGHTEST_YELLOW 255,255,191
#define TCOD_LIGHTEST_LIME 239,255,191
#define TCOD_LIGHTEST_CHARTREUSE 223,255,191
#define TCOD_LIGHTEST_GREEN 191,255,191
#define TCOD_LIGHTEST_SEA 191,255,223
#define TCOD_LIGHTEST_TURQUOISE 191,255,239
#define TCOD_LIGHTEST_CYAN 191,255,255
#define TCOD_LIGHTEST_SKY 191,239,255
#define TCOD_LIGHTEST_AZURE 191,223,255
#define TCOD_LIGHTEST_BLUE 191,191,255
#define TCOD_LIGHTEST_HAN 207,191,255
#define TCOD_LIGHTEST_VIOLET 223,191,255
#define TCOD_LIGHTEST_PURPLE 239,191,255
#define TCOD_LIGHTEST_FUCHSIA 255,191,255
#define TCOD_LIGHTEST_MAGENTA 255,191,239
#define TCOD_LIGHTEST_PINK 255,191,223
#define TCOD_LIGHTEST_CRIMSON 255,191,207

/* lighter */
#define TCOD_LIGHTER_RED 255,127,127
#define TCOD_LIGHTER_FLAME 255,159,127
#define TCOD_LIGHTER_ORANGE 255,191,127
#define TCOD_LIGHTER_AMBER 255,223,127
#define TCOD_LIGHTER_YELLOW 255,255,127
#define TCOD_LIGHTER_LIME 223,255,127
#define TCOD_LIGHTER_CHARTREUSE 191,255,127
#define TCOD_LIGHTER_GREEN 127,255,127
#define TCOD_LIGHTER_SEA 127,255,191
#define TCOD_LIGHTER_TURQUOISE 127,255,223
#define TCOD_LIGHTER_CYAN 127,255,255
#define TCOD_LIGHTER_SKY 127,223,255
#define TCOD_LIGHTER_AZURE 127,191,255
#define TCOD_LIGHTER_BLUE 127,127,255
#define TCOD_LIGHTER_HAN 159,127,255
#define TCOD_LIGHTER_VIOLET 191,127,255
#define TCOD_LIGHTER_PURPLE 223,127,255
#define TCOD_LIGHTER_FUCHSIA 255,127,255
#define TCOD_LIGHTER_MAGENTA 255,127,223
#define TCOD_LIGHTER_PINK 255,127,191
#define TCOD_LIGHTER_CRIMSON 255,127,159

/* light */
#define TCOD_LIGHT_RED 255,63,63
#define TCOD_LIGHT_FLAME 255,111,63
#define TCOD_LIGHT_ORANGE 255,159,63
#define TCOD_LIGHT_AMBER 255,207,63
#define TCOD_LIGHT_YELLOW 255,255,63
#define TCOD_LIGHT_LIME 207,255,63
#define TCOD_LIGHT_CHARTREUSE 159,255,63
#define TCOD_LIGHT_GREEN 63,255,63
#define TCOD_LIGHT_SEA 63,255,159
#define TCOD_LIGHT_TURQUOISE 63,255,207
#define TCOD_LIGHT_CYAN 63,255,255
#define TCOD_LIGHT_SKY 63,207,255
#define TCOD_LIGHT_AZURE 63,159,255
#define TCOD_LIGHT_BLUE 63,63,255
#define TCOD_LIGHT_HAN 111,63,255
#define TCOD_LIGHT_VIOLET 159,63,255
#define TCOD_LIGHT_PURPLE 207,63,255
#define TCOD_LIGHT_FUCHSIA 255,63,255
#define TCOD_LIGHT_MAGENTA 255,63,207
#define TCOD_LIGHT_PINK 255,63,159
#define TCOD_LIGHT_CRIMSON 255,63,111

/* normal */
#define TCOD_RED 255,0,0
#define TCOD_FLAME 255,63,0
#define TCOD_ORANGE 255,127,0
#define TCOD_AMBER 255,191,0
#define TCOD_YELLOW 255,255,0
#define TCOD_LIME 191,255,0
#define TCOD_CHARTREUSE 127,255,0
#define TCOD_GREEN 0,255,0
#define TCOD_SEA 0,255,127
#define TCOD_TURQUOISE 0,255,191
#define TCOD_CYAN 0,255,255
#define TCOD_SKY 0,191,255
#define TCOD_AZURE 0,127,255
#define TCOD_BLUE 0,0,255
#define TCOD_HAN 63,0,255
#define TCOD_VIOLET 127,0,255
#define TCOD_PURPLE 191,0,255
#define TCOD_FUCHSIA 255,0,255
#define TCOD_MAGENTA 255,0,191
#define TCOD_PINK 255,0,127
#define TCOD_CRIMSON 255,0,63

/* dark */
#define TCOD_DARK_RED 191,0,0
#define TCOD_DARK_FLAME 191,47,0
#define TCOD_DARK_ORANGE 191,95,0
#define TCOD_DARK_AMBER 191,143,0
#define TCOD_DARK_YELLOW 191,191,0
#define TCOD_DARK_LIME 143,191,0
#define TCOD_DARK_CHARTREUSE 95,191,0
#define TCOD_DARK_GREEN 0,191,0
#define TCOD_DARK_SEA 0,191,95
#define TCOD_DARK_TURQUOISE 0,191,143
#define TCOD_DARK_CYAN 0,191,191
#define TCOD_DARK_SKY 0,143,191
#define TCOD_DARK_AZURE 0,95,191
#define TCOD_DARK_BLUE 0,0,191
#define TCOD_DARK_HAN 47,0,191
#define TCOD_DARK_VIOLET 95,0,191
#define TCOD_DARK_PURPLE 143,0,191
#define TCOD_DARK_FUCHSIA 191,0,191
#define TCOD_DARK_MAGENTA 191,0,143
#define TCOD_DARK_PINK 191,0,95
#define TCOD_DARK_CRIMSON 191,0,47

/* darker */
#define TCOD_DARKER_RED 127,0,0
#define TCOD_DARKER_FLAME 127,31,0
#define TCOD_DARKER_ORANGE 127,63,0
#define TCOD_DARKER_AMBER 127,95,0
#define TCOD_DARKER_YELLOW 127,127,0
#define TCOD_DARKER_LIME 95,127,0
#define TCOD_DARKER_CHARTREUSE 63,127,0
#define TCOD_DARKER_GREEN 0,127,0
#define TCOD_DARKER_SEA 0,127,63
#define TCOD_DARKER_TURQUOISE 0,127,95
#define TCOD_DARKER_CYAN 0,127,127
#define TCOD_DARKER_SKY 0,95,127
#define TCOD_DARKER_AZURE 0,63,127
#define TCOD_DARKER_BLUE 0,0,127
#define TCOD_DARKER_HAN 31,0,127
#define TCOD_DARKER_VIOLET 63,0,127
#define TCOD_DARKER_PURPLE 95,0,127
#define TCOD_DARKER_FUCHSIA 127,0,127
#define TCOD_DARKER_MAGENTA 127,0,95
#define TCOD_DARKER_PINK 127,0,63
#define TCOD_DARKER_CRIMSON 127,0,31

/* darkest */
#define TCOD_DARKEST_RED 63,0,0
#define TCOD_DARKEST_FLAME 63,15,0
#define TCOD_DARKEST_ORANGE 63,31,0
#define TCOD_DARKEST_AMBER 63,47,0
#define TCOD_DARKEST_YELLOW 63,63,0
#define TCOD_DARKEST_LIME 47,63,0
#define TCOD_DARKEST_CHARTREUSE 31,63,0
#define TCOD_DARKEST_GREEN 0,63,0
#define TCOD_DARKEST_SEA 0,63,31
#define TCOD_DARKEST_TURQUOISE 0,63,47
#define TCOD_DARKEST_CYAN 0,63,63
#define TCOD_DARKEST_SKY 0,47,63
#define TCOD_DARKEST_AZURE 0,31,63
#define TCOD_DARKEST_BLUE 0,0,63
#define TCOD_DARKEST_HAN 15,0,63
#define TCOD_DARKEST_VIOLET 31,0,63
#define TCOD_DARKEST_PURPLE 47,0,63
#define TCOD_DARKEST_FUCHSIA 63,0,63
#define TCOD_DARKEST_MAGENTA 63,0,47
#define TCOD_DARKEST_PINK 63,0,31
#define TCOD_DARKEST_CRIMSON 63,0,15

/* metallic */
#define TCOD_BRASS 191,151,96
#define TCOD_COPPER 197,136,124
#define TCOD_GOLD 229,191,0
#define TCOD_SILVER 203,203,203

/* miscellaneous */
#define TCOD_CELADON 172,255,175
#define TCOD_PEACH 255,159,127

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
// TCODConsole non public methods
bool TCOD_console_init(TCOD_Console* con);
bool TCOD_console_init(TCOD_Console* con, const std::string& title,
                       bool fullscreen);
int TCOD_console_stringLength(const unsigned char* s);
unsigned char* TCOD_console_forward(unsigned char* s,int l);
// TCODSystem non public methods
bool TCOD_sys_init(struct TCOD_Console *console, bool fullscreen);
void TCOD_sys_set_custom_font(const char *font_name,int nb_ch, int nb_cv,int flags);
void TCOD_sys_map_ascii_to_font(int asciiCode, int fontCharX, int fontCharY);
void TCOD_sys_decode_font_(void);
struct SDL_Surface* TCOD_sys_create_bitmap_for_console(TCOD_Console* console);
void TCOD_sys_save_bitmap(struct SDL_Surface* bitmap, const char *filename);
struct SDL_Surface* TCOD_sys_create_bitmap(int width, int height,
                                           TCOD_color_t *buf);
void TCOD_sys_delete_bitmap(struct SDL_Surface* bitmap);
void TCOD_sys_console_to_bitmap(
    struct SDL_Surface* bitmap,
    struct TCOD_Console *console,
    struct TCOD_Console *cache);
TCODLIB_CAPI struct SDL_Surface* TCOD_sys_get_surface(int width, int height,
                                                      bool alpha);
void TCOD_sys_save_fps(void);
void TCOD_sys_restore_fps(void);
void TCOD_sys_set_dirty(int dx, int dy, int dw, int dh);
void TCOD_sys_set_dirty_character_code(int ch);
int TCOD_get_tileid_for_charcode_(int charcode);
namespace tcod {
namespace console {
/**
 *  Validate and return a console.
 */
inline TCOD_Console* validate_(TCOD_Console* console)
{
  console = (console ? console : TCOD_ctx.root);
  TCOD_ASSERT(console);
  return console;
}
/**
 *  Validate and return a constant console.
 */
inline const TCOD_Console* validate_(const TCOD_Console* console)
{
  console = (console ? console : TCOD_ctx.root);
  TCOD_ASSERT(console);
  return console;
}
/**
 *  Return a console reference from a valid console pointer.
 */
inline TCOD_Console& ensure_(TCOD_Console* console)
{
  console = validate_(console);
  if (!console) {
    throw std::logic_error("libtcod has not been initialized yet.");
  }
  return *console;
}
/**
 *  Return a console reference from a valid constant console pointer.
 */
inline const TCOD_Console& ensure_(const TCOD_Console* console)
{
  console = validate_(console);
  if (!console) {
    throw std::logic_error("libtcod has not been initialized yet.");
  }
  return *console;
}
} // namespace console
} // namespace tcod
/**
 *  Validate and return a console.
 */
inline TCOD_Console* TCOD_console_validate_(TCOD_Console* console)
{
  return tcod::console::validate_(console);
}
/**
 *  Validate and return a constant console.
 */
inline const TCOD_Console* TCOD_console_validate_(const TCOD_Console* console)
{
  return tcod::console::validate_(console);
}
/**
 *  Return true if the console is valid and the index is within it.
 */
inline bool TCOD_console_is_index_valid_(const TCOD_Console* console,
                                         int x, int y)
{
  return console && 0 <= x && x < console->w && 0 <= y && y < console->h;
}
TCOD_event_t TCOD_sys_handle_mouse_event(
    const union SDL_Event* ev, TCOD_mouse_t* mouse);
TCOD_event_t TCOD_sys_handle_key_event(
    const union SDL_Event* ev, TCOD_key_t* key);
/**
 *  Private function, gets pointers of an old renderer.
 */
struct SDL_Window* TCOD_sys_get_sdl_window_(void);
/**
 *  Private function, gets pointers of an old renderer.
 */
struct SDL_Renderer* TCOD_sys_get_sdl_renderer_(void);
#endif // __cplusplus
#endif // TCODLIB_INT_H_
