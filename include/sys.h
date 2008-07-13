#ifndef _TCOD_SYS_H
#define _TCOD_SYS_H

TCODLIB_API uint32 TCOD_sys_elapsed_milli();
TCODLIB_API float TCOD_sys_elapsed_seconds();
TCODLIB_API void TCOD_sys_sleep_milli(uint32 val);
TCODLIB_API void TCOD_sys_save_screenshot(const char *filename);
TCODLIB_API void TCOD_sys_force_fullscreen_resolution(int width, int height);
TCODLIB_API void TCOD_sys_set_fps(int val);
TCODLIB_API int TCOD_sys_get_fps();
TCODLIB_API float TCOD_sys_get_last_frame_length();
TCODLIB_API void TCOD_sys_get_current_resolution(int *w, int *h);

#endif
