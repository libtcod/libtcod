# MODE options are DEBUG, SIZE, PERFORMANCE, and RELEASE.

GCC_CC_WARNINGS = [
    "-Wall",
    "-Wextra",
    "-Wpedantic",
    "-Wshadow=local",
    "-Wcast-align",
    "-Wduplicated-cond",
    "-Wduplicated-branches",
    "-Wlogical-op",
    "-Wnull-dereference",
    "-Wdouble-promotion",
    "-Wformat=2",
]

GCC_CXX_WARNINGS = [
    "-Wnon-virtual-dtor",
    "-Woverloaded-virtual",
    "-Wuseless-cast",
]

DEBUG_GCC = {
    "CFLAGS": ["-Wstrict-prototypes"],
    "CCFLAGS": [*GCC_CC_WARNINGS, "-g", "-fvisibility=hidden"],
    "CXXFLAGS": GCC_CXX_WARNINGS,
}
DEBUG_MSVC = {
    "CCFLAGS": ["/nologo", "/std:c++17", "-W3"],
    "LINKFLAGS": ["/nologo"],
}

SIZE_GCC = {
    "CCFLAGS": [*GCC_CC_WARNINGS, "-Os", "-flto", "-fvisibility=hidden"],
    "CXXFLAGS": GCC_CXX_WARNINGS,
    "LINKFLAGS": ["-Os", "-flto"],
    "CPPDEFINES": ["NDEBUG"],
}
SIZE_MSVC = {
    "CCFLAGS": ["/nologo", "/std:c++17", "-W3", "-O1", "-GL", "-GS-"],
    "LINKFLAGS": ["/nologo", "-LTCG"],
    "CPPDEFINES": ["NDEBUG"],
}

PERFORMANCE_GCC = {
    "CCFLAGS": [*GCC_CC_WARNINGS, "-O3", "-flto", "-fvisibility=hidden"],
    "CXXFLAGS": GCC_CXX_WARNINGS,
    "LINKFLAGS": ["-O3", "-flto"],
    "CPPDEFINES": ["NDEBUG"],
}
PERFORMANCE_MSVC = {
    "CCFLAGS": ["/nologo", "/std:c++17", "-W3", "-O2", "-GL", "-GS-"],
    "LINKFLAGS": ["/nologo", "-LTCG"],
    "CPPDEFINES": ["NDEBUG"],
}

RELEASE_GCC = PERFORMANCE_GCC
RELEASE_MSVC = PERFORMANCE_MSVC

# DEBUG_RELEASE is like RELEASE but with debug flags enabled.
DEBUG_RELEASE_GCC = {
    "CCFLAGS": RELEASE_GCC["CCFLAGS"] + ["-g"],
    "CXXFLAGS": RELEASE_GCC["CXXFLAGS"],
    "LINKFLAGS": RELEASE_GCC["LINKFLAGS"],
    "CPPDEFINES": ["NDEBUG"],
}
DEBUG_RELEASE_MSVC = {
    "CCFLAGS": RELEASE_MSVC["CCFLAGS"],
    "LINKFLAGS": RELEASE_MSVC["LINKFLAGS"],
    "CPPDEFINES": ["NDEBUG"],
}
