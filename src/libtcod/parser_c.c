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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libtcod_int.h"
#include "parser.h"

#define MAX_NAME_LEN 128

/* definition property */
typedef struct TCOD_struct_prop_t {
  char* name;
  TCOD_value_type_t value;
  bool mandatory;
} TCOD_struct_prop_t;

static TCOD_lex_t* lex = NULL;

static const char* symbols[] = {"{", "}", "=", "/", "+", "-", "[", "]", ",", "#", NULL};

static const char* keywords[] = {"struct", "bool", "char", "int", "float", "string", "color", "dice", NULL};

static TCOD_parser_listener_t* listener = NULL;

static bool default_new_struct(TCOD_ParserStruct* str, const char* name);
static bool default_new_flag(const char* name);
static bool default_new_property(const char* propname, TCOD_value_type_t type, TCOD_value_t value);
static bool default_end_struct(TCOD_ParserStruct* str, const char* name);
static void default_error(const char* msg);

static TCOD_parser_listener_t default_listener = {
    default_new_struct, default_new_flag, default_new_property, default_end_struct, default_error};

static bool string_copy(char* dest, const char* source, int len) {
  if (source == NULL) return false;
  strncpy(dest, source, len - 1);
  dest[len - 1] = '\0';
  if (strlen(source) >= (unsigned)len) return false;
  return true;
}

void TCOD_parser_error(const char* msg, ...) {
  va_list ap;
  va_start(ap, msg);
  char error_partial[2048] = "";
  vsnprintf(error_partial, sizeof(error_partial), msg, ap);
  va_end(ap);
  char error_final[4096] = "";
  snprintf(error_final, sizeof(error_final), "error in %s line %d : %s", lex->filename, lex->file_line, error_partial);
  listener->error(error_final);
  lex->token_type = TCOD_LEX_ERROR;
}

const char* TCOD_struct_get_name(const TCOD_ParserStruct* def) { return def->name; }

/* add a property to an entity definition */
void TCOD_struct_add_property(TCOD_ParserStruct* def, const char* name, TCOD_value_type_t type, bool mandatory) {
  TCOD_struct_prop_t* prop = calloc(1, sizeof(*prop));
  prop->name = TCOD_strdup(name);
  prop->value = type;
  prop->mandatory = mandatory;
  TCOD_list_push(def->props, prop);
}

/* add a list property to an entity definition */
void TCOD_struct_add_list_property(TCOD_ParserStruct* def, const char* name, TCOD_value_type_t type, bool mandatory) {
  TCOD_struct_add_property(def, name, type | TCOD_TYPE_LIST, mandatory);
}

void TCOD_struct_add_value_list_sized(
    TCOD_ParserStruct* def, const char* name, const char* const* value_list, int size, bool mandatory) {
  /* duplicate value list to avoid issues with C# garbage collector */
  const int type = TCOD_TYPE_VALUELIST00 + TCOD_list_size(def->lists);

  char** newArray = NULL;
  if (size) newArray = calloc(size + 1, sizeof(*newArray));

  for (int i = 0; i < size; ++i) newArray[i] = TCOD_strdup(value_list[i]);
  newArray[size] = NULL;

  TCOD_struct_add_property(def, name, type, mandatory);
  TCOD_list_push(def->lists, newArray);
}

/* add a value-list property to an entity definition */
void TCOD_struct_add_value_list(
    TCOD_ParserStruct* def, const char* name, const char* const* value_list, bool mandatory) {
  int size = 0;
  if (value_list) {
    while (value_list[size] != NULL) {
      ++size;
    };
  }
  TCOD_struct_add_value_list_sized(def, name, value_list, size, mandatory);
}

/* add a flag (simplified bool value) to an entity definition */
/* a flag cannot be mandatory. if present => true, if omitted => false */
void TCOD_struct_add_flag(TCOD_ParserStruct* def, const char* propname) {
  TCOD_list_push(def->flags, TCOD_strdup(propname));
}

/* add a sub-entity to an entity definition */
void TCOD_struct_add_structure(TCOD_ParserStruct* def, const TCOD_ParserStruct* sub_definition) {
  TCOD_list_push(def->structs, sub_definition);
}

/* check if given property is mandatory for this entity definition */
bool TCOD_struct_is_mandatory(TCOD_ParserStruct* def, const char* propname) {
  for (TCOD_struct_prop_t** i_prop = (void*)TCOD_list_begin(def->props); i_prop != (void*)TCOD_list_end(def->props);
       i_prop++) {
    if (strcmp((*i_prop)->name, propname) == 0) return (*i_prop)->mandatory;
  }
  return false;
}

