#!/bin/bash
set -e
export VALGRIND=""
export VALGRIND_PY=""
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    VALGRIND="valgrind --gen-suppressions=all --error-exitcode=1"
    VALGRIND_PY="PYTHONMALLOC='malloc' $VALGRIND --show-leak-kinds=definite --suppressions=$TRAVIS_BUILD_DIR/.ci/python.supp"
fi

if [[ "$BUILD_TOOL" == "autotools" ]]; then
    echo "#include <libtcod.h>" | $CC -xc -c -I$HOME/.local/include/libtcod -
    echo "#include <libtcod.hpp>" | $CC -xc++ -std=c++14 -c -I$HOME/.local/include/libtcod -
    echo "#include <gui/gui.hpp>" | $CC -xc++ -std=c++14 -c -I$HOME/.local/include/libtcod -
    (cd buildsys/autotools && make check)
elif [[ "$BUILD_TOOL" == "scons" ]]; then
    env LD_LIBRARY_PATH="$TRAVIS_BUILD_DIR" $VALGRIND ./unittest ~[!nonportable]
elif [[ "$BUILD_TOOL" == "conan" ]]; then
    export CONAN_SYSREQUIRES_MODE=enabled
    .ci/conan_build.py
fi
if [[ "$BUILD_TOOL" != "conan" ]]; then
    (cd python && env $VALGRIND_PY pytest -v)
fi
