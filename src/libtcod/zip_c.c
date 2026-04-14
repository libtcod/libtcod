/* BSD 3-Clause License
 *
 * Copyright © 2008-2026, Jice and the libtcod contributors.
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
#include "zip.h"
#ifndef TCOD_NO_ZLIB
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "console.h"
#include "libtcod_int.h"
#include "utility.h"

typedef struct TCOD_Zip {
  TCOD_List* buffer; /* list<int> */
  uintptr_t ibuffer; /* byte buffer. bytes are send into buffer 4 by 4 (32 bits OS) or 8 by 8(64 bits OS) */
  int isize; /* number of bytes in ibuffer */
  int bsize; /* number of bytes in buffer */
  int offset; /* current reading position */
} zip_data_t;

TCOD_zip_t TCOD_zip_new(void) {
  zip_data_t* zip = calloc(1, sizeof(*zip));
  return zip;
}

void TCOD_zip_delete(TCOD_zip_t zip) {
  if (zip->buffer) TCOD_list_delete(zip->buffer);
  free(zip);
}

/* output interface */
void TCOD_zip_put_char(TCOD_zip_t zip, char value) {
  const uintptr_t iv = (uintptr_t)(uint8_t)value;
  /* store one byte in ibuffer */
  switch (zip->isize) {
    case 0:
      zip->ibuffer |= iv;
      break;
    case 1:
      zip->ibuffer |= (iv << 8);
      break;
    case 2:
      zip->ibuffer |= (iv << 16);
      break;
    case 3:
      zip->ibuffer |= (iv << 24);
      break;
#ifdef TCOD_64BITS
    /* for 64 bits OS */
    case 4:
      zip->ibuffer |= (iv << 32);
      break;
    case 5:
      zip->ibuffer |= (iv << 40);
      break;
    case 6:
      zip->ibuffer |= (iv << 48);
      break;
    case 7:
      zip->ibuffer |= (iv << 56);
      break;
#endif
  }
  ++zip->isize;
  ++zip->bsize;
  if (zip->isize == sizeof(uintptr_t)) {
    /* ibuffer full. send it to buffer */
    if (!zip->buffer) zip->buffer = TCOD_list_new();
    TCOD_list_push(zip->buffer, (void*)zip->ibuffer);
    zip->isize = 0;
    zip->ibuffer = 0;
  }
}

void TCOD_zip_put_int(TCOD_zip_t zip, int value) {
#ifndef TCOD_64BITS
  if (zip->isize == 0) {
    /* the buffer is padded. read 4 bytes */
    if (!zip->buffer) zip->buffer = TCOD_list_new();
    TCOD_list_push(zip->buffer, (void*)value);
    zip->bsize += sizeof(uintptr_t);
  } else {
#endif
    /* the buffer is not padded. read 4x1 byte */
    TCOD_zip_put_char(zip, (char)(value & 0xFF));
    TCOD_zip_put_char(zip, (char)((value & 0xFF00) >> 8));
    TCOD_zip_put_char(zip, (char)((value & 0xFF0000) >> 16));
    TCOD_zip_put_char(zip, (char)((value & 0xFF000000) >> 24));
#ifndef TCOD_64BITS
  }
#endif
}

void TCOD_zip_put_float(TCOD_zip_t zip, float value) { TCOD_zip_put_int(zip, *(int*)(&value)); }

void TCOD_zip_put_string(TCOD_zip_t zip, const char* value) {
  if (value == NULL)
    TCOD_zip_put_int(zip, -1);
  else {
    const size_t length = strlen(value);
    TCOD_zip_put_int(zip, (int)length);
    for (size_t i = 0; i <= length; ++i) TCOD_zip_put_char(zip, value[i]);
  }
}

void TCOD_zip_put_data(TCOD_zip_t zip, int nbBytes, const void* data) {
  if (data == NULL)
    TCOD_zip_put_int(zip, -1);
  else {
    const char* value = (const char*)data;
    TCOD_zip_put_int(zip, nbBytes);
    for (int i = 0; i < nbBytes; ++i) TCOD_zip_put_char(zip, value[i]);
  }
}

void TCOD_zip_put_color(TCOD_zip_t zip, const TCOD_color_t value) {
  TCOD_zip_put_char(zip, value.r);
  TCOD_zip_put_char(zip, value.g);
  TCOD_zip_put_char(zip, value.b);
}

void TCOD_zip_put_image(TCOD_zip_t zip, const TCOD_Image* value) {
  int w, h;
  TCOD_image_get_size(value, &w, &h);
  TCOD_zip_put_int(zip, w);
  TCOD_zip_put_int(zip, h);
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      TCOD_zip_put_color(zip, TCOD_image_get_pixel(value, x, y));
    }
  }
}

void TCOD_zip_put_console(TCOD_zip_t zip, const TCOD_Console* value) {
  int w, h;
  w = TCOD_console_get_width(value);
  h = TCOD_console_get_height(value);
  TCOD_zip_put_int(zip, w);
  TCOD_zip_put_int(zip, h);
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      TCOD_zip_put_char(zip, TCOD_console_get_char(value, x, y) & 0xff);
      TCOD_zip_put_color(zip, TCOD_console_get_char_foreground(value, x, y));
      TCOD_zip_put_color(zip, TCOD_console_get_char_background(value, x, y));
    }
  }
}

