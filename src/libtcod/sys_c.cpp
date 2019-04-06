/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
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
#include "sys.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <exception>
#include <string.h>
#include <sys/stat.h>

#include "libtcod_int.h"
#include "version.h"
#ifdef TCOD_WINDOWS
#define NOMINMAX 1
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <dlfcn.h>
#endif

bool TCOD_sys_create_directory(const char *path) {
#ifdef TCOD_WINDOWS
	return (CreateDirectory(path,NULL) != 0 || GetLastError() == ERROR_ALREADY_EXISTS);
#else
	return mkdir(path,0755) == 0 || errno == EEXIST;
#endif
}

bool TCOD_sys_delete_file(const char *path) {
#ifdef TCOD_WINDOWS
	return DeleteFile(path) != 0;
#else
	return unlink(path) == 0 || errno == ENOENT;
#endif
}

bool TCOD_sys_delete_directory(const char *path) {
#ifdef TCOD_WINDOWS
	return RemoveDirectory(path) != 0;
#else
	return rmdir(path) == 0 || errno == ENOENT;
#endif
}


bool TCOD_sys_is_directory(const char *path) {
#ifdef TCOD_WINDOWS
	DWORD type=GetFileAttributes(path);
	return ( type & FILE_ATTRIBUTE_DIRECTORY) != 0 ;
#else
	DIR *d=opendir(path);
	if ( d ) { closedir(d); return true; }
	return false;
#endif
}


static bool filename_match(const char *name, const char *pattern) {
	const char *ptr;
	if ( pattern == NULL || pattern[0] == 0 ) return true;
	ptr=strchr(pattern,'*');
	if ( ! ptr ) return strcmp(name,pattern) == 0;
	if ( ptr != name && strncmp(name,pattern, ptr - pattern) != 0 ) return false;
	return  strcmp( name + strlen(name) - strlen(ptr+1), ptr+1) == 0;
}

TCOD_list_t TCOD_sys_get_directory_content(const char *path, const char *pattern) {
    TCOD_list_t list=TCOD_list_new();
#ifdef TCOD_WINDOWS
    WIN32_FIND_DATA FileData;
    HANDLE          hList;
	char dname[ 512 ];
	sprintf(dname, "%s\\*",path);
    hList = FindFirstFile(dname, &FileData);
    if (hList == INVALID_HANDLE_VALUE)
    {
        return list;
    }
	do
	{
		if ( ! (strcmp(FileData.cFileName,".") == 0 || strcmp(FileData.cFileName,"..") == 0 ) )
		{
			if ( filename_match(FileData.cFileName,pattern) )
				TCOD_list_push(list,TCOD_strdup(FileData.cFileName));
		}

	} while ( FindNextFile(hList, &FileData) );
    FindClose(hList);
#else
    DIR *dir = opendir(path);
    struct dirent *dirent = NULL;
    if ( ! dir ) return list;
    while ( ( dirent = readdir(dir) ) )
    {
		if ( ! (strcmp(dirent->d_name,".") == 0 || strcmp(dirent->d_name,"..") == 0 ) )
		{
			if ( filename_match(dirent->d_name,pattern) )
				TCOD_list_push(list,TCOD_strdup(dirent->d_name));
		}
	}
	closedir(dir);
#endif
	return list;
}

/* thread stuff */
#ifdef TCOD_WINDOWS
/* Helper function to count set bits in the processor mask. */
static DWORD CountSetBits(ULONG_PTR bitMask)
{
    DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
    DWORD bitSetCount = 0;
    ULONG_PTR bitTest = static_cast<ULONG_PTR>(1) << LSHIFT;
    DWORD i;

    for (i = 0; i <= LSHIFT; ++i)
    {
        bitSetCount += ((bitMask & bitTest)?1:0);
        bitTest/=2;
    }

    return bitSetCount;
}
#endif

