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
#ifndef NO_SDL
#include <SDL3/SDL.h>
#endif  // NO_SDL
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "libtcod_int.h"
#include "sys.h"
#include "version.h"
#ifdef TCOD_WINDOWS
#define NOMINMAX 1
#include <windows.h>
#else
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif
// clang-format off
/**
    Converts TCOD_FONT_LAYOUT_TCOD tile position to Extended ASCII code-point.
 */
static const int tcod_codec_eascii_[256] = {
    32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
    48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
    64,  91,  92,  93,  94,  95,  96,  123, 124, 125, 126, 176, 177, 178, 179, 196,
    197, 180, 193, 195, 194, 192, 218, 191, 217, 226, 227, 228, 229, 230, 231, 232,
    24,  25,  27,  26,  30,  31,  17,  16,  18,  29,  224, 225, 9,   10,  186, 205,
    206, 185, 202, 204, 203, 200, 201, 187, 188, 0,   0,   0,   0,   0,   0,   0,
    65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,
    81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  0,   0,   0,   0,   0,   0,
    97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
    113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};
// clang-format on
/**
    Older startup function, mostly called from TCOD_sys_init.
 */
void TCOD_sys_startup(void) {}
/**
    All shutdown routines call this function.

    Mostly used internally. TCOD_quit should be called to shutdown the library.
 */
void TCOD_sys_shutdown(void) {
  // Quitting or restarting can drop SDL's queue of events.
  // key_state needs to be cleared to prevent key modifiers from getting stuck when the library internals reset.
  TCOD_ctx.key_state = (TCOD_key_t){0};

  if (TCOD_ctx.root) {
    TCOD_console_delete(TCOD_ctx.root);
  }
  if (TCOD_ctx.engine) {
    TCOD_context_delete(TCOD_ctx.engine);
    TCOD_ctx.engine = NULL;
    return;
  }
}
/**
    See TCOD_console_map_ascii_code_to_font
  */
void TCOD_sys_map_ascii_to_font(int asciiCode, int fontCharX, int fontCharY) {
  if (!TCOD_ctx.tileset) {
    return;
  }
  TCOD_tileset_assign_tile(TCOD_ctx.tileset, TCOD_ctx.tileset->virtual_columns * fontCharY + fontCharX, asciiCode);
}
/**
    Set a code-point to point to an already existing code-point.
 */
static void TCOD_sys_map_clone_(int new_codepoint, int old_codepoint) {
  if (!TCOD_ctx.tileset) {
    return;
  }
  if (old_codepoint < 0) {
    return;
  }
  if (old_codepoint >= TCOD_ctx.tileset->character_map_length) {
    return;
  }
  TCOD_sys_map_ascii_to_font(new_codepoint, TCOD_ctx.tileset->character_map[old_codepoint], 0);
}
/**
    Decode the font layout depending on the current flags.
 */
