#!/bin/bash
set -e
export VALGRIND=""
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    VALGRIND="PYTHONMALLOC='malloc' valgrind --show-leak-kinds=definite --gen-suppressions=all --error-exitcode=0"
fi

if [[ "$BUILD_TOOL" == "autotools" ]]; then
    echo "#include <libtcod.h>" | $CC -xc -c -I$HOME/.local/include/libtcod -
    echo "#include <libtcod.hpp>" | $CC -xc++ -std=c++14 -c -I$HOME/.local/include/libtcod -
    echo "#include <gui/gui.hpp>" | $CC -xc++ -std=c++14 -c -I$HOME/.local/include/libtcod -
    (cd buildsys/autotools && make check)
elif [[ "$BUILD_TOOL" == "scons" ]]; then
    env LD_LIBRARY_PATH="$TRAVIS_BUILD_DIR" $VALGRIND ./unittest ~[!nonportable]
elif [[ "$BUILD_TOOL" == "conan" ]]; then
    .ci/conan_build.py
fi
if [[ "$BUILD_TOOL" != "conan" ]]; then
    (cd python && env $VALGRIND pytest -v)
fi
