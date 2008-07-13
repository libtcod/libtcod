#ifndef _TCOD_TREE_H
#define _TCOD_TREE_H

typedef struct _TCOD_tree_t {
	struct _TCOD_tree_t *next;
	struct _TCOD_tree_t *father;
	struct _TCOD_tree_t *sons;
} TCOD_tree_t;

TCODLIB_API TCOD_tree_t *TCOD_tree_new();
TCODLIB_API void TCOD_tree_add_son(TCOD_tree_t *node, TCOD_tree_t *son);

#endif
