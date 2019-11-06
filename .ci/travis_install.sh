#!/bin/bash
if [[ "$BUILD_TOOL" == "scons" ]]; then
    BUILDMODE="MODE=DEBUG_RELEASE"
    cd build/scons
    scons develop_all dist -j 3 -s CPPDEFINES=NDEBUG ARCH=x86_64 $BUILDMODE || exit 1
    cd ../..
elif [[ "$BUILD_TOOL" == "autotools" ]]; then
    cd build/autotools && autoreconf --install && ./configure --prefix=$HOME/.local && make -j 3 install || exit 1
    cd ../..
    export LIBTCOD_DLL_PATH=~/.local/lib
elif [[ "$BUILD_TOOL" == "conan" ]]; then
    .ci/conan_build.py
else
    echo "BUILD_TOOL not defined correctly, is currently: $BUILD_TOOL"
    exit 1
fi