/* returns the type of given property */
/* NONE if the property does not exist */
TCOD_value_type_t TCOD_struct_get_type(const TCOD_ParserStruct* def, const char* propname) {
  for (const TCOD_struct_prop_t** i_prop = (void*)TCOD_list_begin(def->props);
       i_prop != (void*)TCOD_list_end(def->props);
       i_prop++) {
    if (strcmp((*i_prop)->name, propname) == 0) return (*i_prop)->value;
  }
  for (const char** iflag = (void*)TCOD_list_begin(def->flags); iflag != (void*)TCOD_list_end(def->flags); iflag++) {
    if (strcmp((*iflag), propname) == 0) return TCOD_TYPE_BOOL;
  }
  return TCOD_TYPE_NONE;
}

TCOD_value_t TCOD_parse_bool_value(void) {
  if (strcmp(lex->tok, "true") == 0) return (TCOD_value_t){.b = true};
  if (strcmp(lex->tok, "false") == 0) return (TCOD_value_t){.b = false};
  TCOD_parser_error("parseBoolValue : unknown value %s for bool. 'true' or 'false' expected", lex->tok);
  return (TCOD_value_t){.b = false};
}

TCOD_value_t TCOD_parse_char_value(void) {
  if (lex->token_type != TCOD_LEX_CHAR && lex->token_type != TCOD_LEX_INTEGER) {
    TCOD_parser_error("parseCharValue : char constant expected instead of '%s'", lex->tok);
  }
  return (TCOD_value_t){.c = (char)lex->token_int_val};
}

TCOD_value_t TCOD_parse_integer_value(void) {
  if (lex->token_type != TCOD_LEX_INTEGER)
    TCOD_parser_error("parseIntegerValue : integer constant expected instead of '%s'", lex->tok);
  return (TCOD_value_t){.i = lex->token_int_val};
}

TCOD_value_t TCOD_parse_float_value(void) {
  if (lex->token_type != TCOD_LEX_FLOAT && lex->token_type != TCOD_LEX_INTEGER)
    TCOD_parser_error("parseFloatValue : float constant expected instead of '%s'", lex->tok);
  return (TCOD_value_t){.f = lex->token_type == TCOD_LEX_FLOAT ? lex->token_float_val : (float)lex->token_int_val};
}

TCOD_value_t TCOD_parse_string_value(void) {
  TCOD_list_t l = TCOD_list_new();
  if (lex->token_type != TCOD_LEX_STRING)
    TCOD_parser_error("parseStringValue : string constant expected instead of '%s'", lex->tok);
  while (true) {
    TCOD_lex_t save;
    TCOD_list_push(l, TCOD_strdup(lex->tok));
    TCOD_lex_savepoint(lex, &save);
    if (TCOD_lex_parse(lex) != TCOD_LEX_STRING) {
      TCOD_lex_restore(lex, &save);
      break;
    }
  }
  size_t slen = 0;
  for (char** s = (void*)TCOD_list_begin(l); s != (void*)TCOD_list_end(l); ++s) {
    slen += strlen(*s);
  }
  TCOD_value_t ret = {.s = calloc(slen + 1, sizeof(*ret.s))};
  if (!ret.s) TCOD_parser_error("parseStringValue : out of memory allocating string of length %ld.", slen + 1);
  for (char** s = (void*)TCOD_list_begin(l); s != (void*)TCOD_list_end(l); ++s) {
    if (ret.s) strcat(ret.s, *s);
    free(*s);
  }
  TCOD_list_delete(l);
  return ret;
}

TCOD_value_t TCOD_parse_color_value(void) {
  TCOD_value_t ret = {.col = {0, 0, 0}};
  if (lex->token_type == TCOD_LEX_SYMBOL && lex->tok[0] == '#') {
    char tmp[128] = "";
    int tok = TCOD_lex_parse(lex);
    /* format : col = #FFFFFF */
    strcat(tmp, "#");
    if (tok == TCOD_LEX_IDEN || tok == TCOD_LEX_INTEGER) {
      strcat(tmp, lex->tok);
      strcpy(lex->tok, tmp);
      if (strlen(lex->tok) < 7 && tok == TCOD_LEX_INTEGER) {
        /* special case of #12AABB => symbol # +
                integer 12 + iden AABB */
        tok = TCOD_lex_parse(lex);
        if (tok == TCOD_LEX_IDEN) {
          strcat(tmp, lex->tok);
          strcpy(lex->tok, tmp);
        }
      }
      lex->token_type = TCOD_LEX_STRING;
    }
  }
  if (lex->token_type != TCOD_LEX_STRING)
    TCOD_parser_error("parseColorValue : string constant expected instead of '%s'", lex->tok);
  if (lex->tok[0] == '#') {
    if (strlen(lex->tok) != 7)
      TCOD_parser_error("parseColorValue : bad color format. '#rrggbb' expected instead of '%s'", lex->tok);
    /* web format : #rrggbb */
    const int r = (TCOD_lex_hextoint(lex->tok[1]) << 4) + TCOD_lex_hextoint(lex->tok[2]);
    const int g = (TCOD_lex_hextoint(lex->tok[3]) << 4) + TCOD_lex_hextoint(lex->tok[4]);
    const int b = (TCOD_lex_hextoint(lex->tok[5]) << 4) + TCOD_lex_hextoint(lex->tok[6]);
    ret.col.r = (uint8_t)r;
    ret.col.g = (uint8_t)g;
    ret.col.b = (uint8_t)b;
  } else {
    /* standard format : rrr,ggg,bbb */
    char* begin = lex->tok;
    char* end = strchr(begin, ',');
    bool ok = true;
    if (!end)
      ok = false;
    else {
      *end = 0;
      ret.col.r = (uint8_t)atoi(begin);
      begin = end + 1;
      end = strchr(begin, ',');
      if (!end)
        ok = false;
      else {
        ret.col.g = (uint8_t)atoi(begin);
        begin = end + 1;
        ret.col.b = (uint8_t)atoi(begin);
      }
    }
    if (!ok) TCOD_parser_error("parseColorValue : bad color format 'rrr,ggg,bbb' expected instead of '%s'", lex->tok);
  }
  return ret;
}

