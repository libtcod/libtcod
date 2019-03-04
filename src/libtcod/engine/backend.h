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
#ifndef LIBTCOD_ENGINE_BACKEND_H_
#define LIBTCOD_ENGINE_BACKEND_H_

#ifdef __cplusplus
#include <memory>
#endif /* __cplusplus */

#include "../console_types.h"
#include "../mouse_types.h"
#include "../sys.h"

#ifdef __cplusplus
namespace tcod {
namespace engine {
class Backend {
 public:
  virtual ~Backend() = default;
  /**
   *  Legacy wait for event virtual function.
   */
  virtual TCOD_event_t legacy_wait_for_event(
      int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse, bool flush) = 0;
  /**
   *  Legacy check for event virtual function.
   */
  virtual TCOD_event_t legacy_check_for_event(
      int eventMask, TCOD_key_t *key, TCOD_mouse_t *mouse) = 0;
};
} // namespace engine
} // namespace tcod
#endif /* __cplusplus */
#endif /* LIBTCOD_ENGINE_BACKEND_H_ */
