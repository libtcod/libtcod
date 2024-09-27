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
#ifndef LIBTCODFOV_ERROR_H_
#define LIBTCODFOV_ERROR_H_
#include "config.h"
#include "version.h"
/**
 *  An enum of libtcod error codes.
 *
 *  On values other than `TCODFOV_E_OK` you can use `TCODFOV_get_error()` to learn
 *  more information.
 *  \rst
 *  .. versionadded:: 1.16
 *  \endrst
 */
typedef enum TCODFOV_Error {
  /**
   *  The function completed successfully without issues.
   *
   *  A function is successful when `(err >= 0)`.  Positive values may be used
   *  for warnings, or for other outputs.
   */
  TCODFOV_E_OK = 0,
  /**
   *  The error code for generic runtime errors.
   *
   *  The returned code my be changed in the future to something more specific.
   *  Use `(err < 0)` to check if the value is an error.
   */
  TCODFOV_E_ERROR = -1,
  /**
   *  The function failed because a given input argument was invalid.
   */
  TCODFOV_E_INVALID_ARGUMENT = -2,
  /**
   *  The function failed because it was unable to allocate enough memory.
   */
  TCODFOV_E_OUT_OF_MEMORY = -3,
  /**
      This function needs additional attention, but is otherwise functioning
      correctly.  See its documentation.
      \rst
      .. versionadded:: 1.16
      \endrst
   */
  TCODFOV_E_REQUIRES_ATTENTION = -4,
  /**
   *  The function completed, but a minor issue was detected.
   */
  TCODFOV_E_WARN = 1,
} TCODFOV_Error;
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
/***************************************************************************
    @brief Return the last error message.  If there is no error then the string will have a length of zero.

    \rst
    .. versionadded:: 1.12
    \endrst
 */
TCODFOV_NODISCARD
TCODFOV_PUBLIC const char* TCODFOV_get_error(void);
/***************************************************************************
    @brief Set an error message and return TCODFOV_E_ERROR.

    \rst
    .. versionadded:: 1.12
    \endrst
 */
TCODFOV_PUBLIC TCODFOV_Error TCODFOV_set_error(const char* msg);
/**
 *  Set an error message and return TCODFOV_E_ERROR.
 *  \rst
 *  .. versionadded:: 1.16
 *  \endrst
 */
TCODFOV_FORMAT(1, 2)
TCODFOV_PUBLIC TCODFOV_Error TCODFOV_set_errorf(const char* fmt, ...);
/**
 *  Clear a current existing error message.
 *  \rst
 *  .. versionadded:: 1.16
 *  \endrst
 */
TCODFOV_PUBLIC void TCODFOV_clear_error(void);
/**
 *  Set an error with version, file, and line info added to the output.
 *
 *  Used internally.
 */
#define TCODFOV_set_errorv(msg) TCODFOV_set_errorf("%s:%i\n%s", TCODFOV_STRVERSIONNAME " " __FILE__, __LINE__, (msg))
/**
 *  Format an error with version, file, and line info added to the output.
 *
 *  Used internally.
 */
#define TCODFOV_set_errorvf(fmt, ...) \
  TCODFOV_set_errorf("%s:%i\n" fmt, TCODFOV_STRVERSIONNAME " " __FILE__, __LINE__, __VA_ARGS__)
#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
#endif  // LIBTCODFOV_ERROR_H_