TCOD_value_t TCOD_parse_dice_value(void) {
  /* dice format : [<m>(x|*)]<n>(D|d)<f>[(+|-)<a>] */
  TCOD_value_t ret = {.dice = {.multiplier = 1.0f}};
  char* begin = lex->tok;
  char* ptr = strchr(begin, 'x');
  if (!ptr) ptr = strchr(begin, '*');
  if (ptr) {
    /* parse multiplier */
    *ptr = 0;
    ret.dice.multiplier = (float)atof(lex->tok);
    begin = ptr + 1;
  }
  ptr = strchr(begin, 'D');
  if (!ptr) ptr = strchr(begin, 'd');
  if (!ptr)
    TCOD_parser_error(
        "parseDiceValue : bad dice format. [<m>(x|*)]<n>(D|d)<f>[(+|-)<a>] expected instead of '%s'", lex->tok);
  *ptr = 0;
  /* parse nb_rolls */
  ret.dice.nb_rolls = atoi(begin);
  begin = ptr + 1;
  ptr = strchr(begin, '+');
  bool minus = false;
  if (!ptr) {
    ptr = strchr(begin, '-');
    if (ptr) minus = true;
  }
  if (ptr) *ptr = 0;
  /* parse nb_faces */
  ret.dice.nb_faces = atoi(begin);
  if (ptr) {
    /* parse addsub */
    begin = ptr + 1;
    ret.dice.addsub = (float)atof(begin);
    if (minus) ret.dice.addsub = -ret.dice.addsub;
  }
  return ret;
}

TCOD_value_t TCOD_parse_value_list_value(TCOD_ParserStruct* def, int list_num) {
  char* value;
  char** value_list = TCOD_list_get(def->lists, list_num);
  if (lex->token_type != TCOD_LEX_STRING)
    TCOD_parser_error("parseValueListValue : string constant expected instead of '%s'", lex->tok);
  int i = 0;
  value = value_list[i];
  while (value) {
    if (strcmp(lex->tok, value) == 0) break;
    i++;
    value = value_list[i];
  }
  if (!value) TCOD_parser_error("parseValueListValue : incorrect value '%s'", lex->tok);
  return (TCOD_value_t){.s = value};
}

