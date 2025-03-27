/* BSD 3-Clause License
 *
 * Copyright © 2008-2025, Jice and the libtcod contributors.
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
#include "logging.h"

#include <stdarg.h>
#include <stdio.h>

#include "version.h"

/***************************************************************************
    @brief The current log level.  Messages below this level are not processed.
 */
static int TCOD_log_level = TCOD_LOG_WARNING;
/***************************************************************************
    @brief Return a readable name for the given log level.
 */
static const char* TCOD_log_level_name(int level) {
  if (level >= TCOD_LOG_CRITICAL) {
    return "CRITICAL";
  } else if (level >= TCOD_LOG_ERROR) {
    return "ERROR";
  } else if (level >= TCOD_LOG_WARNING) {
    return "WARNING";
  } else if (level >= TCOD_LOG_INFO) {
    return "INFO";
  } else if (level >= TCOD_LOG_DEBUG) {
    return "DEBUG";
  }
  return "TRACE";
}
/***************************************************************************
    @brief The default logger used when no other logger is provided, prints the error to stderr.
 */
static void TCOD_logger_default(const TCOD_LogMessage* message, void* userdata) {
  (void)userdata;  // Ignored parameter.
  if (!message) {
    const TCOD_LogMessage bad_log = {
        .message = "Logger has been invoked with a NULL message!",
        .level = TCOD_LOG_ERROR,
        .source = __FILE__,
        .lineno = __LINE__,
    };
    TCOD_logger_default(&bad_log, NULL);
    return;
  }
  if (!message->source) {
    const TCOD_LogMessage no_source = {
        .message = "Message source attribute was NULL!",
        .level = TCOD_LOG_ERROR,
        .source = "",
        .lineno = message->lineno,
    };
    TCOD_logger_default(&no_source, NULL);
    return;
  }
  fprintf(
      stderr, "%s:%d:%s:%s\n", message->source, message->lineno, TCOD_log_level_name(message->level), message->message);
}
/***************************************************************************
    @brief The active logger callback.
 */
static TCOD_LoggingCallback TCOD_logger_current = &TCOD_logger_default;
/***************************************************************************
    @brief User data for the active logger callback.
 */
static void* TCOD_logger_current_userdata = NULL;
/***************************************************************************
    @brief Log a message if it passes the log level.
 */
static void TCOD_log_internal(const TCOD_LogMessage* message) {
  if (!message) return;
  if (message->level < TCOD_log_level) return;
  if (TCOD_logger_current) TCOD_logger_current(message, TCOD_logger_current_userdata);
}
void TCOD_set_log_level(int level) { TCOD_log_level = level; }
void TCOD_set_log_callback(TCOD_LoggingCallback callback, void* userdata) {
  TCOD_logger_current = callback;
  TCOD_logger_current_userdata = userdata;
}
void TCOD_log_verbose_(const char* msg, int level, const char* source, int line) {
  if (!msg) return;
  const TCOD_LogMessage message = {
      .message = msg,
      .level = level,
      .source = source ? source : "",
      .lineno = line,
  };
  TCOD_log_internal(&message);
}
void TCOD_log_verbose_fmt_(int level, const char* source, int line, const char* fmt, ...) {
  if (!fmt) return;
  if (level < TCOD_log_level) return;
  char buffer[4096];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  TCOD_log_verbose_(buffer, level, source, line);
}
