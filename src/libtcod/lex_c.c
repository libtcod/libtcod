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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "lex.h"

#define MAX_JAVADOC_COMMENT_SIZE 16384

char* TCOD_strdup(const char* str) {
  char* duplicate = malloc(strlen(str) + 1);
  if (duplicate) strcpy(duplicate, str);
  return duplicate;
}

int TCOD_strcasecmp(const char* s1, const char* s2) {
  while (*s1 != '\0' && tolower(*s1) == tolower(*s2)) {
    ++s1;
    ++s2;
  }
  return tolower(*s1) - tolower(*s2);
}

int TCOD_strncasecmp(const char* s1, const char* s2, size_t n) {
  while (n-- && tolower(*s1) == tolower(*s2)) {
    ++s1;
    ++s2;
  }
  return tolower(*s1) - tolower(*s2);
}

static const char* TCOD_LEX_names[] = {
    "unknown token", "symbol", "keyword", "identifier", "string", "integer", "float", "char", "eof"};

const char* TCOD_lex_get_token_name(int token_type) { return TCOD_LEX_names[token_type]; }

static void allocate_tok(TCOD_lex_t* lex, int len) {
  if (lex->toklen > len) return;
  while (lex->toklen <= len) lex->toklen *= 2;
  lex->tok = realloc(lex->tok, lex->toklen);  // This might fail and isn't checked.
}

TCOD_lex_t* TCOD_lex_new_intern(void) {
  TCOD_lex_t* lex = calloc(1, sizeof(TCOD_lex_t));
  if (!lex) TCOD_set_errorv("Out of memory allocating TCOD lexer.");
  return lex;
}

TCOD_lex_t* TCOD_lex_new(
    const char* const* symbols_in,
    const char* const* keywords_in,
    const char* simpleComment,
    const char* commentStart,
    const char* commentStop,
    const char* javadocCommentStart,
    const char* stringDelim,
    int flags) {
  TCOD_lex_t* lex = TCOD_lex_new_intern();
  if (!lex) return NULL;
  lex->flags = flags;
  lex->last_javadoc_comment = calloc(MAX_JAVADOC_COMMENT_SIZE, sizeof(*lex->last_javadoc_comment));
  while (symbols_in && *symbols_in) {
    if (strlen(*symbols_in) >= TCOD_LEX_SYMBOL_SIZE) {
      TCOD_set_errorvf("symbol '%s' too long (max size %d)", *symbols_in, TCOD_LEX_SYMBOL_SIZE);
      TCOD_lex_delete(lex);
      return NULL;
    }
    strcpy(lex->symbols[lex->nb_symbols], *symbols_in);
    ++lex->nb_symbols;
    ++symbols_in;
  }
  while (keywords_in && *keywords_in) {
    if (strlen(*keywords_in) >= TCOD_LEX_KEYWORD_SIZE) {
      TCOD_set_errorvf("keyword '%s' too long (max size %d)", *keywords_in, TCOD_LEX_KEYWORD_SIZE);
      TCOD_lex_delete(lex);
      return NULL;
    }
    strcpy(lex->keywords[lex->nb_keywords], *keywords_in);
    if (lex->flags & TCOD_LEX_FLAG_NOCASE) {
      char* ptr = lex->keywords[lex->nb_keywords];
      while (*ptr) {
        *ptr = (char)toupper(*ptr);
        ++ptr;
      }
    }
    ++lex->nb_keywords;
    ++keywords_in;
  }
  lex->simple_comment = simpleComment;
  lex->comment_start = commentStart;
  lex->comment_stop = commentStop;
  lex->javadoc_comment_start = javadocCommentStart;
  lex->stringDelim = stringDelim;
  lex->lastStringDelim = '\0';
  lex->tok = calloc(256, sizeof(*lex->tok));
  lex->toklen = 256;
  return lex;
}