TCOD_value_t TCOD_parse_property_value(TCOD_Parser* parser, TCOD_ParserStruct* def, char* propname, bool list) {
  TCOD_value_type_t type = TCOD_struct_get_type(def, propname);
  if (!list) type &= ~TCOD_TYPE_LIST;
  TCOD_value_t ret = {0};
  if (type & TCOD_TYPE_LIST) {
    type &= ~TCOD_TYPE_LIST;
    if (strcmp(lex->tok, "[") != 0) {
      TCOD_parser_error("'[' expected for list value instead of '%s'", lex->tok);
    }
    ret.list = TCOD_list_new();
    do {
      const int tok = TCOD_lex_parse(lex);
      if (tok == TCOD_LEX_EOF || tok == TCOD_LEX_ERROR) {
        TCOD_parser_error("Missing ']' in list value");
      }
      TCOD_value_t val = TCOD_parse_property_value(parser, def, propname, false);
      if (type == TCOD_TYPE_STRING || (type >= TCOD_TYPE_VALUELIST00 && type <= TCOD_TYPE_VALUELIST15)) {
        TCOD_list_push(ret.list, TCOD_strdup(val.s));
      } else {
        TCOD_list_push(ret.list, val.custom);
      }
      TCOD_lex_parse(lex);
      if (strcmp(lex->tok, ",") != 0 && strcmp(lex->tok, "]") != 0) {
        TCOD_parser_error("',' or ']' expected in list value instead of '%s'", lex->tok);
      }
    } while (strcmp(lex->tok, "]") != 0);
  } else {
    switch (type) {
      case TCOD_TYPE_BOOL:
        return TCOD_parse_bool_value();
      case TCOD_TYPE_CHAR:
        return TCOD_parse_char_value();
      case TCOD_TYPE_INT:
        return TCOD_parse_integer_value();
      case TCOD_TYPE_FLOAT:
        return TCOD_parse_float_value();
      case TCOD_TYPE_STRING:
        return TCOD_parse_string_value();
      case TCOD_TYPE_COLOR:
        return TCOD_parse_color_value();
      case TCOD_TYPE_DICE:
        return TCOD_parse_dice_value();
      case TCOD_TYPE_VALUELIST00:
      case TCOD_TYPE_VALUELIST01:
      case TCOD_TYPE_VALUELIST02:
      case TCOD_TYPE_VALUELIST03:
      case TCOD_TYPE_VALUELIST04:
      case TCOD_TYPE_VALUELIST05:
      case TCOD_TYPE_VALUELIST06:
      case TCOD_TYPE_VALUELIST07:
      case TCOD_TYPE_VALUELIST08:
      case TCOD_TYPE_VALUELIST09:
      case TCOD_TYPE_VALUELIST10:
      case TCOD_TYPE_VALUELIST11:
      case TCOD_TYPE_VALUELIST12:
      case TCOD_TYPE_VALUELIST13:
      case TCOD_TYPE_VALUELIST14:
      case TCOD_TYPE_VALUELIST15:
        return TCOD_parse_value_list_value(def, type - TCOD_TYPE_VALUELIST00);
      case TCOD_TYPE_CUSTOM00:
      case TCOD_TYPE_CUSTOM01:
      case TCOD_TYPE_CUSTOM02:
      case TCOD_TYPE_CUSTOM03:
      case TCOD_TYPE_CUSTOM04:
      case TCOD_TYPE_CUSTOM05:
      case TCOD_TYPE_CUSTOM06:
      case TCOD_TYPE_CUSTOM07:
      case TCOD_TYPE_CUSTOM08:
      case TCOD_TYPE_CUSTOM09:
      case TCOD_TYPE_CUSTOM10:
      case TCOD_TYPE_CUSTOM11:
      case TCOD_TYPE_CUSTOM12:
      case TCOD_TYPE_CUSTOM13:
      case TCOD_TYPE_CUSTOM14:
      case TCOD_TYPE_CUSTOM15:
        if (parser->customs[type - TCOD_TYPE_CUSTOM00]) {
          return parser->customs[type - TCOD_TYPE_CUSTOM00](lex, listener, def, propname);
        } else {
          TCOD_parser_error(
              "parse_property_value : no custom parser for property type %d for entity %s prop %s",
              type,
              def->name,
              propname);
        }
        break;
      default:
        TCOD_parser_error(
            "parse_property_value : unknown property type %d for entity %s prop %s", type, def->name, propname);
        break;
    }
  }
  return ret;
}

