/* BSD 3-Clause License
 *
 * Copyright © 2008-2023, Jice and the libtcod contributors.
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
#include "fov.hpp"

TCODMap::TCODMap(int width, int height) { data = TCODFOV_map_new(width, height); }

void TCODMap::clear(bool transparent, bool walkable) { TCODFOV_map_clear(data, transparent, walkable); }

void TCODMap::setProperties(int x, int y, bool isTransparent, bool isWalkable) {
  TCODFOV_map_set_properties(data, x, y, isTransparent, isWalkable);
}

void TCODMap::copy(const TCODMap* source) { TCODFOV_map_copy(source->data, data); }

void TCODMap::computeFov(int x, int y, int maxRadius, bool light_walls, TCODFOV_fov_algorithm_t algo) {
  TCODFOV_map_compute_fov(data, x, y, maxRadius, light_walls, algo);
}

bool TCODMap::isInFov(int x, int y) const { return TCODFOV_map_is_in_fov(data, x, y) != 0; }

void TCODMap::setInFov(int x, int y, bool fov) { TCODFOV_map_set_in_fov(data, x, y, fov); }

bool TCODMap::isTransparent(int x, int y) const { return TCODFOV_map_is_transparent(data, x, y) != 0; }

bool TCODMap::isWalkable(int x, int y) const { return TCODFOV_map_is_walkable(data, x, y) != 0; }

int TCODMap::getWidth() const { return TCODFOV_map_get_width(data); }

int TCODMap::getHeight() const { return TCODFOV_map_get_height(data); }

int TCODMap::getNbCells() const { return TCODFOV_map_get_nb_cells(data); }

TCODMap::~TCODMap() { TCODFOV_map_delete(data); }