char* TCOD_lex_get_last_javadoc(TCOD_lex_t* lex) {
  if (!lex->javadoc_read && lex->last_javadoc_comment[0] != '\0') {
    lex->javadoc_read = true;
    return lex->last_javadoc_comment;
  }
  lex->javadoc_read = false;
  lex->last_javadoc_comment[0] = '\0';
  return NULL;
}

void TCOD_lex_delete(TCOD_lex_t* lex) {
  if (!lex->is_savepoint) {
    if (lex->filename) free(lex->filename);
    if (lex->buf && lex->allocBuf) free(lex->buf);
    if (lex->last_javadoc_comment) free(lex->last_javadoc_comment);
  }
  lex->filename = NULL;
  lex->buf = NULL;
  lex->allocBuf = false;
  if (lex->tok) free(lex->tok);
  free(lex);
}

void TCOD_lex_set_data_buffer_internal(TCOD_lex_t* lex) {
  lex->file_line = 1;
  lex->pos = lex->buf;
  lex->token_type = TCOD_LEX_UNKNOWN;
  lex->token_int_val = 0;
  lex->token_float_val = 0.0;
  lex->token_idx = -1;
  lex->tok[0] = '\0';
}

void TCOD_lex_set_data_buffer(TCOD_lex_t* lex, char* dat) {
  lex->buf = dat;
  lex->allocBuf = false;
  TCOD_lex_set_data_buffer_internal(lex);
}

bool TCOD_lex_set_data_file(TCOD_lex_t* lex, const char* path) {
  if (!path) {
    TCOD_set_errorv("TCOD_lex_set_data_file(NULL) called");
    return false;
  }
  FILE* f = fopen(path, "rb");
  if (f == NULL) {
    TCOD_set_errorvf("Cannot open '%s'", path);
    return false;
  }
  fseek(f, 0, SEEK_END);
  const long size = ftell(f);
  fclose(f);
  f = fopen(path, "r");

  lex->buf = calloc(size + 1, sizeof(*lex->buf));
  lex->filename = TCOD_strdup(path);
  if (lex->buf == NULL || lex->filename == NULL) {
    fclose(f);
    if (lex->buf) free(lex->buf);
    if (lex->filename) {
      free(lex->filename);
    }
    TCOD_set_errorv("Out of memory");
    return false;
  }
  char* ptr = lex->buf;
  /* can't rely on size to read because of MS/DOS dumb CR/LF handling */
  while (fgets(ptr, size, f)) {
    ptr += strlen(ptr);
  }
  fclose(f);
  TCOD_lex_set_data_buffer_internal(lex);
  lex->allocBuf = true;
  return true;
}

void TCOD_lex_get_new_line(TCOD_lex_t* lex) {
  if (*(lex->pos) == '\n') {
    lex->file_line++;
    lex->pos++;
  }
}

