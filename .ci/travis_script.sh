#!/bin/bash
set -e
if [[ "$BUILD_TOOL" == "autotools" ]]; then
    echo "#include <libtcod.h>" | $CC -xc -c -I$HOME/.local/include/libtcod -
    echo "#include <libtcod.hpp>" | $CC -xc++ -std=c++14 -c -I$HOME/.local/include/libtcod -
    echo "#include <gui/gui.hpp>" | $CC -xc++ -std=c++14 -c -I$HOME/.local/include/libtcod -
    (cd buildsys/autotools && make check)
elif [[ "$BUILD_TOOL" == "scons" ]]; then
    LD_LIBRARY_PATH=$TRAVIS_BUILD_DIR ./unittest ~[!nonportable]
elif [[ "$BUILD_TOOL" == "conan" ]]; then
    .ci/conan_build.py
fi
if [[ "$BUILD_TOOL" != "conan" ]]; then
    if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
        (cd python && pytest -v)
    else
        (cd python && PYTHONMALLOC=malloc valgrind --show-leak-kinds=definite pytest -v)
    fi
fi
