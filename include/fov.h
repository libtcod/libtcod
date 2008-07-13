#ifndef _TCOD_FOV_H
#define _TCOD_FOV_H

typedef void *TCOD_map_t;

// allocate a new map
TCODLIB_API TCOD_map_t TCOD_map_new(int width, int height);
// set all cells as solid rock (cannot see through nor walk)
TCODLIB_API void TCOD_map_clear(TCOD_map_t map);
// change a cell properties
TCODLIB_API void TCOD_map_set_properties(TCOD_map_t map, int x, int y, bool is_transparent, bool is_walkable);
// destroy a map
TCODLIB_API void TCOD_map_delete(TCOD_map_t map);
// calculate the field of view (potentially visible cells from player_x,player_y)
TCODLIB_API void TCOD_map_compute_fov(TCOD_map_t map, int player_x, int player_y, int max_radius);
// check if a cell is in the last computed field of view
TCODLIB_API bool TCOD_map_is_in_fov(TCOD_map_t map, int x, int y);
// retrieve properties from the map
TCODLIB_API bool TCOD_map_is_transparent(TCOD_map_t map, int x, int y);
TCODLIB_API bool TCOD_map_is_walkable(TCOD_map_t map, int x, int y);
#endif