int TCOD_lex_get_space(TCOD_lex_t* lex) {
  const char* startPos = NULL;
  while (1) {
    char c;
    while ((c = *lex->pos) <= ' ') {
      if (c == '\n')
        TCOD_lex_get_new_line(lex);
      else if (c == 0)
        return TCOD_LEX_EOF; /* end of file */
      else
        lex->pos++;
    }
    if (lex->simple_comment && strncmp(lex->pos, lex->simple_comment, strlen(lex->simple_comment)) == 0) {
      if (!startPos) startPos = lex->pos;
      while (*lex->pos != '\0' && *lex->pos != '\n') lex->pos++;
      TCOD_lex_get_new_line(lex);
      continue;
    }
    if (lex->comment_start && lex->comment_stop &&
        strncmp(lex->pos, lex->comment_start, strlen(lex->comment_start)) == 0) {
      const int isJavadoc =
          (lex->javadoc_comment_start &&
           strncmp(lex->pos, lex->javadoc_comment_start, strlen(lex->javadoc_comment_start)) == 0);
      int cmtLevel = 1;
      const char* javadocStart = NULL;
      if (!startPos) startPos = lex->pos;
      if (isJavadoc) {
        javadocStart = lex->pos + strlen(lex->javadoc_comment_start);
        while (isspace(*javadocStart)) javadocStart++;
      }
      lex->pos++;
      do {
        if (*lex->pos == '\n') {
          TCOD_lex_get_new_line(lex);
        } else
          lex->pos++;
        if (*lex->pos == '\0') return TCOD_LEX_EOF;
        if ((lex->flags & TCOD_LEX_FLAG_NESTING_COMMENT) &&
            strncmp(lex->pos - 1, lex->comment_start, strlen(lex->comment_start)) == 0)
          cmtLevel++;
        if (strncmp(lex->pos - 1, lex->comment_stop, strlen(lex->comment_stop)) == 0) cmtLevel--;
      } while (cmtLevel > 0);
      lex->pos++;
      if (isJavadoc) {
        const char* end = lex->pos - strlen(lex->comment_stop);
        while (isspace(*end) && end > javadocStart) end--;
        const char* src = javadocStart;
        char* dst = lex->last_javadoc_comment;
        while (src < end) {
          /* skip heading spaces */
          while (src < end && isspace(*src) && *src != '\n') src++;
          /* copy comment line */
          while (src < end && *src != '\n') *dst++ = *src++;
          if (*src == '\n') *dst++ = *src++;
        }
        /* remove trailing spaces */
        while (dst > lex->last_javadoc_comment && isspace(*(dst - 1))) dst--;
        *dst = '\0';
        lex->javadoc_read = false;
      }
      continue;
    }
    break;
  }
  if ((lex->flags & TCOD_LEX_FLAG_TOKENIZE_COMMENTS) && startPos && lex->pos > startPos) {
    const int len = (int)(lex->pos - startPos);
    allocate_tok(lex, len + 1);
    strncpy(lex->tok, startPos, len);
    lex->tok[len] = 0;
    lex->token_type = TCOD_LEX_COMMENT;
    lex->token_idx = -1;
    return TCOD_LEX_COMMENT;
  }
  return TCOD_LEX_UNKNOWN;
}

int TCOD_lex_hextoint(char c) {
  const int v = toupper(c);
  if (v >= '0' && v <= '9') return v - '0';
  return 10 + (v - 'A');
}

static bool TCOD_lex_get_special_char(TCOD_lex_t* lex, char* c) {
  *c = *(++(lex->pos));

  switch (*c) {
    case 'n':
      *c = '\n';
      break;
    case 't':
      *c = '\t';
      break;
    case 'r':
      *c = '\r';
      break;
    case '\\':
    case '\"':
    case '\'':
      break;
    case 'x': {
      /* hexadecimal value "\x80" */
      int value = 0;
      bool hasHex = false;
      *c = *(++(lex->pos));
      while ((*c >= '0' && *c <= '9') || (*c >= 'a' && *c <= 'f') || (*c >= 'A' && *c <= 'F')) {
        hasHex = true;
        value <<= 4;
        value += TCOD_lex_hextoint(*c);
        *c = *(++(lex->pos));
      }
      if (!hasHex) {
        TCOD_set_errorv("\\x must be followed by an hexadecimal value");
        return false;
      }
      *c = (char)value;  // Cast to EASCII.
      lex->pos--;
    } break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7': {
      /* octal value "\200" */
      int value = 0;
      while (*c >= '0' && *c <= '7') {
        value <<= 3;
        value += (*c - '0');
        *c = *(++(lex->pos));
      }
      *c = (char)value;  // Cast to EASCII.
      lex->pos--;
    } break;
    default:
      TCOD_set_errorv("bad escape sequence inside quote");
      return false;
  }
  return true;
}

