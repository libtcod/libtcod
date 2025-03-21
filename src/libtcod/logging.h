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
#pragma once
#ifndef LIBTCOD_LOGGING_H_
#define LIBTCOD_LOGGING_H_

#include "config.h"
#include "error.h"

typedef enum TCOD_LogLevel {
  TCOD_LOG_DEBUG = 10,
  TCOD_LOG_INFO = 20,
  TCOD_LOG_WARNING = 30,
  TCOD_LOG_ERROR = 40,
  TCOD_LOG_CRITICAL = 50,
} TCOD_LogLevel;
/***************************************************************************
    @brief Information being logged, this is a temporary object which doesn't last longer than the logging callbacks.

    This struct will not contain NULL character pointers.
    \rst
    .. versionadded:: 1.19
    \endrst
 */
typedef struct TCOD_LogMessage {
  const char* message;  // Message that was logged.
  int level;  // The log level, usually one of TCOD_LogLevel.
  const char* source;  // Source file of the logged message, usually __FILE__.
  int lineno;  // Source line of the logged message, usually __LINE__.
} TCOD_LogMessage;
/***************************************************************************
    @brief A callback for logger listeners.
 */
typedef void (*TCOD_LoggingCallback)(const TCOD_LogMessage* message, void* userdata);
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
/***************************************************************************
    @brief Set the level of messages being logged.

    @param level Should be one of the levels from TCOD_LogLevel.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCODLIB_API void TCOD_set_log_level(int level);
/***************************************************************************
    @brief Sets a callback for libtcod's logged output.

    @param callback A `TCOD_LoggingCallback` function which processes `TCOD_LogMessage*` parameters.
        Can be NULL to disable logging.
    @param userdata Userdata to be passed to the log function.

    \rst
    .. versionadded:: 1.19
    \endrst
 */
TCODLIB_API void TCOD_set_log_callback(TCOD_LoggingCallback callback, void* userdata);
/***************************************************************************
    @brief Log a message without formatting.  This function is provisional.

    @param msg The message to log.
    @param level The log level of this message.
    @param source The source file of this message, should be __FILE__, may be NULL.
    @param line The source line of this message, should be __LINE__.
 */
TCODLIB_API void TCOD_log_verbose_(const char* msg, int level, const char* source, int line);
/***************************************************************************
    @brief Log a formatted message.  This function is provisional.

    @param level The log level of this message.
    @param source The source file of this message, should be __FILE__, may be NULL.
    @param line The source line of this message, should be __LINE__.
    @param fmt The message format string.
    @param ... Variable arguments for the format.
 */
TCODLIB_FORMAT(4, 5)
TCODLIB_API void TCOD_log_verbose_fmt_(int level, const char* source, int line, const char* fmt, ...);

// All of these are for internal use.
#define TCOD_log_debug_f(fmt, ...) TCOD_log_verbose_fmt_(TCOD_LOG_DEBUG, __FILE__, __LINE__, (fmt), __VA_ARGS__)
#define TCOD_log_info_f(fmt, ...) TCOD_log_verbose_fmt_(TCOD_LOG_INFO, __FILE__, __LINE__, (fmt), __VA_ARGS__)
#define TCOD_log_warning_f(fmt, ...) TCOD_log_verbose_fmt_(TCOD_LOG_WARNING, __FILE__, __LINE__, (fmt), __VA_ARGS__)
#define TCOD_log_error_f(fmt, ...) TCOD_log_verbose_fmt_(TCOD_LOG_ERROR, __FILE__, __LINE__, (fmt), __VA_ARGS__)
#define TCOD_log_critical_f(fmt, ...) TCOD_log_verbose_fmt_(TCOD_LOG_CRITICAL, __FILE__, __LINE__, (fmt), __VA_ARGS__)
#define TCOD_log_debug(msg) TCOD_log_verbose_((msg), TCOD_LOG_DEBUG, __FILE__, __LINE__)
#define TCOD_log_info(msg) TCOD_log_verbose_((msg), TCOD_LOG_INFO, __FILE__, __LINE__)
#define TCOD_log_warning(msg) TCOD_log_verbose_((msg), TCOD_LOG_WARNING, __FILE__, __LINE__)
#define TCOD_log_error(msg) TCOD_log_verbose_((msg), TCOD_LOG_ERROR, __FILE__, __LINE__)
#define TCOD_log_critical(msg) TCOD_log_verbose_((msg), TCOD_LOG_CRITICAL, __FILE__, __LINE__)
#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
#endif  // LIBTCOD_LOGGING_H_
