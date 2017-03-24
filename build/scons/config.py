
DEFINES = ['NDEBUG', 'TCOD_SDL2']

# MODE options are DEBUG, SIZE, PERFORMANCE, and RELEASE.

DEBUG_GCC = {
    #'CFLAGS': ['-Wstrict-prototypes'],
    'CCFLAGS': ['-Wall', '-g'],
}
DEBUG_MSVC = {'CCFLAGS': ['-W3', '-DEBUG']}

SIZE_GCC = {
    'CCFLAGS': ['-Wall', '-Os', '-flto'],
    'LINKFLAGS': ['-Os', '-flto'],
}
SIZE_MSVC = {
    'CCFLAGS': ['-W3', '-O1', '-GL', '-GS-'],
    'LINKFLAGS': ['-LTCG'],
}

PERFORMANCE_GCC = {
    'CCFLAGS': ['-Wall', '-O3', '-flto'],
    'LINKFLAGS': ['-O3', '-flto'],
}
PERFORMANCE_MSVC = {
    'CCFLAGS': ['-W3', '-O2', '-GL', '-GS-'],
    'LINKFLAGS': ['-LTCG'],
}

RELEASE_GCC = PERFORMANCE_GCC
RELEASE_MSVC = PERFORMANCE_MSVC