int TCOD_lex_get_string(TCOD_lex_t* lex) {
  int len = 0;
  do {
    char c = *(++(lex->pos));
    if (c == '\0') {
      TCOD_set_errorv("EOF inside quote");
      return TCOD_LEX_ERROR;
    }
    if (c == '\n') {
      TCOD_set_errorv("newline inside quote");
      return TCOD_LEX_ERROR;
    }
    if (c == '\\') {
      if (!TCOD_lex_get_special_char(lex, &c)) return TCOD_LEX_ERROR;
    } else if (c == lex->lastStringDelim) {
      allocate_tok(lex, len);
      lex->tok[len] = '\0';
      lex->token_type = TCOD_LEX_STRING;
      lex->token_idx = -1;
      lex->pos++;
      return TCOD_LEX_STRING;
    }
    allocate_tok(lex, len);
    lex->tok[len++] = c;
  } while (1);
}

int TCOD_lex_get_number(TCOD_lex_t* lex) {
  int len = 0;
  if (*lex->pos == '-') {
    allocate_tok(lex, len);
    lex->tok[len++] = '-';
    lex->pos++;
  }

  int c = toupper(*lex->pos);

  bool bhex = false;
  bool bfloat = false;
  if (c == '0' && (lex->pos[1] == 'x' || lex->pos[1] == 'X')) {
    bhex = true;
    allocate_tok(lex, len);
    lex->tok[len++] = '0';
    lex->pos++;
    c = toupper(*(lex->pos));
  }
  do {
    allocate_tok(lex, len);
    lex->tok[len++] = (char)c;
    lex->pos++;
    if (c == '.') {
      if (bhex) {
        TCOD_set_errorv("bad constant format");
        return TCOD_LEX_ERROR;
      }
      bfloat = true;
    }
    c = toupper(*lex->pos);
  } while ((c >= '0' && c <= '9') || (bhex && c >= 'A' && c <= 'F') || c == '.');
  allocate_tok(lex, len);
  lex->tok[len] = 0;

  if (!bfloat) {
    lex->token_int_val = strtol(lex->tok, NULL, 0);
    lex->token_float_val = (float)lex->token_int_val;
    lex->token_type = TCOD_LEX_INTEGER;
    lex->token_idx = -1;
    return TCOD_LEX_INTEGER;
  } else {
    lex->token_float_val = (float)atof(lex->tok);
    lex->token_type = TCOD_LEX_FLOAT;
    lex->token_idx = -1;
    return TCOD_LEX_FLOAT;
  }
}

int TCOD_lex_get_char(TCOD_lex_t* lex) {
  char c = *(++(lex->pos));

  if (c == '\0') {
    TCOD_set_errorv("EOF inside simple quote");
    return TCOD_LEX_ERROR;
  }
  if (c == '\n') {
    TCOD_set_errorv("newline inside simple quote");
    return TCOD_LEX_ERROR;
  }
  if (c == '\\') {
    if (!TCOD_lex_get_special_char(lex, &c)) return TCOD_LEX_ERROR;
    lex->pos++;
  } else
    lex->pos++;

  if (*lex->pos != '\'') {
    TCOD_set_errorv("bad character inside simple quote");
    return TCOD_LEX_ERROR;
  }
  lex->pos++;
  lex->tok[0] = c;
  lex->tok[1] = '\0';
  lex->token_type = TCOD_LEX_CHAR;
  lex->token_int_val = (int)c;
  lex->token_idx = -1;
  return TCOD_LEX_CHAR;
}

int TCOD_lex_get_symbol(TCOD_lex_t* lex) {
  int symbol = 0;
  while (symbol < lex->nb_symbols) {
    if (((lex->flags & TCOD_LEX_FLAG_NOCASE) &&
         TCOD_strncasecmp(lex->symbols[symbol], lex->pos, strlen(lex->symbols[symbol])) == 0) ||
        (strncmp(lex->symbols[symbol], lex->pos, strlen(lex->symbols[symbol])) == 0)) {
      strcpy(lex->tok, lex->symbols[symbol]);
      lex->pos += strlen(lex->symbols[symbol]);
      lex->token_idx = symbol;
      lex->token_type = TCOD_LEX_SYMBOL;
      return TCOD_LEX_SYMBOL;
    }
    ++symbol;
  }

  ++lex->pos;
  TCOD_set_errorvf("unknown symbol %.10s", lex->pos - 1);
  return TCOD_LEX_ERROR;
}