void TCOD_sys_decode_font_(void) {
  if (!TCOD_ctx.tileset) {
    return;
  }
  if (TCOD_ctx.font_flags & TCOD_FONT_LAYOUT_CP437) {
    for (int i = 0; i < (int)(sizeof(TCOD_CHARMAP_CP437) / sizeof(*TCOD_CHARMAP_CP437)); ++i) {
      TCOD_sys_map_ascii_to_font(TCOD_CHARMAP_CP437[i], i, 0);
    }
  } else if (TCOD_ctx.font_tcod_layout) {
    for (int i = 0; i < (int)(sizeof(TCOD_CHARMAP_TCOD) / sizeof(*TCOD_CHARMAP_TCOD)); ++i) {
      TCOD_sys_map_ascii_to_font(TCOD_CHARMAP_TCOD[i], i, 0);
    }
    for (int i = 0; i < (int)(sizeof(tcod_codec_eascii_) / sizeof(*tcod_codec_eascii_)); ++i) {
      TCOD_sys_map_ascii_to_font(tcod_codec_eascii_[i], i, 0);
    }
  } else {
    if (TCOD_ctx.font_in_row) {
      /* for font in row */
      for (int i = 0; i < TCOD_ctx.tileset->tiles_count; ++i) {
        TCOD_sys_map_ascii_to_font(i, i, 0);
      }
    } else {
      /* for font in column */
      for (int i = 0; i < TCOD_ctx.tileset->tiles_count; ++i) {
        int fy = i % TCOD_ctx.tileset->virtual_columns;
        int fx = i / TCOD_ctx.tileset->virtual_columns;
        TCOD_sys_map_ascii_to_font(i, fx, fy);
      }
    }
    // Assume the font is mostly Code Page 437 and try to clone the Unicode codepoints from the EASCII codepoints.
    for (int i = 1; i <= 0xFE; ++i) {
      if (TCOD_CHARMAP_CP437[i] <= 0xFF) continue;  // Prefer the EASCII codepoints for backwards compatibility.
      TCOD_sys_map_clone_(TCOD_CHARMAP_CP437[i], i);
    }
    // Manually apply codepoints from TCOD's old default tileset.
    TCOD_sys_map_clone_(0x2500, 0xC4);  // TCOD_CHAR_HLINE
    TCOD_sys_map_clone_(0x2502, 0xB3);  // TCOD_CHAR_VLINE
    TCOD_sys_map_clone_(0x250C, 0xDA);  // TCOD_CHAR_NW
    TCOD_sys_map_clone_(0x2510, 0xBF);  // TCOD_CHAR_NE
    TCOD_sys_map_clone_(0x2514, 0xC0);  // TCOD_CHAR_SW
    TCOD_sys_map_clone_(0x2518, 0xD9);  // TCOD_CHAR_SE

    TCOD_sys_map_clone_(0x2190, 27);  // TCOD_CHAR_ARROW_W
    TCOD_sys_map_clone_(0x2192, 26);  // TCOD_CHAR_ARROW_E

    TCOD_sys_map_clone_(0x251C, 195);  // TCOD_CHAR_TEEE
    TCOD_sys_map_clone_(0x2524, 180);  // TCOD_CHAR_TEEW

    TCOD_sys_map_clone_(0x2550, 205);  // TCOD_CHAR_DHLINE
    TCOD_sys_map_clone_(0x2551, 186);  // TCOD_CHAR_DVLINE
    TCOD_sys_map_clone_(0x2554, 201);  // TCOD_CHAR_DNW
    TCOD_sys_map_clone_(0x2557, 187);  // TCOD_CHAR_DNE
    TCOD_sys_map_clone_(0x255A, 200);  // TCOD_CHAR_DSW
    TCOD_sys_map_clone_(0x255D, 188);  // TCOD_CHAR_DSE

    TCOD_sys_map_clone_(0x2611, 225);  // TCOD_CHAR_CHECKBOX_SET
    TCOD_sys_map_clone_(0x2610, 224);  // TCOD_CHAR_CHECKBOX_UNSET;

    TCOD_sys_map_clone_(0x2598, 226);  // TCOD_CHAR_SUBP_NW
    TCOD_sys_map_clone_(0x259D, 227);  // TCOD_CHAR_SUBP_NE
    TCOD_sys_map_clone_(0x2580, 228);  // TCOD_CHAR_SUBP_N
    TCOD_sys_map_clone_(0x2597, 229);  // TCOD_CHAR_SUBP_SE
    TCOD_sys_map_clone_(0x259E, 230);  // TCOD_CHAR_SUBP_DIAG
    TCOD_sys_map_clone_(0x2590, 231);  // TCOD_CHAR_SUBP_E
    TCOD_sys_map_clone_(0x2596, 231);  // TCOD_CHAR_SUBP_SW
  }
}

bool TCOD_sys_create_directory(const char* path) {
#ifdef TCOD_WINDOWS
  return (CreateDirectory(path, NULL) != 0 || GetLastError() == ERROR_ALREADY_EXISTS);
#else
  return mkdir(path, 0755) == 0 || errno == EEXIST;
#endif
}

