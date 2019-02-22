
#ifndef LIBTCOD_TILESET_TRUETYPE_H_
#define LIBTCOD_TILESET_TRUETYPE_H_
#ifdef __cplusplus
#include <memory>
#endif // __cplusplus
#include "tileset.h"

#ifdef __cplusplus
namespace tcod {
namespace tileset {
/**
 *  Return a Tileset from a `.ttf` or `.otf` font file.
 */
auto load_truetype(const std::string& path, int tile_width, int tile_height)
-> std::unique_ptr<Tileset>;
} // namespace tileset
} // namespace tcod
#endif // __cplusplus
/**
 *  Set the global tileset from a TrueType font file.
 */
TCODLIB_CAPI int TCOD_tileset_load_truetype_(
    const char* path,
    int tile_width,
    int tile_height);
#endif // LIBTCOD_TILESET_TRUETYPE_H_