int TCOD_lex_get_iden(TCOD_lex_t* lex) {
  char c = *lex->pos;
  int len = 0;
  do {
    allocate_tok(lex, len);
    lex->tok[len++] = c;
    c = *(++(lex->pos));
  } while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_');
  allocate_tok(lex, len);
  lex->tok[len] = 0;

  int key = 0;
  while (key < lex->nb_keywords) {
    if (strcmp(lex->tok, lex->keywords[key]) == 0 ||
        (lex->flags & TCOD_LEX_FLAG_NOCASE && TCOD_strcasecmp(lex->tok, lex->keywords[key]) == 0)) {
      lex->token_type = TCOD_LEX_KEYWORD;
      lex->token_idx = key;
      return TCOD_LEX_KEYWORD;
    }
    ++key;
  }
  lex->token_type = TCOD_LEX_IDEN;
  lex->token_idx = -1;
  return TCOD_LEX_IDEN;
}

int TCOD_lex_parse(TCOD_lex_t* lex) {
  const int token = TCOD_lex_get_space(lex);
  if (token == TCOD_LEX_ERROR) return token;
  const char* ptr = lex->pos;
  if (token != TCOD_LEX_UNKNOWN) {
    lex->token_type = token;
    return token;
  }
  if (strchr(lex->stringDelim, *ptr)) {
    lex->lastStringDelim = *ptr;
    return TCOD_lex_get_string(lex);
  }
  if (*ptr == '\'') {
    return TCOD_lex_get_char(lex);
  }
  if (isdigit((int)(*ptr)) || (*ptr == '-' && isdigit((int)(ptr[1])))) {
    return TCOD_lex_get_number(lex);
  }
  if ((*ptr >= 'a' && *ptr <= 'z') || (*ptr >= 'A' && *ptr <= 'Z') || *ptr == '_') {
    return TCOD_lex_get_iden(lex);
  }
  return TCOD_lex_get_symbol(lex);
}

int TCOD_lex_parse_until_token_type(TCOD_lex_t* lex, int tokenType) {
  int token = TCOD_lex_parse(lex);
  if (token == TCOD_LEX_ERROR) {
    return token;
  }
  while (token != TCOD_LEX_EOF) {
    if (token == tokenType) {
      return token;
    }
    token = TCOD_lex_parse(lex);
    if (token == TCOD_LEX_ERROR) {
      return token;
    }
  }
  return token;
}

int TCOD_lex_parse_until_token_value(TCOD_lex_t* lex, const char* tokenValue) {
  int token = TCOD_lex_parse(lex);
  if (token == TCOD_LEX_ERROR) {
    return token;
  }
  while (token != TCOD_LEX_EOF) {
    if (strcmp(lex->tok, tokenValue) == 0 ||
        ((lex->flags & TCOD_LEX_FLAG_NOCASE) && TCOD_strcasecmp(lex->tok, tokenValue) == 0)) {
      return token;
    }
    token = TCOD_lex_parse(lex);
    if (token == TCOD_LEX_ERROR) {
      return token;
    }
  }
  return token;
}

void TCOD_lex_savepoint(TCOD_lex_t* lex, TCOD_lex_t* savepoint) {
  *savepoint = *lex;
  savepoint->tok = calloc(lex->toklen, sizeof(*savepoint->tok));
  strcpy(savepoint->tok, lex->tok);
  savepoint->is_savepoint = true;
}

void TCOD_lex_restore(TCOD_lex_t* lex, TCOD_lex_t* savepoint) {
  *lex = *savepoint;
  lex->is_savepoint = false;
}

bool TCOD_lex_expect_token_type(TCOD_lex_t* lex, int token_type) { return (TCOD_lex_parse(lex) == token_type); }

bool TCOD_lex_expect_token_value(TCOD_lex_t* lex, int token_type, const char* token_value) {
  TCOD_lex_parse(lex);
  return (token_type == lex->token_type && strcmp(lex->tok, token_value) == 0);
}