// Return the size of an RNG to be serialized.
static int get_rng_bytesize(const TCOD_Random* rng) {
  if (!rng) return 0;
  switch (rng->algorithm) {
    case TCOD_RNG_MT:
    case TCOD_RNG_CMWC:
      return (int)sizeof(rng->mt_cmwc);
    default:
      return 0;  // Unkown RNG.  This could be a fatal error.
  }
}

void TCOD_zip_put_random(TCOD_zip_t zip, const TCOD_Random* rng) {
  const int size = get_rng_bytesize(rng);
  TCOD_zip_put_int(zip, size);
  TCOD_zip_put_data(zip, size, rng);
}

int TCOD_zip_save_to_file(TCOD_zip_t zip, const char* filename) {
  gzFile file = gzopen(filename, "wb");
  int length = zip->bsize;
  if (!file) return 0;
  gzwrite(file, &length, sizeof(int));
  if (length == 0) {
    gzclose(file);
    return 0;
  }
  if (zip->isize > 0) {
    /* send remaining bytes from ibuffer to buffer */
    if (!zip->buffer) zip->buffer = TCOD_list_new();
    TCOD_list_push(zip->buffer, (void*)zip->ibuffer);
    zip->isize = 0;
    zip->ibuffer = 0;
  }
  void* buffer = (void*)TCOD_list_begin(zip->buffer);
  int result = gzwrite(file, buffer, length);
  if (result != length) {
    gzclose(file);
    return 0;
  }
  result = gzclose(file);
  if (result != Z_OK) return 0;
  return length;
}

/* input interface */
int TCOD_zip_load_from_file(TCOD_zip_t zip, const char* filename) {
  static const int wordsize = sizeof(uintptr_t);
  gzFile file = gzopen(filename, "rb");
  if (!file) return 0;
  int length;
  gzread(file, &length, sizeof(int));
  if (length == 0) {
    gzclose(file);
    return 0;
  }
  if (zip->buffer) {
    TCOD_list_delete(zip->buffer);
    memset(zip, 0, sizeof(zip_data_t));
  }
  zip->buffer = TCOD_list_allocate((length + wordsize - 1) / wordsize);
  TCOD_list_set_size(zip->buffer, (length + wordsize - 1) / wordsize);
  void* buffer = (void*)TCOD_list_begin(zip->buffer);
  const int lread = gzread(file, buffer, length);
  gzclose(file);
  return lread == 0 ? length : lread;
}

char TCOD_zip_get_char(TCOD_zip_t zip) {
  char c = 0;
  if (zip->isize == 0) {
    /* ibuffer is empty. get 4 or 8 new bytes from buffer */
    zip->ibuffer = (uintptr_t)TCOD_list_get(zip->buffer, zip->offset);
    ++zip->offset;
    zip->isize = sizeof(uintptr_t);
  }
  /* read one byte from ibuffer */
#ifdef TCOD_64BITS
  switch (zip->isize) {
    case 8:
      c = zip->ibuffer & 0xFFL;
      break;
    case 7:
      c = (zip->ibuffer >> 8) & 0xFFL;
      break;
    case 6:
      c = (zip->ibuffer >> 16) & 0xFFL;
      break;
    case 5:
      c = (zip->ibuffer >> 24) & 0xFFL;
      break;
    case 4:
      c = (zip->ibuffer >> 32) & 0xFFL;
      break;
    case 3:
      c = (zip->ibuffer >> 40) & 0xFFL;
      break;
    case 2:
      c = (zip->ibuffer >> 48) & 0xFFL;
      break;
    case 1:
      c = (zip->ibuffer >> 56) & 0xFFL;
      break;
  }
#else
  switch (zip->isize) {
    case 4:
      c = (char)(zip->ibuffer & 0xFF);
      break;
    case 3:
      c = (char)((zip->ibuffer & 0xFF00) >> 8);
      break;
    case 2:
      c = (char)((zip->ibuffer & 0xFF0000) >> 16);
      break;
    case 1:
      c = (char)((zip->ibuffer & 0xFF000000) >> 24);
      break;
  }
#endif
  --zip->isize;
  return c;
}

int TCOD_zip_get_int(TCOD_zip_t zip) {
#ifndef TCOD_64BITS
  if (zip->isize == 0) {
    /* buffer is padded. read 4 bytes */
    const int i = (int)TCOD_list_get(zip->buffer, zip->offset);
    ++zip->offset;
    return i;
  } else {
#endif
    /* buffer is not padded. read 4x 1 byte */
    uint32_t i1 = (uint32_t)(uint8_t)TCOD_zip_get_char(zip);
    uint32_t i2 = (uint32_t)(uint8_t)TCOD_zip_get_char(zip);
    uint32_t i3 = (uint32_t)(uint8_t)TCOD_zip_get_char(zip);
    uint32_t i4 = (uint32_t)(uint8_t)TCOD_zip_get_char(zip);

    return i1 | (i2 << 8) | (i3 << 16) | (i4 << 24);
#ifndef TCOD_64BITS
  }
#endif
}

