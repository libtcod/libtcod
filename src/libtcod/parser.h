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
#ifndef TCOD_PARSER_H_
#define TCOD_PARSER_H_

#include "color.h"
#include "lex.h"
#include "list.h"
#include "mersenne.h"
#include "portability.h"

#ifdef __cplusplus
extern "C" {
#endif
/* generic type */
typedef enum {
  TCOD_TYPE_NONE,
  TCOD_TYPE_BOOL,
  TCOD_TYPE_CHAR,
  TCOD_TYPE_INT,
  TCOD_TYPE_FLOAT,
  TCOD_TYPE_STRING,
  TCOD_TYPE_COLOR,
  TCOD_TYPE_DICE,
  TCOD_TYPE_VALUELIST00,
  TCOD_TYPE_VALUELIST01,
  TCOD_TYPE_VALUELIST02,
  TCOD_TYPE_VALUELIST03,
  TCOD_TYPE_VALUELIST04,
  TCOD_TYPE_VALUELIST05,
  TCOD_TYPE_VALUELIST06,
  TCOD_TYPE_VALUELIST07,
  TCOD_TYPE_VALUELIST08,
  TCOD_TYPE_VALUELIST09,
  TCOD_TYPE_VALUELIST10,
  TCOD_TYPE_VALUELIST11,
  TCOD_TYPE_VALUELIST12,
  TCOD_TYPE_VALUELIST13,
  TCOD_TYPE_VALUELIST14,
  TCOD_TYPE_VALUELIST15,
  TCOD_TYPE_CUSTOM00,
  TCOD_TYPE_CUSTOM01,
  TCOD_TYPE_CUSTOM02,
  TCOD_TYPE_CUSTOM03,
  TCOD_TYPE_CUSTOM04,
  TCOD_TYPE_CUSTOM05,
  TCOD_TYPE_CUSTOM06,
  TCOD_TYPE_CUSTOM07,
  TCOD_TYPE_CUSTOM08,
  TCOD_TYPE_CUSTOM09,
  TCOD_TYPE_CUSTOM10,
  TCOD_TYPE_CUSTOM11,
  TCOD_TYPE_CUSTOM12,
  TCOD_TYPE_CUSTOM13,
  TCOD_TYPE_CUSTOM14,
  TCOD_TYPE_CUSTOM15,
  TCOD_TYPE_LIST = 1024
} TCOD_value_type_t;

/* generic value */
typedef union {
  bool b;
  char c;
  int32_t i;
  float f;
  char* s;
  TCOD_color_t col;
  TCOD_dice_t dice;
  TCOD_list_t list;
  void* custom;
} TCOD_value_t;

// Forward declarations for callback.
struct TCOD_parser_listener_t;
struct TCOD_ParserStruct;

/* a custom type parser */
typedef TCOD_value_t (*TCOD_parser_custom_t)(
    TCOD_lex_t* lex, struct TCOD_parser_listener_t* listener, struct TCOD_ParserStruct* str, char* propname);

/***************************************************************************
    @brief Parser struct, member variables are for internal use.
 */
typedef struct TCOD_ParserStruct {
  char* name;  // Entity type name.
  TCOD_list_t flags;  // List of flags.
  TCOD_list_t props;  // List of properties (name, type, mandatory)
  TCOD_list_t lists;  // List of value lists.
  TCOD_list_t structs;  // List of sub-structures.
} TCOD_ParserStruct;
typedef struct TCOD_ParserStruct TCOD_struct_int_t;  // Deprecated
typedef struct TCOD_ParserStruct* TCOD_parser_struct_t;  // Deprecated

/***************************************************************************
    @brief Parser, member variables are for internal use.
 */
typedef struct TCOD_Parser {
  TCOD_list_t structs;  // List of structures.
  TCOD_parser_custom_t customs[16];  // List of custom type parsers.
  bool fatal;  // True if a fatal error has occurred.
  TCOD_list_t props;  // List of properties if default listener is used.
} TCOD_Parser;
typedef struct TCOD_Parser TCOD_parser_int_t;  // Deprecated

/* parser structures */
TCODLIB_API const char* TCOD_struct_get_name(const TCOD_ParserStruct* def);
TCODLIB_API void TCOD_struct_add_property(
    TCOD_ParserStruct* def, const char* name, TCOD_value_type_t type, bool mandatory);
TCODLIB_API void TCOD_struct_add_list_property(
    TCOD_ParserStruct* def, const char* name, TCOD_value_type_t type, bool mandatory);
TCODLIB_API void TCOD_struct_add_value_list(
    TCOD_ParserStruct* def, const char* name, const char* const* value_list, bool mandatory);
TCODLIB_API void TCOD_struct_add_value_list_sized(
    TCOD_ParserStruct* def, const char* name, const char* const* value_list, int size, bool mandatory);
TCODLIB_API void TCOD_struct_add_flag(TCOD_ParserStruct* def, const char* propname);
TCODLIB_API void TCOD_struct_add_structure(TCOD_ParserStruct* def, const TCOD_ParserStruct* sub_structure);
TCODLIB_API bool TCOD_struct_is_mandatory(TCOD_ParserStruct* def, const char* propname);
TCODLIB_API TCOD_value_type_t TCOD_struct_get_type(const TCOD_ParserStruct* def, const char* propname);

/* parser listener */
typedef struct TCOD_parser_listener_t {
  bool (*new_struct)(TCOD_ParserStruct* str, const char* name);
  bool (*new_flag)(const char* name);
  bool (*new_property)(const char* propname, TCOD_value_type_t type, TCOD_value_t value);
  bool (*end_struct)(TCOD_ParserStruct* str, const char* name);
  void (*error)(const char* msg);
} TCOD_parser_listener_t;

/* the parser */
struct TCOD_Parser;
typedef struct TCOD_Parser* TCOD_parser_t;

TCODLIB_API TCOD_Parser* TCOD_parser_new(void);
TCODLIB_API TCOD_ParserStruct* TCOD_parser_new_struct(TCOD_Parser* parser, const char* name);
TCODLIB_API TCOD_value_type_t TCOD_parser_new_custom_type(TCOD_Parser* parser, TCOD_parser_custom_t custom_type_parser);
TCODLIB_API void TCOD_parser_run(TCOD_Parser* parser, const char* filename, TCOD_parser_listener_t* listener);
TCODLIB_API void TCOD_parser_delete(TCOD_Parser* parser);
/* error during parsing. can be called by the parser listener */
TCODLIB_FORMAT(1, 2)
TCODLIB_API void TCOD_parser_error(const char* msg, ...);
/* default parser listener */
TCODLIB_API bool TCOD_parser_has_property(TCOD_Parser* parser, const char* name);
TCODLIB_API bool TCOD_parser_get_bool_property(TCOD_Parser* parser, const char* name);
TCODLIB_API int TCOD_parser_get_char_property(TCOD_Parser* parser, const char* name);
TCODLIB_API int TCOD_parser_get_int_property(TCOD_Parser* parser, const char* name);
TCODLIB_API float TCOD_parser_get_float_property(TCOD_Parser* parser, const char* name);
TCODLIB_API const char* TCOD_parser_get_string_property(TCOD_Parser* parser, const char* name);
TCODLIB_API TCOD_color_t TCOD_parser_get_color_property(TCOD_Parser* parser, const char* name);
TCODLIB_API TCOD_dice_t TCOD_parser_get_dice_property(TCOD_Parser* parser, const char* name);
TCODLIB_API void TCOD_parser_get_dice_property_py(TCOD_Parser* parser, const char* name, TCOD_dice_t* dice);
TCODLIB_API void* TCOD_parser_get_custom_property(TCOD_Parser* parser, const char* name);
TCODLIB_API TCOD_list_t TCOD_parser_get_list_property(TCOD_Parser* parser, const char* name, TCOD_value_type_t type);

TCODLIB_API TCOD_value_t TCOD_parse_bool_value(void);
TCODLIB_API TCOD_value_t TCOD_parse_char_value(void);
TCODLIB_API TCOD_value_t TCOD_parse_integer_value(void);
TCODLIB_API TCOD_value_t TCOD_parse_float_value(void);
TCODLIB_API TCOD_value_t TCOD_parse_string_value(void);
TCODLIB_API TCOD_value_t TCOD_parse_color_value(void);
TCODLIB_API TCOD_value_t TCOD_parse_dice_value(void);
TCODLIB_API TCOD_value_t TCOD_parse_value_list_value(TCOD_ParserStruct* def, int list_num);
TCODLIB_API TCOD_value_t
TCOD_parse_property_value(TCOD_Parser* parser, TCOD_ParserStruct* def, char* propname, bool list);
#ifdef __cplusplus
}
#endif
#endif  // TCOD_PARSER_H_