static bool TCOD_parser_parse_entity(TCOD_Parser* parser, TCOD_ParserStruct* def) {
  const char* name = NULL;
  if (TCOD_lex_parse(lex) == TCOD_LEX_STRING) {
    /* entity type name */
    name = TCOD_strdup(lex->tok);
    TCOD_lex_parse(lex);
  }
  if (strcmp(lex->tok, "{") != 0) {
    TCOD_parser_error("Parser::parseEntity : '{' expected");
    return false;
  }
  TCOD_lex_parse(lex);
  while (strcmp(lex->tok, "}") != 0) {
    TCOD_lex_t saved = {0};
    TCOD_lex_savepoint(lex, &saved);
    bool dynStruct = false;
    TCOD_value_type_t dynType = TCOD_TYPE_NONE;
    if (lex->token_type == TCOD_LEX_KEYWORD) {
      /* dynamic property declaration */
      if (strcmp(lex->tok, "bool") == 0)
        dynType = TCOD_TYPE_BOOL;
      else if (strcmp(lex->tok, "char") == 0)
        dynType = TCOD_TYPE_CHAR;
      else if (strcmp(lex->tok, "int") == 0)
        dynType = TCOD_TYPE_INT;
      else if (strcmp(lex->tok, "float") == 0)
        dynType = TCOD_TYPE_FLOAT;
      else if (strcmp(lex->tok, "string") == 0)
        dynType = TCOD_TYPE_STRING;
      else if (strcmp(lex->tok, "color") == 0)
        dynType = TCOD_TYPE_COLOR;
      else if (strcmp(lex->tok, "dice") == 0)
        dynType = TCOD_TYPE_DICE;
      else if (strcmp(lex->tok, "struct") == 0)
        dynStruct = true;
      else {
        TCOD_parser_error("Parser::parseEntity : dynamic declaration of '%s' not supported", lex->tok);
        return false;
      }
      /* TODO : dynamically declared sub-structures */
      TCOD_lex_parse(lex);
      if (strcmp(lex->tok, "[") == 0) {
        if (dynType == TCOD_TYPE_NONE) {
          TCOD_parser_error("Parser::parseEntity : unexpected symbol '['");
          return false;
        }
        TCOD_lex_parse(lex);
        if (strcmp(lex->tok, "]") != 0) {
          TCOD_parser_error("Parser::parseEntity : syntax error. ']' expected instead of '%s'", lex->tok);
          return false;
        }
        dynType |= TCOD_TYPE_LIST;
        TCOD_lex_parse(lex);
      }
      if (lex->token_type != TCOD_LEX_IDEN) {
        // If an identifier does not exist here then it was mistaken for the type.
        TCOD_lex_restore(lex, &saved);
      }
    }
    /* parse entity type content */
    if (lex->token_type != TCOD_LEX_IDEN && lex->token_type != TCOD_LEX_KEYWORD) {  // Allow keywords as identifiers.
      TCOD_parser_error("Parser::parseEntity : identifier or type expected");
      return false;
    }
    /* is it a flag ? */
    bool found = false;
    if (!dynStruct && dynType == TCOD_TYPE_NONE) {
      for (char** iflag = (void*)TCOD_list_begin(def->flags); iflag != (void*)TCOD_list_end(def->flags); ++iflag) {
        if (strcmp(*iflag, lex->tok) == 0) {
          found = true;
          if (!listener->new_flag(lex->tok)) return false;
          break;
        }
      }
    }
    if (!found && !dynStruct) {
      do {
        /* is it a property ? */
        for (TCOD_struct_prop_t** i_prop = (void*)TCOD_list_begin(def->props);
             i_prop != (void*)TCOD_list_end(def->props);
             i_prop++) {
          if (strcmp((*i_prop)->name, lex->tok) == 0) {
            char propname[MAX_NAME_LEN];
            string_copy(propname, lex->tok, MAX_NAME_LEN);
            TCOD_lex_parse(lex);
            if (strcmp(lex->tok, "=") != 0) {
              TCOD_parser_error("Parser::parseEntity : '=' expected");
              return false;
            }
            TCOD_lex_parse(lex);
            if (!listener->new_property(
                    propname,
                    TCOD_struct_get_type(def, propname),
                    TCOD_parse_property_value(parser, def, propname, true)))
              return false;
            if (lex->token_type == TCOD_LEX_ERROR) return false;
            found = true;
            break;
          }
        }
        if (!found && dynType != TCOD_TYPE_NONE) {
          /* dynamically add a property to the current structure */
          TCOD_struct_add_property(def, lex->tok, dynType, false);
        }
      } while (!found && dynType != TCOD_TYPE_NONE);
    }
    if (!found) {
      /* is it a sub-entity type */
      char id[MAX_NAME_LEN * 2 + 2];
      bool blockFound = false;
      do {
        TCOD_lex_t save;
        TCOD_lex_savepoint(lex, &save);
        char type[MAX_NAME_LEN];
        string_copy(type, lex->tok, MAX_NAME_LEN);
        strcpy(id, type);
        const char* subname = NULL;
        bool named = false;
        if (TCOD_lex_parse(lex) == TCOD_LEX_STRING) {
          /* <type>#<name> */
          strcat(id, "#");
          strcat(id, lex->tok);
          named = true;
          subname = TCOD_strdup(lex->tok);
          TCOD_lex_restore(lex, &save);
          for (TCOD_ParserStruct** sub = (void*)TCOD_list_begin(def->structs);
               sub != (void*)TCOD_list_end(def->structs);
               sub++) {
            if (strcmp((*sub)->name, id) == 0) {
              if (!listener->new_struct(*sub, lex->tok)) return false;
              if (!TCOD_parser_parse_entity(parser, *sub)) return false;
              blockFound = true;
              found = true;
              break;
            }
          }
        } else {
          TCOD_lex_restore(lex, &save);
        }
        if (!blockFound) {
          /* <type> alone */
          for (TCOD_ParserStruct** sub = (void*)TCOD_list_begin(def->structs);
               sub != (void*)TCOD_list_end(def->structs);
               sub++) {
            if (strcmp((*sub)->name, type) == 0) {
              if (!listener->new_struct(*sub, subname)) return false;
              if (!TCOD_parser_parse_entity(parser, *sub)) return false;
              blockFound = true;
              found = true;
              break;
            }
          }
        }
        if (!blockFound && dynStruct) {
          /* unknown structure. auto-declaration */
          const TCOD_ParserStruct* s = NULL;
          for (const TCOD_ParserStruct** i_def = (void*)TCOD_list_begin(parser->structs);
               i_def != (void*)TCOD_list_end(parser->structs);
               i_def++) {
            if (strcmp((*i_def)->name, id) == 0) {
              s = *i_def;
              break;
            }
          }
          if (s == NULL && named) {
            /* look for general definition <type> for entity <type>#<name> */
            for (const TCOD_ParserStruct** i_def = (void*)TCOD_list_begin(parser->structs);
                 i_def != (void*)TCOD_list_end(parser->structs);
                 i_def++) {
              if (strcmp((*i_def)->name, type) == 0) {
                s = *i_def;
                break;
              }
            }
          }
          if (s == NULL) {
            /* dyn struct not found. create it */
            s = TCOD_parser_new_struct(parser, type);
          }
          TCOD_struct_add_structure(def, s);
        }
      } while (!blockFound && dynStruct);
      if (!blockFound) {
        TCOD_parser_error("Parser::parseEntity : entity type %s does not contain %s", def->name, id);
        return false;
      }
    }
    TCOD_lex_parse(lex);
  }
  if (!listener->end_struct(def, name)) return false;
  return true;
}

