#ifndef _TCOD_RANDOM_H
#define _TCOD_RANDOM_H

typedef void *TCOD_random_t;

TCODLIB_API TCOD_random_t TCOD_random_get_instance();
TCODLIB_API TCOD_random_t TCOD_random_new();
TCODLIB_API TCOD_random_t TCOD_random_new_from_seed(uint32 seed);
TCODLIB_API int TCOD_random_get_int(TCOD_random_t mersenne, int min, int max);
TCODLIB_API float TCOD_random_get_float(TCOD_random_t mersenne, float min, float max);
TCODLIB_API int TCOD_random_get_int_from_byte_array(int min, int max, const char *data,int len);
TCODLIB_API void TCOD_random_delete(TCOD_random_t mersenne);

#endif