float TCOD_zip_get_float(TCOD_zip_t zip) {
  int i = TCOD_zip_get_int(zip);
  return *(float*)(&i);
}

TCOD_color_t TCOD_zip_get_color(TCOD_zip_t zip) {
  TCOD_color_t col;
  col.r = TCOD_zip_get_char(zip);
  col.g = TCOD_zip_get_char(zip);
  col.b = TCOD_zip_get_char(zip);
  return col;
}

const char* TCOD_zip_get_string(TCOD_zip_t zip) {
  const int length = TCOD_zip_get_int(zip);
  const char* result = (const char*)TCOD_list_begin(zip->buffer);
  if (length == -1) return NULL;
  int b_offset = zip->offset * sizeof(uintptr_t) - zip->isize; /* current offset in bytes */
  result += b_offset; /* the string address in buffer */
  b_offset += length + 1; /* new offset */
  /* update ibuffer */
  zip->offset = (b_offset + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);
  zip->isize = b_offset % sizeof(uintptr_t);
  if (zip->isize != 0) {
    zip->isize = sizeof(uintptr_t) - zip->isize;
    zip->ibuffer = (uintptr_t)TCOD_list_get(zip->buffer, zip->offset - 1);
  }
  return result;
}

int TCOD_zip_get_data(TCOD_zip_t zip, int nbBytes, void* data) {
  const int length = TCOD_zip_get_int(zip);
  const char* in = (const char*)TCOD_list_begin(zip->buffer);
  if (length == -1) return 0;
  int b_offset = zip->offset * sizeof(uintptr_t) - zip->isize; /* current offset in bytes */
  in += b_offset; /* the data address in buffer */
  /* copy it to data */
  char* out = (char*)data;
  for (int i = 0; i < TCOD_MIN(length, nbBytes); ++i) {
    *(out++) = *(in++);
    ++b_offset;
  }
  /* update ibuffer */
  zip->offset = (b_offset + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);
  zip->isize = b_offset % sizeof(uintptr_t);
  if (zip->isize != 0) {
    zip->isize = sizeof(uintptr_t) - zip->isize;
    zip->ibuffer = (uintptr_t)TCOD_list_get(zip->buffer, zip->offset - 1);
  }
  return length;
}

TCOD_Image* TCOD_zip_get_image(TCOD_zip_t zip) {
  const int w = TCOD_zip_get_int(zip);
  const int h = TCOD_zip_get_int(zip);
  TCOD_Image* result = TCOD_image_new(w, h);
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      TCOD_image_put_pixel(result, x, y, TCOD_zip_get_color(zip));
    }
  }
  return result;
}

TCOD_console_t TCOD_zip_get_console(TCOD_zip_t zip) {
  const int w = TCOD_zip_get_int(zip);
  const int h = TCOD_zip_get_int(zip);
  TCOD_console_t result = TCOD_console_new(w, h);
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      TCOD_console_set_char(result, x, y, (unsigned char)TCOD_zip_get_char(zip));
      TCOD_console_set_char_foreground(result, x, y, TCOD_zip_get_color(zip));
      TCOD_console_set_char_background(result, x, y, TCOD_zip_get_color(zip), TCOD_BKGND_SET);
    }
  }
  return result;
}

TCODLIB_API TCOD_Random* TCOD_zip_get_random(TCOD_zip_t zip) {
  const int s = TCOD_zip_get_int(zip);
  if (s <= 0) return NULL;
  TCOD_Random* result = malloc(s);
  TCOD_zip_get_data(zip, s, result);
  return result;
}

uint32_t TCOD_zip_get_current_bytes(TCOD_zip_t zip) {
  if (!zip->buffer) zip->buffer = TCOD_list_new();
  return TCOD_list_size(zip->buffer) * sizeof(uintptr_t) + zip->isize;
}

uint32_t TCOD_zip_get_remaining_bytes(TCOD_zip_t zip) {
  if (!zip->buffer) zip->buffer = TCOD_list_new();
  return (TCOD_list_size(zip->buffer) - zip->offset) * sizeof(uintptr_t) + zip->isize;
}

void TCOD_zip_skip_bytes(TCOD_zip_t zip, uint32_t nbBytes) {
  const uint32_t b_offset = zip->offset * sizeof(uintptr_t) - zip->isize + nbBytes; /* new offset */
  TCOD_IFNOT(b_offset <= TCOD_list_size(zip->buffer) * sizeof(uintptr_t)) return;
  zip->offset = (b_offset + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);
  zip->isize = b_offset % sizeof(uintptr_t);
  if (zip->isize != 0) {
    zip->isize = sizeof(uintptr_t) - zip->isize;
    zip->ibuffer = (uintptr_t)TCOD_list_get(zip->buffer, zip->offset - 1);
  }
}
#endif  // TCOD_NO_ZLIB
