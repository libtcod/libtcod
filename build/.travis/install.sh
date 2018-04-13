#!/bin/bash
set -e

if [[ "$BUILD_TOOL" == "scons" ]]; then
    cd build/scons
    scons develop dist -j 3 ARCH=x86_64
    cd ../..
elif [[ "$BUILD_TOOL" == "autotools" ]]; then
    cd build/autotools
    autoreconf --install
    ./configure --prefix=$HOME/.local
    make -j 3 install
    cd ../..
    export LIBTCOD_DLL_PATH=~/.local/lib
else
    echo "BUILD_TOOL not defined correctly, is currently: $BUILD_TOOL"
    exit 1
fi