int TCOD_sys_get_num_cores(void) {
#ifdef TCOD_WINDOWS
	/* what a crap !!! works only on xp sp3 & vista */
	typedef enum _PROCESSOR_CACHE_TYPE {
	  CacheUnified,
	  CacheInstruction,
	  CacheData,
	  CacheTrace
	} PROCESSOR_CACHE_TYPE;

	typedef struct _CACHE_DESCRIPTOR {
	  BYTE                   Level;
	  BYTE                   Associativity;
	  WORD                   LineSize;
	  DWORD                  Size;
	  PROCESSOR_CACHE_TYPE   Type;
	} CACHE_DESCRIPTOR;
	typedef enum _LOGICAL_PROCESSOR_RELATIONSHIP {
	  RelationProcessorCore,
	  RelationNumaNode,
	  RelationCache,
	  RelationProcessorPackage
	} LOGICAL_PROCESSOR_RELATIONSHIP;

	typedef struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION {
	  ULONG_PTR                        ProcessorMask;
	  LOGICAL_PROCESSOR_RELATIONSHIP   Relationship;
	  union {
	    struct {
	      BYTE Flags;
	    } ProcessorCore;
	    struct {
	      DWORD NodeNumber;
	    } NumaNode;
	    CACHE_DESCRIPTOR Cache;
	    ULONGLONG Reserved[2];
	  };
	} SYSTEM_LOGICAL_PROCESSOR_INFORMATION,
	 *PSYSTEM_LOGICAL_PROCESSOR_INFORMATION;
	typedef BOOL (WINAPI *LPFN_GLPI)(
	    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION,
	    PDWORD);

    LPFN_GLPI glpi;
    BOOL done = FALSE;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
    DWORD returnLength = 0;
    DWORD logicalProcessorCount = 0;
    DWORD byteOffset = 0;

    glpi = reinterpret_cast<LPFN_GLPI>(
        GetProcAddress(GetModuleHandle(TEXT("kernel32")),
                       "GetLogicalProcessorInformation"));
    if (! glpi) {
        return 1;
    }

    while (!done) {
        DWORD rc = glpi(buffer, &returnLength);

        if (FALSE == rc)
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                if (buffer)
                    free(buffer);

                buffer = static_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(
                    malloc(returnLength));

                if (NULL == buffer) {
                    return 1;
                }
            } else {
                return 1;
            }
        } else {
            done = TRUE;
        }
    }

    ptr = buffer;

    while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength) {
        switch (ptr->Relationship) {
        case RelationProcessorCore:
            /* A hyperthreaded core supplies more than one logical processor. */
            logicalProcessorCount += CountSetBits(ptr->ProcessorMask);
            break;
        default: break;
        }
        byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }

    free(buffer);

    return logicalProcessorCount;
#else
	return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

TCOD_thread_t TCOD_thread_new(int (*func)(void *), void *data)
{
#ifdef TCOD_WINDOWS
	HANDLE ret = CreateThread(
      NULL, 0, reinterpret_cast<DWORD (WINAPI*)(LPVOID)>(func), data, 0, NULL);
	return ret;
#else
	pthread_t id;
	int iret;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	iret =pthread_create(
      &id, &attr, reinterpret_cast<void *(*)(void *)>(func), data);
	if ( iret != 0 ) id=0;
	return reinterpret_cast<TCOD_thread_t>(id);
#endif
}

void TCOD_thread_delete(TCOD_thread_t th)
{
#ifdef TCOD_WINDOWS
	CloseHandle(th);
#endif
}

void TCOD_thread_wait(TCOD_thread_t th) {
#ifdef TCOD_WINDOWS
	WaitForSingleObject(th, INFINITE);
#else
	pthread_t id = reinterpret_cast<pthread_t>(th);
	pthread_join(id,NULL);
#endif
}

TCOD_mutex_t TCOD_mutex_new()
{
#ifdef TCOD_WINDOWS
	CRITICAL_SECTION *cs =
      static_cast<CRITICAL_SECTION*>(calloc(sizeof(CRITICAL_SECTION), 1));
	InitializeCriticalSection(cs);
	return static_cast<TCOD_mutex_t>(cs);
#else
	static pthread_mutex_t tmp=PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t *mut =
      static_cast<pthread_mutex_t*>(calloc(sizeof(pthread_mutex_t), 1));
	*mut = tmp;
	return static_cast<TCOD_mutex_t>(mut);
#endif
}

