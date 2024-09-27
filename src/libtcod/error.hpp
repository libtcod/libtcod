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
#pragma once
#ifndef LIBTCODFOV_ERROR_HPP_
#define LIBTCODFOV_ERROR_HPP_

#include <exception>
#include <stdexcept>
#include <string>

#include "error.h"
namespace tcod::fov {
/**
 *  Set an error message and return a relevant error code, usually -1.
 *
 *  Used internally.
 */
inline TCODFOV_Error set_error(const std::string& msg) { return TCODFOV_set_errorv(msg.c_str()); }
inline TCODFOV_Error set_error(const std::exception& e) { return TCODFOV_set_errorv(e.what()); }
/**
 *  Check and throw error messages.
 *
 *  Used internally.
 */
inline int check_throw_error(int error) {
  if (error >= 0) {
    return error;
  }
  std::string error_msg = TCODFOV_get_error();
  switch (error) {
    case TCODFOV_E_ERROR:
    default:
      throw std::runtime_error(error_msg);
      break;
    case TCODFOV_E_INVALID_ARGUMENT:
      throw std::invalid_argument(error_msg);
      break;
  }
}
inline TCODFOV_Error check_throw_error(TCODFOV_Error error) {
  return static_cast<TCODFOV_Error>(check_throw_error(static_cast<int>(error)));
}
}  // namespace tcod::fov
#endif  // LIBTCODFOV_ERROR_HPP_
