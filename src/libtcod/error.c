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
#include "error.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "logging.h"

// Maximum error length in bytes.
#define MAX_ERROR_LENGTH 1024
// Current error message.
static char error_msg_[MAX_ERROR_LENGTH] = "";

const char* TCODFOV_get_error(void) { return error_msg_; }
TCODFOV_Error TCODFOV_set_error(const char* msg) {
  strncpy(error_msg_, msg, sizeof(error_msg_) - 1);
  TCODFOV_log_error(msg);
  return TCODFOV_E_ERROR;
}
TCODFOV_Error TCODFOV_set_errorf(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(error_msg_, sizeof(error_msg_), fmt, ap);
  va_end(ap);
  TCODFOV_log_error(error_msg_);
  return TCODFOV_E_ERROR;
}
void TCODFOV_clear_error(void) { error_msg_[0] = '\0'; }
