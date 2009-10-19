/*
* libtcod 1.4.3
* Copyright (c) 2008,2009 Jice
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Jice ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Jice BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <ctype.h>
#include <stdlib.h>
#include "libtcod.h"
#ifdef TCOD_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#endif
#include "libtcod_int.h"


#if defined(TCOD_WINDOWS)
char *strcasestr (const char *haystack, const char *needle) {
	const char *p, *startn = 0, *np = 0;

	for (p = haystack; *p; p++) {
		if (np) {
			if (toupper(*p) == toupper(*np)) {
				if (!*++np)
					return (char *)startn;
			} else
				np = 0;
		} else if (toupper(*p) == toupper(*needle)) {
			np = needle + 1;
			startn = p;
		}
	}

	return 0;
}
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

// thread stuff
#ifdef TCOD_WINDOWS
// Helper function to count set bits in the processor mask.
static DWORD CountSetBits(ULONG_PTR bitMask)
{
    DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
    DWORD bitSetCount = 0;
    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
    DWORD i;

    for (i = 0; i <= LSHIFT; ++i)
    {
        bitSetCount += ((bitMask & bitTest)?1:0);
        bitTest/=2;
    }

    return bitSetCount;
}
#endif

int TCOD_sys_get_num_cores() {
#ifdef TCOD_WINDOWS
	// what a crap !!! works only on xp sp3 & vista
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
	} CACHE_DESCRIPTOR,
	 *PCACHE_DESCRIPTOR;
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
	  }                            ;
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

    glpi = (LPFN_GLPI) GetProcAddress(
                            GetModuleHandle(TEXT("kernel32")),
                            "GetLogicalProcessorInformation");
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

                buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(
                        returnLength);

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
            // A hyperthreaded core supplies more than one logical processor.
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
	HANDLE ret = CreateThread(NULL,0,(DWORD (WINAPI *)( LPVOID ))func,data,0,NULL);
	return (TCOD_thread_t)ret;
#else
	pthread_t id;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	int iret =pthread_create(&id,&attr,(void *(*)(void *))func,data);
	if ( iret != 0 ) id=0;
	return (TCOD_thread_t)id;
#endif
}

void TCOD_thread_delete(TCOD_thread_t th)
{
#ifdef TCOD_WINDOWS
	CloseHandle((HANDLE)th);
#endif
}

TCOD_mutex_t TCOD_mutex_new()
{
#ifdef TCOD_WINDOWS
	CRITICAL_SECTION *cs = (CRITICAL_SECTION *)calloc(sizeof(CRITICAL_SECTION),1);
	InitializeCriticalSection(cs);
	return (TCOD_mutex_t)cs;
#else
	static pthread_mutex_t tmp=PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t *mut = (pthread_mutex_t *)calloc(sizeof(pthread_mutex_t),1);
	*mut = tmp;
	return (TCOD_mutex_t)mut;
#endif
}

void TCOD_mutex_in(TCOD_mutex_t mut)
{
#ifdef TCOD_WINDOWS
	EnterCriticalSection((CRITICAL_SECTION *)mut);
#else
	pthread_mutex_lock((pthread_mutex_t *)mut);
#endif
}

void TCOD_mutex_out(TCOD_mutex_t mut)
{
#ifdef TCOD_WINDOWS
	LeaveCriticalSection((CRITICAL_SECTION *)mut);
#else
	pthread_mutex_unlock((pthread_mutex_t *)mut);
#endif
}

void TCOD_mutex_delete(TCOD_mutex_t mut)
{
#ifdef TCOD_WINDOWS
	DeleteCriticalSection((CRITICAL_SECTION *)mut);
	free(mut);
#else
	pthread_mutex_destroy((pthread_mutex_t *)mut);
	free(mut);
#endif
}

#ifndef TCOD_WINDOWS
// internal semaphore type
typedef struct {
	int value;
	pthread_cond_t cond;
	pthread_mutex_t mut;
} sem_t;
#endif

TCOD_semaphore_t TCOD_semaphore_new(int initVal)
{
#ifdef TCOD_WINDOWS
	HANDLE ret = CreateSemaphore(NULL,initVal,255,NULL);
	return (TCOD_semaphore_t)ret;
#else
	static pthread_mutex_t tmp=PTHREAD_MUTEX_INITIALIZER;
	sem_t *ret = (sem_t *)calloc(sizeof(sem_t),1);
	ret->value=initVal;
	pthread_cond_init(&ret->cond,NULL);
	ret->mut = tmp;
	return (TCOD_semaphore_t) ret;
#endif
}

void TCOD_semaphore_lock(TCOD_semaphore_t sem)
{
#ifdef TCOD_WINDOWS
	WaitForSingleObject((HANDLE)sem,INFINITE);
#else
	sem_t *s =(sem_t *)sem;
	TCOD_mutex_in(&s->mut);
	s->value--;
	if ( s->value >= 0)
	{
		TCOD_mutex_out(&s->mut);
		return;
	}
	pthread_cond_wait(&s->cond, &s->mut);
	TCOD_mutex_out(&s->mut);
#endif
}

void TCOD_semaphore_unlock(TCOD_semaphore_t sem)
{
#ifdef TCOD_WINDOWS
	ReleaseSemaphore((HANDLE)sem,1,NULL);
#else
	sem_t *s=(sem_t *)sem;
	TCOD_mutex_in(&s->mut);
	s->value++;
	if ( s->value > 0)
	{
		// no thread waiting on condition
		TCOD_mutex_out(&s->mut);
		return;
	}
	// awake a thread
	pthread_cond_signal(&s->cond);
	TCOD_mutex_out(&s->mut);
#endif
}

void TCOD_semaphore_delete( TCOD_semaphore_t sem)
{
#ifdef TCOD_WINDOWS
	CloseHandle((HANDLE)sem);
#else
	if ( sem )
	{
		sem_t *s = (sem_t *)sem;
		pthread_cond_destroy(&s->cond);
		pthread_mutex_destroy(&s->mut);
		free (sem);
	}
#endif
}
// library initialization function
#ifdef TCOD_WINDOWS
BOOL APIENTRY DllMain( HANDLE hModule, DWORD reason, LPVOID reserved) {
	switch (reason ) {
		case DLL_PROCESS_ATTACH : TCOD_sys_startup(); break;
		default : break;
	}
	return TRUE;
}
#else
	#ifndef TCOD_MACOSX
	void __attribute__ ((constructor)) DllMain() {
		TCOD_sys_startup();
	}
	#endif
#endif

