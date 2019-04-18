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
#include "globals.h"

#include <cstdlib>

namespace tcod {
namespace engine {
std::shared_ptr<Backend> active_backend = nullptr;
std::shared_ptr<Display> active_display = nullptr;
std::shared_ptr<Tileset> active_tileset = nullptr;
std::shared_ptr<Tilesheet> active_tilesheet = nullptr;

void set_backend(std::shared_ptr<Backend> backend)
{
  active_backend = backend;
}
std::shared_ptr<Backend> get_backend()
{
  return active_backend;
}
void set_display(std::shared_ptr<Display> display)
{
  active_display = display;
}
std::shared_ptr<Display> get_display()
{
  return active_display;
}
void set_tileset(std::shared_ptr<Tileset> tileset)
{
  active_tileset = tileset;
  if (tileset && active_display) {
    active_display->set_tileset(tileset);
  }
}
auto get_tileset() -> std::shared_ptr<Tileset>
{
  return active_tileset;
}
void set_tilesheet(std::shared_ptr<Tilesheet> sheet)
{
  active_tilesheet = sheet;
}
auto get_tilesheet() -> std::shared_ptr<Tilesheet>
{
  return active_tilesheet;
}
} // namespace engine
} // namespace tcod
TCODLIB_CAPI TCOD_Tileset* TCOD_get_default_tileset(void)
{
  return new TCOD_Tileset(tcod::engine::get_tileset());
}
TCODLIB_CAPI void TCOD_set_default_tileset(TCOD_Tileset* tileset)
{
  tcod::engine::set_tileset(tileset ? (*tileset) : nullptr);
}
