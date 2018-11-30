# MODE options are DEBUG, SIZE, PERFORMANCE, and RELEASE.

GCC_CC_WARNINGS = [
    '-Wall',
    '-Wextra',
    '-Wpedantic',
    '-Wshadow=local',
    '-Wcast-align',
    '-Wduplicated-cond',
    '-Wduplicated-branches',
    '-Wlogical-op',
    '-Wnull-dereference',
    '-Wdouble-promotion',
    '-Wformat=2',
]

GCC_CXX_WARNINGS = [
    '-Wnon-virtual-dtor',
    '-Wold-style-cast',
    '-Woverloaded-virtual',
    '-Wuseless-cast',
]

DEBUG_GCC = {
    #'CFLAGS': ['-Wstrict-prototypes'],
    'CCFLAGS': GCC_CC_WARNINGS + [
        '-g',
        '-fvisibility=hidden'
    ],
    'CXXFLAGS': GCC_CXX_WARNINGS,
}
DEBUG_MSVC = {
    'CCFLAGS': ['/nologo', '-W3'],
    'LINKFLAGS': ['/nologo'],
}

SIZE_GCC = {
    'CCFLAGS': GCC_CC_WARNINGS + [
        '-Os',
        '-flto',
        '-fvisibility=hidden'
    ],
    'CXXFLAGS': GCC_CXX_WARNINGS,
    'LINKFLAGS': ['-Os', '-flto'],
}
SIZE_MSVC = {
    'CCFLAGS': ['/nologo', '-W3', '-O1', '-GL', '-GS-'],
    'LINKFLAGS': ['/nologo', '-LTCG'],
}

PERFORMANCE_GCC = {
    'CCFLAGS': GCC_CC_WARNINGS + [
        '-O3',
        '-flto',
        '-fvisibility=hidden'
    ],
    'CXXFLAGS': GCC_CXX_WARNINGS,
    'LINKFLAGS': ['-O3', '-flto'],
}
PERFORMANCE_MSVC = {
    'CCFLAGS': ['/nologo', '-W3', '-O2', '-GL', '-GS-'],
    'LINKFLAGS': ['/nologo', '-LTCG'],
}

RELEASE_GCC = PERFORMANCE_GCC
RELEASE_MSVC = PERFORMANCE_MSVC