/****************************************/
/* generic parser */
/****************************************/

TCOD_Parser* TCOD_parser_new(void) {
  TCOD_Parser* ent = calloc(1, sizeof(*ent));
  ent->structs = TCOD_list_new();
  return ent;
}

TCOD_value_type_t TCOD_parser_new_custom_type(TCOD_Parser* parser, TCOD_parser_custom_t custom_type_parser) {
  TCOD_value_type_t type = TCOD_TYPE_CUSTOM00;
  while (parser->customs[type - TCOD_TYPE_CUSTOM00] && type < TCOD_TYPE_CUSTOM15) type = (TCOD_value_type_t)(type + 1);
  if (parser->customs[type - TCOD_TYPE_CUSTOM00]) {
    /* no more custom types slots available */
    return TCOD_TYPE_NONE;
  }
  parser->customs[type - TCOD_TYPE_CUSTOM00] = custom_type_parser;
  return type;
}

TCOD_ParserStruct* TCOD_parser_new_struct(TCOD_Parser* parser, const char* name) {
  TCOD_ParserStruct* ent = calloc(1, sizeof(*ent));
  ent->name = TCOD_strdup(name);
  ent->flags = TCOD_list_new();
  ent->props = TCOD_list_new();
  ent->lists = TCOD_list_new();
  ent->structs = TCOD_list_new();
  TCOD_list_push(parser->structs, ent);
  return ent;
}

void TCOD_parser_delete(TCOD_Parser* parser) {
  for (TCOD_ParserStruct** i_def = (void*)TCOD_list_begin(parser->structs);
       i_def != (void*)TCOD_list_end(parser->structs);
       ++i_def) {
    free((*i_def)->name);

    for (TCOD_struct_prop_t** propCleanup = (void*)TCOD_list_begin((*i_def)->props);
         propCleanup != (void*)TCOD_list_end((*i_def)->props);
         ++propCleanup) {
      free((*propCleanup)->name);
    }
    TCOD_list_clear_and_delete((*i_def)->props);

    int listSize = 0;
    for (char*** listCleanup = (void*)TCOD_list_begin((*i_def)->lists);
         listCleanup != (void*)TCOD_list_end((*i_def)->lists);
         ++listCleanup) {
      while ((*listCleanup)[listSize] != NULL) {
        free((*listCleanup)[listSize]);
        ++listSize;
      }
    }
    TCOD_list_clear_and_delete((*i_def)->lists);
  }
  TCOD_list_clear_and_delete(parser->structs);
}

