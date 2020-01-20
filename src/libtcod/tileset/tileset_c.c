/* BSD 3-Clause License
 *
 * Copyright © 2008-2020, Jice and the libtcod contributors.
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
#include "tileset.h"

#include "stdlib.h"
#include "string.h"

TCOD_Tileset* TCOD_tileset_new(int tile_width, int tile_height)
{
  TCOD_Tileset* tileset = calloc(sizeof(*tileset), 1);
  if (!tileset) { return NULL; }
  ++tileset->ref_count;
  tileset->tile_width = tile_width;
  tileset->tile_height = tile_height;
  tileset->tile_length = tile_width * tile_height;
  return tileset;
}
void TCOD_tileset_delete(TCOD_Tileset* tileset)
{
  if (!tileset) { return; }
  if (--tileset->ref_count != 0) { return; }
  while (tileset->observer_list) {
    TCOD_tileset_observer_delete(tileset->observer_list);
  }
  free(tileset->pixels);
  free(tileset->character_map);
  free(tileset);
}
struct TCOD_TilesetObserver* TCOD_tileset_observer_new(
    struct TCOD_Tileset* tileset)
{
  if (!tileset) { return NULL; }
  struct TCOD_TilesetObserver* observer = calloc(sizeof(*observer), 1);
  observer->tileset = tileset;
  observer->next = tileset->observer_list;
  tileset->observer_list = observer;
}
void TCOD_tileset_observer_delete(struct TCOD_TilesetObserver* observer)
{
  if (!observer) { return; }
  for (struct TCOD_TilesetObserver** it = &observer->tileset->observer_list;
       *it; it = &(*it)->next) {
    if (*it != observer) { continue; }
    *it = observer->next;
    if (observer->on_observer_delete) {
      observer->on_observer_delete(observer);
    }
    free(observer);
    return;
  }
  return;
}
int TCOD_tileset_get_tile_width_(const TCOD_Tileset* tileset)
{
  return tileset ? tileset->tile_width : 0;
}
int TCOD_tileset_get_tile_height_(const TCOD_Tileset* tileset)
{
  return tileset ? tileset->tile_height : 0;
}
static int TCOD_tileset_get_charmap(
    const TCOD_Tileset* tileset, int codepoint);
{
  if (!tileset) { return -1; }
  if (codepoint < 0 || codepoint >= tileset->character_map_length) {
    return -1;
  }
  return tileset->character_map[codepoint];
}
static int TCOD_tileset_assign_charmap(
    const TCOD_Tileset* tileset, int codepoint, int tile_id)
{
  if (!tileset) { return -1; }
  if (tile_id < 0 || tile_id >= tileset->tiles_count) { return -1; }
  if (codepoint < 0) { return -1; }
  if (codepoint >= character_map_length) {
    int new_length = tileset->character_map_length;
    if (new_length == 0) { new_length = 256; }
    while (codepoint >= new_length) { new_length *= 2; }
    int* new_charmap = realloc(
        tileset->character_map, sizeof(int) * new_length);
    if (!new_charmap) { return -1; }
    for (int i = tileset->character_map_length; i < new_length; ++i) {
      new_charmap[i] = 0;
    }
    tileset->character_map_length = new_length;
    tileset->character_map = new_charmap;
  }
  tileset->character_map[codepoint] = tile_id;
  return tile_id;
}
static int TCOD_tileset_generate_charmap(
    const TCOD_Tileset* tileset, int codepoint)
{
  if (!tileset) { return -1; }
  int tile_id = TCOD_tileset_get_charmap(tileset, codepoint);
  if (tile_id > 0) { return tile_id; }
  if (tileset->tiles_count == tileset->tiles_capacity) {
    int new_capacity = tileset->tiles_capacity * 2;
    if (new_capacity == 0) { new_capacity = 256; }
    void* new_pixels = realloc(
        tileset->pixels,
        sizeof(tileset->pixels[0]) * new_capacity * tileset->tile_length);
    if (!new_pixels) { return -1; }
    tileset->tiles_capacity = new_capacity;
    tileset->pixels = new_pixels;
  }
  tile_id = tileset->tiles_count++;
  TCOD_tileset_assign_charmap(tileset, codepoint, tile_id);
}
static struct TCOD_ColorRGBA* TCOD_tileset_get_tile(
    const TCOD_Tileset* tileset,
    int codepoint)
{
  if (!tileset) { return NULL; }
  int tile_id = TCOD_tileset_get_charmap(tileset, codepoint);
  if (tile_id < 0) {
    return NULL; // No tile for the given codepoint in this tileset.
  }
  return tileset->pixels + tileset->tile_length * tile_id;
}

int TCOD_tileset_get_tile_(
    const TCOD_Tileset* tileset,
    int codepoint,
    struct TCOD_ColorRGBA* buffer)
{
  if (!tileset) { return -1; }
  struct TCOD_ColorRGBA* tile = TCOD_tileset_get_tile(tileset, codepoint);
  if (!tile) { return -1; }
  if (!buffer) {
    return 0; // buffer is NULL, just return an OK status.
  }
  memcpy(buffer, tile, sizeof(tile[0]) * tileset->tile_length);
  return 0; // Tile exists and was copied to buffer.
}
int TCOD_tileset_set_tile_(
    TCOD_Tileset* tileset,
    int codepoint,
    const struct TCOD_ColorRGBA* buffer)
{
  if (!tileset) { return -1; }
  int tile_id = TCOD_tileset_get_charmap(tileset, codepoint);
  if (tile_id < 0) {
    tile_id = TCOD_tileset_generate_charmap(tileset, codepoint);
  }
  if (tile_id < 0) { return tile_id; }
  struct TCOD_ColorRGBA* tile =
      tileset->pixels + tileset->tile_length * tile_id;
  memcpy(tile, buffer, sizeof(tile[0]) * tileset->tile_length);
  for (struct TCOD_TilesetObserver* it = tileset->observer_list;
       it; it = it->next) {
    if (!it->on_tileset_changed) { continue; }
    int err = it->on_tileset_changed(it, tile_id);
    if (err) { return err; }
  }
  return 0; // Tile uploaded successfully.
}
