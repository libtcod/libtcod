#ifndef _TCOD_LIST_H
#define _TCOD_LIST_H

typedef void *TCOD_list_t;

TCODLIB_API TCOD_list_t TCOD_list_new();
TCODLIB_API TCOD_list_t TCOD_list_duplicate(TCOD_list_t l);
TCODLIB_API void TCOD_list_delete(TCOD_list_t l);
TCODLIB_API void TCOD_list_push(TCOD_list_t l, const void * elt);
TCODLIB_API void * TCOD_list_pop(TCOD_list_t l);
TCODLIB_API void * TCOD_list_peek(TCOD_list_t l);
TCODLIB_API void TCOD_list_add_all(TCOD_list_t l, TCOD_list_t l2);
TCODLIB_API void * TCOD_list_get(TCOD_list_t l,int idx);
TCODLIB_API void TCOD_list_set(TCOD_list_t l,const void *elt, int idx);
TCODLIB_API void ** TCOD_list_begin(TCOD_list_t l);
TCODLIB_API void ** TCOD_list_end(TCOD_list_t l);
TCODLIB_API void **TCOD_list_remove_iterator(TCOD_list_t l, void **elt);
TCODLIB_API void TCOD_list_remove(TCOD_list_t l, const void * elt);
TCODLIB_API bool TCOD_list_contains(TCOD_list_t l,const void * elt);
TCODLIB_API void TCOD_list_clear(TCOD_list_t l);
TCODLIB_API void TCOD_list_clear_and_delete(TCOD_list_t l);
TCODLIB_API int TCOD_list_size(TCOD_list_t l);
TCODLIB_API void TCOD_list_insert_before(TCOD_list_t l,const void *elt,int before);
TCODLIB_API bool TCOD_list_is_empty(TCOD_list_t l);

#endif
