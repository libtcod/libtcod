#ifndef TCOD_CONSOLE_PRINTING_H_
#define TCOD_CONSOLE_PRINTING_H_

#include "../portability.h"
#include "../console.h"

#ifdef __cplusplus
extern "C" {
#endif
TCOD_DEPRECATED("Use TCOD_console_printf instead.")
TCODLIB_API void TCOD_console_print(TCOD_Console* con,int x, int y, const char *fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_ex instead.")
TCODLIB_API void TCOD_console_print_ex(TCOD_Console* con,int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char *fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_rect instead.")
TCODLIB_API int TCOD_console_print_rect(TCOD_Console* con,int x, int y, int w, int h, const char *fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_rect_ex instead.")
TCODLIB_API int TCOD_console_print_rect_ex(TCOD_Console* con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char *fmt, ...);
TCODLIB_API int TCOD_console_get_height_rect(TCOD_Console* con,int x, int y, int w, int h, const char *fmt, ...);

#ifndef NO_UNICODE
TCOD_DEPRECATED("Use TCOD_console_printf instead.")
TCODLIB_API void TCOD_console_print_utf(TCOD_Console* con,int x, int y, const wchar_t *fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_ex instead.")
TCODLIB_API void TCOD_console_print_ex_utf(TCOD_Console* con,int x, int y, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const wchar_t *fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_rect instead.")
TCODLIB_API int TCOD_console_print_rect_utf(TCOD_Console* con,int x, int y, int w, int h, const wchar_t *fmt, ...);
TCOD_DEPRECATED("Use TCOD_console_printf_rect_ex instead.")
TCODLIB_API int TCOD_console_print_rect_ex_utf(TCOD_Console* con,int x, int y, int w, int h, TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const wchar_t *fmt, ...);
TCODLIB_API int TCOD_console_get_height_rect_utf(TCOD_Console* con,int x, int y, int w, int h, const wchar_t *fmt, ...);
#endif

/* UTF-8 functions */
TCODLIB_API void TCOD_console_printf(TCOD_Console* con, int x, int y,
                                     const char *fmt, ...);
TCODLIB_API void TCOD_console_printf_ex(
    TCOD_Console* con, int x, int y, TCOD_bkgnd_flag_t flag,
    TCOD_alignment_t alignment, const char *fmt, ...);
TCODLIB_API int TCOD_console_printf_rect(
    TCOD_Console* con, int x, int y, int w, int h, const char *fmt, ...);
TCODLIB_API int TCOD_console_printf_rect_ex(
    TCOD_Console* con,int x, int y, int w, int h,
    TCOD_bkgnd_flag_t flag, TCOD_alignment_t alignment, const char *fmt, ...);
TCODLIB_API
void TCOD_console_printf_frame(struct TCOD_Console *con,
                               int x, int y, int w, int h, int empty,
                               TCOD_bkgnd_flag_t flag, const char *fmt, ...);
TCODLIB_API
int TCOD_console_get_height_rect_fmt(struct TCOD_Console *con,
                                     int x, int y, int w, int h,
                                     const char *fmt, ...);

/* Private internal functions. */
int TCOD_console_print_internal_utf8_(
    TCOD_Console* con, int x, int y, int max_width, int max_height,
    TCOD_bkgnd_flag_t flag, TCOD_alignment_t align,
    const unsigned char *string, int can_split, int count_only);
#ifdef __cplusplus
}
#endif

#endif /* TCOD_CONSOLE_PRINTING_H_ */
