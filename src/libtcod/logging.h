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
#ifndef LIBTCODFOV_LOGGING_H_
#define LIBTCODFOV_LOGGING_H_

#include "config.h"
#include "error.h"

typedef enum TCODFOV_LogLevel {
  TCODFOV_log_DEBUG = 10,
  TCODFOV_log_INFO = 20,
  TCODFOV_log_WARNING = 30,
  TCODFOV_log_ERROR = 40,
  TCODFOV_log_CRITICAL = 50,
} TCODFOV_LogLevel;
/***************************************************************************
    @brief Information being logged, this is a temporary object which doesn't last longer than the logging callbacks.

    This struct will not contain NULL character pointers.
    \rst
    .. versionadded:: 1.19
    \endrst
 */
typedef struct TCODFOV_LogMessage {
  const char* message;  // Message that was logged.
  int level;  // The log level, usually one of TCODFOV_LogLevel.
  const char* source;  // Source file of the logged message, usually __FILE__.
  int lineno;  // Source line of the logged message, usually __LINE__.
} TCODFOV_LogMessage;
/***************************************************************************
    @brief A callback for logger listeners.
 */
typedef void (*TCODFOV_LoggingCallback)(const TCODFOV_LogMessage* message, void* userdata);
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
/***************************************************************************
    @brief Set the level of messages being logged.

    @param level Should be one of the levels from TCODFOV_LogLevel.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCODFOV_PUBLIC void TCODFOV_set_log_level(int level);
/***************************************************************************
    @brief Sets a callback for libtcod's logged output.

    @param callback A `TCODFOV_LoggingCallback` function which processes `TCODFOV_LogMessage*` parameters.
        Can be NULL to disable logging.
    @param userdata Userdata to be passed to the log function.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCODFOV_PUBLIC void TCODFOV_set_log_callback(TCODFOV_LoggingCallback callback, void* userdata);
/***************************************************************************
    @brief Log a message without formatting.  This function is provisional.

    @param msg The message to log.
    @param level The log level of this message.
    @param source The source file of this message, should be __FILE__, may be NULL.
    @param line The source line of this message, should be __LINE__.
 */
TCODFOV_PUBLIC void TCODFOV_log_verbose_(const char* msg, int level, const char* source, int line);
/***************************************************************************
    @brief Log a formatted message.  This function is provisional.

    @param level The log level of this message.
    @param source The source file of this message, should be __FILE__, may be NULL.
    @param line The source line of this message, should be __LINE__.
    @param fmt The message format string.
    @param ... Variable arguments for the format.
 */
TCODFOV_FORMAT(4, 5)
TCODFOV_PUBLIC void TCODFOV_log_verbose_fmt_(int level, const char* source, int line, const char* fmt, ...);

// All of these are for internal use.
#define TCODFOV_log_debug_f(fmt, ...) \
  TCODFOV_log_verbose_fmt_(TCODFOV_log_DEBUG, __FILE__, __LINE__, (fmt), __VA_ARGS__)
#define TCODFOV_log_info_f(fmt, ...) TCODFOV_log_verbose_fmt_(TCODFOV_log_INFO, __FILE__, __LINE__, (fmt), __VA_ARGS__)
#define TCODFOV_log_warning_f(fmt, ...) \
  TCODFOV_log_verbose_fmt_(TCODFOV_log_WARNING, __FILE__, __LINE__, (fmt), __VA_ARGS__)
#define TCODFOV_log_error_f(fmt, ...) \
  TCODFOV_log_verbose_fmt_(TCODFOV_log_ERROR, __FILE__, __LINE__, (fmt), __VA_ARGS__)
#define TCODFOV_log_critical_f(fmt, ...) \
  TCODFOV_log_verbose_fmt_(TCODFOV_log_CRITICAL, __FILE__, __LINE__, (fmt), __VA_ARGS__)
#define TCODFOV_log_debug(msg) TCODFOV_log_verbose_((msg), TCODFOV_log_DEBUG, __FILE__, __LINE__)
#define TCODFOV_log_info(msg) TCODFOV_log_verbose_((msg), TCODFOV_log_INFO, __FILE__, __LINE__)
#define TCODFOV_log_warning(msg) TCODFOV_log_verbose_((msg), TCODFOV_log_WARNING, __FILE__, __LINE__)
#define TCODFOV_log_error(msg) TCODFOV_log_verbose_((msg), TCODFOV_log_ERROR, __FILE__, __LINE__)
#define TCODFOV_log_critical(msg) TCODFOV_log_verbose_((msg), TCODFOV_log_CRITICAL, __FILE__, __LINE__)
#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
#endif  // LIBTCODFOV_LOGGING_H_