/* parse a file */
static struct TCOD_List* default_props;
/* triggers callbacks in the listener for each event during parsing */
void TCOD_parser_run(TCOD_Parser* parser, const char* filename, TCOD_parser_listener_t* _listener) {
  if (!_listener && !parser->props) parser->props = TCOD_list_new();
  listener = _listener ? _listener : &default_listener;
  default_props = parser->props;
  lex = TCOD_lex_new(symbols, keywords, "//", "/*", "*/", NULL, "\"", TCOD_LEX_FLAG_NESTING_COMMENT);
  if (!TCOD_lex_set_data_file(lex, (char*)filename)) {
    char buf[1024];
    sprintf(buf, "Fatal error : %s\n", TCOD_get_error());
    listener->error(buf);
    return;
  }
  while (1) {
    TCOD_lex_parse(lex);
    if (lex->token_type == TCOD_LEX_EOF || lex->token_type == TCOD_LEX_ERROR) break;
    bool dynStruct = false;
    if (lex->token_type == TCOD_LEX_KEYWORD) {
      if (strcmp(lex->tok, "struct") == 0) {
        /* level 0 dynamic structure declaration */
        dynStruct = true;
        TCOD_lex_parse(lex);
      } else {
        TCOD_parser_error("Parser::parse : unexpected keyword '%s'", lex->tok);
        return;
      }
    }
    /* get entity type */
    if (lex->token_type != TCOD_LEX_IDEN) {
      TCOD_parser_error("Parser::parse : identifier token expected");
      return;
    }
    char type[MAX_NAME_LEN];
    string_copy(type, lex->tok, MAX_NAME_LEN);
    char id[MAX_NAME_LEN * 2 + 2];
    strcpy(id, type);
    TCOD_lex_t save;
    TCOD_lex_savepoint(lex, &save);
    bool named = false;
    if (TCOD_lex_parse(lex) == TCOD_LEX_STRING) {
      /* named entity. id = <type>#<name> */
      strcat(id, "#");
      if (strlen(lex->tok) >= MAX_NAME_LEN) {
        TCOD_parser_error("Parser::parse : name %s too long. Max %d characters", lex->tok, MAX_NAME_LEN - 1);
        return;
      }
      strcat(id, lex->tok);
      named = true;
    }
    TCOD_lex_restore(lex, &save);
    TCOD_ParserStruct* def = NULL;
    do {
      /* look for a definition for id */
      for (TCOD_ParserStruct** i_def = (void*)TCOD_list_begin(parser->structs);
           i_def != (void*)TCOD_list_end(parser->structs);
           i_def++) {
        if (strcmp((*i_def)->name, id) == 0) {
          def = *i_def;
          break;
        }
      }
      if (def == NULL && named) {
        /* look for general definition <type> for entity <type>#<name> */
        for (TCOD_ParserStruct** i_def = (void*)TCOD_list_begin(parser->structs);
             i_def != (void*)TCOD_list_end(parser->structs);
             i_def++) {
          if (strcmp((*i_def)->name, type) == 0) {
            def = (*i_def);
            break;
          }
        }
      }
      if (def == NULL && dynStruct) {
        /* dyn struct not found. create it */
        TCOD_parser_new_struct(parser, type);
      }
    } while (def == NULL && dynStruct);
    if (def == NULL) {
      TCOD_parser_error("Parser::parse : unknown entity type %s", type);
      return;
    } else {
      if (!listener->new_struct(def, named ? strchr(id, '#') + 1 : NULL)) return;
      if (!TCOD_parser_parse_entity(parser, def)) return;
    }
  }
  if (lex->token_type == TCOD_LEX_ERROR) {
    TCOD_parser_error("Parser::parse : error while parsing");
    return;
  }
  TCOD_lex_delete(lex);
}
/* default parser listener */
typedef struct {
  char* name;
  TCOD_value_type_t type;
  TCOD_value_t value;
} prop_t;
static char cur_prop_name[512] = "";
static bool default_new_struct(TCOD_ParserStruct* str, const char* name) {
  (void)name;  // Ignored parameters.
  if (cur_prop_name[0]) strcat(cur_prop_name, ".");
  strcat(cur_prop_name, str->name);
  return true;
}

static bool default_new_flag(const char* name) {
  char tmp[1024] = "";
  snprintf(tmp, sizeof(tmp), "%s.%s", cur_prop_name, name);
  prop_t* prop = calloc(1, sizeof(*prop));
  prop->name = TCOD_strdup(tmp);
  prop->type = TCOD_TYPE_BOOL;
  prop->value.b = true;
  TCOD_list_push(default_props, prop);
  return true;
}

static bool default_new_property(const char* propname, TCOD_value_type_t type, TCOD_value_t value) {
  char tmp[1024] = "";
  snprintf(tmp, sizeof(tmp), "%s.%s", cur_prop_name, propname);
  prop_t* prop = calloc(1, sizeof(*prop));
  prop->name = TCOD_strdup(tmp);
  prop->type = type;
  prop->value = value;
  TCOD_list_push(default_props, prop);
  return true;
}

static bool default_end_struct(TCOD_ParserStruct* str, const char* name) {
  (void)str;
  (void)name;  // Ignored parameters.
  char* ptr = strrchr(cur_prop_name, '.');
  if (ptr)
    *ptr = '\0';
  else
    cur_prop_name[0] = '\0';
  return true;
}

static void default_error(const char* msg) { TCOD_set_errorvf("Error during parser: %s\n", msg); }