bool TCOD_sys_delete_file(const char* path) {
#ifdef TCOD_WINDOWS
  return DeleteFile(path) != 0;
#else
  return unlink(path) == 0 || errno == ENOENT;
#endif
}

bool TCOD_sys_delete_directory(const char* path) {
#ifdef TCOD_WINDOWS
  return RemoveDirectory(path) != 0;
#else
  return rmdir(path) == 0 || errno == ENOENT;
#endif
}

bool TCOD_sys_is_directory(const char* path) {
#ifdef TCOD_WINDOWS
  DWORD type = GetFileAttributes(path);
  return (type & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
  DIR* d = opendir(path);
  if (d) {
    closedir(d);
    return true;
  }
  return false;
#endif
}

static bool filename_match(const char* name, const char* pattern) {
  const char* ptr;
  if (pattern == NULL || pattern[0] == 0) return true;
  ptr = strchr(pattern, '*');
  if (!ptr) return strcmp(name, pattern) == 0;
  if (ptr != name && strncmp(name, pattern, ptr - pattern) != 0) return false;
  return strcmp(name + strlen(name) - strlen(ptr + 1), ptr + 1) == 0;
}

TCOD_list_t TCOD_sys_get_directory_content(const char* path, const char* pattern) {
  TCOD_list_t list = TCOD_list_new();
#ifdef TCOD_WINDOWS
  WIN32_FIND_DATA FileData;
  HANDLE hList;
  char dname[512];
  snprintf(dname, sizeof(dname), "%s\\*", path);
  hList = FindFirstFile(dname, &FileData);
  if (hList == INVALID_HANDLE_VALUE) {
    return list;
  }
  do {
    if (!(strcmp(FileData.cFileName, ".") == 0 || strcmp(FileData.cFileName, "..") == 0)) {
      if (filename_match(FileData.cFileName, pattern)) TCOD_list_push(list, TCOD_strdup(FileData.cFileName));
    }

  } while (FindNextFile(hList, &FileData));
  FindClose(hList);
#else
  DIR* dir = opendir(path);
  struct dirent* dirent = NULL;
  if (!dir) return list;
  while ((dirent = readdir(dir))) {
    if (!(strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0)) {
      if (filename_match(dirent->d_name, pattern)) TCOD_list_push(list, TCOD_strdup(dirent->d_name));
    }
  }
  closedir(dir);
#endif
  return list;
}

/* thread stuff */

int TCOD_sys_get_num_cores(void) {
#ifndef NO_SDL
  return SDL_GetNumLogicalCPUCores();
#else
  return 1;
#endif  // NO_SDL
}
#ifndef TCOD_NO_THREADS
TCOD_thread_t TCOD_thread_new(int (*func)(void*), void* data) {
#ifdef TCOD_WINDOWS
  HANDLE ret = CreateThread(NULL, 0, (DWORD(WINAPI*)(LPVOID))func, data, 0, NULL);
  return ret;
#else
  pthread_t id;
  int i_ret;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  i_ret = pthread_create(&id, &attr, (void* (*)(void*))func, data);
  if (i_ret != 0) id = 0;
  return (TCOD_thread_t)id;
#endif
}

void TCOD_thread_delete(TCOD_thread_t th) {
#ifdef TCOD_WINDOWS
  CloseHandle(th);
#endif
}

void TCOD_thread_wait(TCOD_thread_t th) {
#ifdef TCOD_WINDOWS
  WaitForSingleObject(th, INFINITE);
#else
  pthread_t id = (pthread_t)th;
  pthread_join(id, NULL);
#endif
}

TCOD_mutex_t TCOD_mutex_new() {
#ifdef TCOD_WINDOWS
  CRITICAL_SECTION* cs = calloc(1, sizeof(CRITICAL_SECTION));
  InitializeCriticalSection(cs);
  return cs;
#else
  static pthread_mutex_t tmp = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t* mut = calloc(1, sizeof(pthread_mutex_t));
  *mut = tmp;
  return (TCOD_mutex_t)mut;
#endif
}

void TCOD_mutex_in(TCOD_mutex_t mut) {
#ifdef TCOD_WINDOWS
  EnterCriticalSection((CRITICAL_SECTION*)mut);
#else
  pthread_mutex_lock((pthread_mutex_t*)mut);
#endif
}

void TCOD_mutex_out(TCOD_mutex_t mut) {
#ifdef TCOD_WINDOWS
  LeaveCriticalSection((CRITICAL_SECTION*)mut);
#else
  pthread_mutex_unlock((pthread_mutex_t*)mut);
#endif
}

void TCOD_mutex_delete(TCOD_mutex_t mut) {
#ifdef TCOD_WINDOWS
  DeleteCriticalSection((CRITICAL_SECTION*)mut);
  free(mut);
#else
  pthread_mutex_destroy((pthread_mutex_t*)mut);
  free(mut);
#endif
}

TCOD_semaphore_t TCOD_semaphore_new(int initVal) {
#ifdef TCOD_WINDOWS
  HANDLE ret = CreateSemaphore(NULL, initVal, 255, NULL);
  return ret;
#else
  sem_t* ret = calloc(1, sizeof(sem_t));
  if (ret) sem_init(ret, 0, initVal);
  return (TCOD_semaphore_t)ret;
#endif
}

void TCOD_semaphore_lock(TCOD_semaphore_t sem) {
#ifdef TCOD_WINDOWS
  WaitForSingleObject(sem, INFINITE);
#else
  if (sem) sem_wait((sem_t*)sem);
#endif
}

void TCOD_semaphore_unlock(TCOD_semaphore_t sem) {
#ifdef TCOD_WINDOWS
  ReleaseSemaphore(sem, 1, NULL);
#else
  if (sem) sem_post((sem_t*)sem);
#endif
}

void TCOD_semaphore_delete(TCOD_semaphore_t sem) {
#ifdef TCOD_WINDOWS
  CloseHandle(sem);
#else
  if (sem) {
    sem_destroy((sem_t*)sem);
    free(sem);
  }
#endif
}

#ifdef TCOD_WINDOWS
/* poor win32 API has no thread conditions */
typedef struct {
  int nbSignals;
  int nbWaiting;
  TCOD_mutex_t mutex;
  TCOD_semaphore_t waiting;
  TCOD_semaphore_t waitDone;
} cond_t;
#endif

TCOD_cond_t TCOD_condition_new(void) {
#ifdef TCOD_WINDOWS
  cond_t* ret = calloc(1, sizeof(cond_t));
  ret->mutex = TCOD_mutex_new();
  ret->waiting = TCOD_semaphore_new(0);
  ret->waitDone = TCOD_semaphore_new(0);
  return ret;
#else
  pthread_cond_t* ret = calloc(1, sizeof(pthread_cond_t));
  if (ret) pthread_cond_init(ret, NULL);
  return (TCOD_cond_t)ret;
#endif
}

void TCOD_condition_signal(TCOD_cond_t p_cond) {
#ifdef TCOD_WINDOWS
  cond_t* cond = p_cond;
  if (cond) {
    TCOD_mutex_in(cond->mutex);
    if (cond->nbWaiting > cond->nbSignals) {
      cond->nbSignals++;
      TCOD_semaphore_unlock(cond->waiting);
      TCOD_mutex_out(cond->mutex);
      TCOD_semaphore_lock(cond->waitDone);
    } else {
      TCOD_mutex_out(cond->mutex);
    }
  }
#else
  if (p_cond) {
    pthread_cond_signal((pthread_cond_t*)p_cond);
  }
#endif
}

void TCOD_condition_broadcast(TCOD_cond_t p_cond) {
#ifdef TCOD_WINDOWS
  cond_t* cond = p_cond;
  if (cond) {
    TCOD_mutex_in(cond->mutex);
    if (cond->nbWaiting > cond->nbSignals) {
      int nbUnlock = cond->nbWaiting - cond->nbSignals;
      int i;
      cond->nbSignals = cond->nbWaiting;
      for (i = nbUnlock; i > 0; i--) {
        TCOD_semaphore_unlock(cond->waiting);
      }
      TCOD_mutex_out(cond->mutex);
      for (i = nbUnlock; i > 0; i--) {
        TCOD_semaphore_lock(cond->waitDone);
      }
    } else {
      TCOD_mutex_out(cond->mutex);
    }
  }
#else
  if (p_cond) {
    pthread_cond_broadcast((pthread_cond_t*)p_cond);
  }
#endif
}

void TCOD_condition_wait(TCOD_cond_t p_cond, TCOD_mutex_t mut) {
#ifdef TCOD_WINDOWS
  cond_t* cond = p_cond;
  if (cond) {
    TCOD_mutex_in(cond->mutex);
    cond->nbWaiting++;
    TCOD_mutex_out(cond->mutex);
    TCOD_mutex_out(mut);
    TCOD_semaphore_lock(cond->waiting);
    TCOD_mutex_in(cond->mutex);
    if (cond->nbSignals > 0) {
      TCOD_semaphore_unlock(cond->waitDone);
      cond->nbSignals--;
    }
    cond->nbWaiting--;
    TCOD_mutex_out(cond->mutex);
  }
#else
  if (p_cond && mut) {
    pthread_cond_wait((pthread_cond_t*)p_cond, (pthread_mutex_t*)mut);
  }
#endif
}

void TCOD_condition_delete(TCOD_cond_t p_cond) {
#ifdef TCOD_WINDOWS
  cond_t* cond = p_cond;
  if (cond) {
    TCOD_mutex_delete(cond->mutex);
    TCOD_semaphore_delete(cond->waiting);
    TCOD_semaphore_delete(cond->waitDone);
    free(cond);
  }
#else
  if (p_cond) {
    pthread_cond_destroy((pthread_cond_t*)p_cond);
    free(p_cond);
  }
#endif
}
#endif  // TCOD_NO_THREADS
void TCOD_sys_get_fullscreen_offsets(int* offset_x, int* offset_y) {
  if (offset_x) *offset_x = TCOD_ctx.fullscreen_offset_x;
  if (offset_y) *offset_y = TCOD_ctx.fullscreen_offset_y;
}
#ifndef NO_SDL
/* dynamic library support */
TCOD_library_t TCOD_load_library(const char* path) { return SDL_LoadObject(path); }
void* TCOD_get_function_address(TCOD_library_t library, const char* function_name) {
  return SDL_LoadFunction(library, function_name);
}
void TCOD_close_library(TCOD_library_t library) { SDL_UnloadObject(library); }
#endif  // NO_SDL
/**
    Load a file into memory.  Returns NULL on failure.

    The returned buffer must be freed by the caller with free()
 */
TCOD_NODISCARD unsigned char* TCOD_load_binary_file_(const char* path, size_t* size) {
  if (!path) {
    TCOD_set_errorv("Given path was NULL.");
    return NULL;
  }
  FILE* file = fopen(path, "rb");
  if (!file) {
    TCOD_set_errorvf("Could not open file:\n%s", path);
    return NULL;
  }
  fseek(file, 0, SEEK_END);
  size_t fsize = (size_t)ftell(file);
  fseek(file, 0, SEEK_SET);
  unsigned char* buffer = malloc(fsize);
  if (!buffer) {
    TCOD_set_errorvf("Could not allocate %ld bytes for file.", fsize);
  } else {
    if (fread(buffer, 1, fsize, file) != fsize) {
      // Shouldn't happen, but just in case.
      TCOD_set_errorv("Could not determine a files size.");
      free(buffer);
      buffer = NULL;
    }
  }
  fclose(file);
  if (size) *size = fsize;
  return buffer;
}