void TCOD_mutex_in(TCOD_mutex_t mut)
{
#ifdef TCOD_WINDOWS
	EnterCriticalSection(static_cast<CRITICAL_SECTION*>(mut));
#else
	pthread_mutex_lock(static_cast<pthread_mutex_t*>(mut));
#endif
}

void TCOD_mutex_out(TCOD_mutex_t mut)
{
#ifdef TCOD_WINDOWS
	LeaveCriticalSection(static_cast<CRITICAL_SECTION*>(mut));
#else
	pthread_mutex_unlock(static_cast<pthread_mutex_t*>(mut));
#endif
}

void TCOD_mutex_delete(TCOD_mutex_t mut)
{
#ifdef TCOD_WINDOWS
	DeleteCriticalSection(static_cast<CRITICAL_SECTION*>(mut));
	free(mut);
#else
	pthread_mutex_destroy(static_cast<pthread_mutex_t*>(mut));
	free(mut);
#endif
}

TCOD_semaphore_t TCOD_semaphore_new(int initVal)
{
#ifdef TCOD_WINDOWS
	HANDLE ret = CreateSemaphore(NULL,initVal,255,NULL);
	return ret;
#else
	sem_t *ret = static_cast<sem_t*>(calloc(sizeof(sem_t), 1));
	if ( ret ) sem_init(ret, 0, initVal);
	return static_cast<TCOD_semaphore_t>(ret);
#endif
}

void TCOD_semaphore_lock(TCOD_semaphore_t sem)
{
#ifdef TCOD_WINDOWS
	WaitForSingleObject(sem, INFINITE);
#else
	if ( sem ) sem_wait(static_cast<sem_t*>(sem));
#endif
}

void TCOD_semaphore_unlock(TCOD_semaphore_t sem)
{
#ifdef TCOD_WINDOWS
	ReleaseSemaphore(sem, 1, NULL);
#else
	if ( sem ) sem_post(static_cast<sem_t*>(sem));
#endif
}