static const TCOD_value_t* TCOD_get_property(TCOD_Parser* parser, TCOD_value_type_t expectedType, const char* name) {
  if (!parser->props) return NULL;
  char err[512];
  for (prop_t** it = (void*)TCOD_list_begin(parser->props); it != (void*)TCOD_list_end(parser->props); ++it) {
    prop_t* prop = *it;
    if (strcmp(prop->name, name) == 0) {
      /* property found. check type */
      if (expectedType == TCOD_TYPE_STRING && prop->type >= TCOD_TYPE_VALUELIST00 &&
          prop->type <= TCOD_TYPE_VALUELIST15)
        return &prop->value;
      if (expectedType == TCOD_TYPE_CUSTOM00 && prop->type >= TCOD_TYPE_CUSTOM00 && prop->type <= TCOD_TYPE_CUSTOM15)
        return &prop->value;
      if (prop->type != expectedType) {
        sprintf(err, "Fatal error ! Try to read property '%s' width bad type\n", name);
        default_error(err);
      }
      return &prop->value;
    }
  }
  /* property not found. Check if it exists */
  char tmp[512];
  strcpy(tmp, name);
  sprintf(err, "Fatal error ! Try to read unknown property '%s'\n", name);
  char* ptr = strchr(tmp, '.');
  char* current_name = tmp;
  TCOD_ParserStruct* str = NULL;
  while (ptr) {
    bool found = false;
    *ptr = 0;
    for (TCOD_ParserStruct** it = (void*)TCOD_list_begin(parser->structs);
         !found && it != (void*)TCOD_list_end(parser->structs);
         ++it) {
      str = *it;
      if (strcmp(str->name, current_name) == 0) found = true;
    }
    if (!found) {
      /* one of the structures is unknown */
      default_error(err);
    }
    current_name = ptr + 1;
    ptr = strchr(current_name, '.');
  }
  if (!str) {
    /* no structure in name */
    default_error(err);
  }
  TCOD_value_type_t type = TCOD_struct_get_type(str, current_name);
  if (type == TCOD_TYPE_NONE) {
    /* property does not exist in structure */
    default_error(err);
  }
  /* optional property not defined in the file => ok */
  return NULL;
}

bool TCOD_parser_has_property(TCOD_Parser* parser, const char* name) {
  if (!parser->props) return false;
  for (prop_t** prop = (void*)TCOD_list_begin(parser->props); prop != (void*)TCOD_list_end(parser->props); ++prop) {
    if (strcmp((*prop)->name, name) == 0) {
      return true;
    }
  }
  return false;
}

bool TCOD_parser_get_bool_property(TCOD_Parser* parser, const char* name) {
  const TCOD_value_t* value = TCOD_get_property(parser, TCOD_TYPE_BOOL, name);
  return value ? value->b : false;
}

int TCOD_parser_get_int_property(TCOD_Parser* parser, const char* name) {
  const TCOD_value_t* value = TCOD_get_property(parser, TCOD_TYPE_INT, name);
  return value ? value->i : 0;
}

int TCOD_parser_get_char_property(TCOD_Parser* parser, const char* name) {
  const TCOD_value_t* value = TCOD_get_property(parser, TCOD_TYPE_CHAR, name);
  return value ? value->c : 0;
}

float TCOD_parser_get_float_property(TCOD_Parser* parser, const char* name) {
  const TCOD_value_t* value = TCOD_get_property(parser, TCOD_TYPE_FLOAT, name);
  return value ? value->f : 0.0f;
}

const char* TCOD_parser_get_string_property(TCOD_Parser* parser, const char* name) {
  const TCOD_value_t* value = TCOD_get_property(parser, TCOD_TYPE_STRING, name);
  return value ? value->s : NULL;
}

TCOD_color_t TCOD_parser_get_color_property(TCOD_Parser* parser, const char* name) {
  const TCOD_value_t* value = TCOD_get_property(parser, TCOD_TYPE_COLOR, name);
  return value ? value->col : TCOD_black;
}

TCOD_dice_t TCOD_parser_get_dice_property(TCOD_Parser* parser, const char* name) {
  const TCOD_value_t* value = TCOD_get_property(parser, TCOD_TYPE_DICE, name);
  return value ? value->dice : (TCOD_dice_t){0, 0, 0.0f, 0.0f};
}

void TCOD_parser_get_dice_property_py(TCOD_Parser* parser, const char* name, TCOD_dice_t* dice) {
  *dice = TCOD_parser_get_dice_property(parser, name);
}

void* TCOD_parser_get_custom_property(TCOD_Parser* parser, const char* name) {
  const TCOD_value_t* value = TCOD_get_property(parser, TCOD_TYPE_CUSTOM00, name);
  return value ? value->custom : NULL;
}

TCOD_list_t TCOD_parser_get_list_property(TCOD_Parser* parser, const char* name, TCOD_value_type_t type) {
  static TCOD_list_t empty_list = NULL;
  if (!empty_list) empty_list = TCOD_list_new();
  const TCOD_value_t* value = TCOD_get_property(parser, TCOD_TYPE_LIST | type, name);
  return value ? value->list : empty_list;
}