void TCOD_semaphore_delete( TCOD_semaphore_t sem)
{
#ifdef TCOD_WINDOWS
	CloseHandle(sem);
#else
	if ( sem )
	{
		sem_destroy(static_cast<sem_t*>(sem));
		free (sem);
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
	cond_t *ret = static_cast<cond_t*>(calloc(sizeof(cond_t), 1));
	ret->mutex = TCOD_mutex_new();
	ret->waiting = TCOD_semaphore_new(0);
	ret->waitDone = TCOD_semaphore_new(0);
	return ret;
#else
	pthread_cond_t *ret =
    static_cast<pthread_cond_t*>(calloc(sizeof(pthread_cond_t), 1));
	if ( ret ) pthread_cond_init(ret,NULL);
	return static_cast<TCOD_cond_t>(ret);
#endif
}

void TCOD_condition_signal(TCOD_cond_t pcond) {
#ifdef TCOD_WINDOWS
	cond_t* cond = static_cast<cond_t*>(pcond);
	if ( cond ) {
		TCOD_mutex_in(cond->mutex);
		if ( cond->nbWaiting > cond->nbSignals ) {
			cond->nbSignals++;
			TCOD_semaphore_unlock(cond->waiting);
			TCOD_mutex_out(cond->mutex);
			TCOD_semaphore_lock(cond->waitDone);
		} else {
			TCOD_mutex_out(cond->mutex);
		}
	}
#else
	if ( pcond ) {
		pthread_cond_signal(static_cast<pthread_cond_t*>(pcond));
	}
#endif
}

void TCOD_condition_broadcast(TCOD_cond_t pcond) {
#ifdef TCOD_WINDOWS
	cond_t *cond = static_cast<cond_t*>(pcond);
	if ( cond ) {
		TCOD_mutex_in(cond->mutex);
		if ( cond->nbWaiting > cond->nbSignals ) {
			int nbUnlock=cond->nbWaiting-cond->nbSignals;
			int i;
			cond->nbSignals=cond->nbWaiting;
			for (i=nbUnlock; i > 0; i--) {
				TCOD_semaphore_unlock(cond->waiting);
			}
			TCOD_mutex_out(cond->mutex);
			for (i=nbUnlock; i > 0; i--) {
				TCOD_semaphore_lock(cond->waitDone);
			}
		} else {
			TCOD_mutex_out(cond->mutex);
		}
	}
#else
	if ( pcond ) {
		pthread_cond_broadcast(static_cast<pthread_cond_t*>(pcond));
	}
#endif
}

void TCOD_condition_wait(TCOD_cond_t pcond, TCOD_mutex_t mut) {
#ifdef TCOD_WINDOWS
	cond_t *cond = static_cast<cond_t*>(pcond);
	if ( cond ) {
		TCOD_mutex_in(cond->mutex);
		cond->nbWaiting++;
		TCOD_mutex_out(cond->mutex);
		TCOD_mutex_out(mut);
		TCOD_semaphore_lock(cond->waiting);
		TCOD_mutex_in(cond->mutex);
		if ( cond->nbSignals > 0 ) {
			TCOD_semaphore_unlock(cond->waitDone);
			cond->nbSignals--;
		}
		cond->nbWaiting--;
		TCOD_mutex_out(cond->mutex);
	}
#else
	if ( pcond && mut ) {
		pthread_cond_wait(static_cast<pthread_cond_t*>(pcond),
                      static_cast<pthread_mutex_t*>(mut));
	}
#endif
}

void TCOD_condition_delete( TCOD_cond_t pcond) {
#ifdef TCOD_WINDOWS
  cond_t* cond = static_cast<cond_t*>(pcond);
	if ( cond ) {
		TCOD_mutex_delete(cond->mutex);
		TCOD_semaphore_delete(cond->waiting);
		TCOD_semaphore_delete(cond->waitDone);
		free(cond);
	}
#else
	if ( pcond ) {
    pthread_cond_destroy(static_cast<pthread_cond_t*>(pcond));
		free (pcond);
	}
#endif
}

#ifndef TCOD_BARE
void TCOD_sys_get_fullscreen_offsets(int *offx, int *offy) {
	if ( offx ) *offx = TCOD_ctx.fullscreen_offsetx;
	if ( offy ) *offy = TCOD_ctx.fullscreen_offsety;
}
#else
void TCOD_sys_startup(void) {
	//TCOD_ctx.max_font_chars = 256;
	//alloc_ascii_tables();
}

void TCOD_sys_shutdown(void) {
}

bool TCOD_sys_read_file(const char *filename, unsigned char **buf, size_t *size) {
	return false;
}

bool TCOD_sys_write_file(const char *filename, unsigned char *buf, uint32_t size) {
	return false;
}
#endif /* TCOD_BARE */
/**
 *  Print formatted text as an error and then forcefully terminate the program.
 */
void TCOD_fatal(const char *fmt, ...)
{
  va_list ap;
  TCOD_sys_shutdown();
  fprintf(stderr, "%s FATAL ERROR:\n", TCOD_STRVERSIONNAME);
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fprintf(stderr, "\n");
  std::terminate();
}
void TCOD_fatal_nopar(const char *msg)
{
  TCOD_fatal("%s", msg);
}

/* dynamic library support */
#ifdef TCOD_WINDOWS
TCOD_library_t TCOD_load_library(const char *path)
{
  return static_cast<TCOD_library_t>(LoadLibrary(path));
}
void* TCOD_get_function_address(
    TCOD_library_t library, const char *function_name)
{
  return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(library),
                                                function_name));
}
void TCOD_close_library(TCOD_library_t library)
{
  FreeLibrary(static_cast<HMODULE>(library));
}
#else
TCOD_library_t TCOD_load_library(const char *path)
{
  void *l = dlopen(path, RTLD_LAZY);
  return l;
}
void * TCOD_get_function_address(TCOD_library_t library, const char *function_name) {
	return dlsym(library, function_name);
}
void TCOD_close_library(TCOD_library_t library) {
	dlclose(library);
}
#endif
