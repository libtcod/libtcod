#!/bin/sh

if [ "${MSYSTEM}" == "MINGW32" ]; then
echo "Building MSYS/MinGW makefiles."
echo "------------------------------"
BUILD_TYPE="MSYS Makefiles"
else
BUILD_TYPE=""
fi

mkdir -p release 
cd release
cmake -DCMAKE_BUILD_TYPE=Release -G "${BUILD_TYPE}" ../../
cd .. 

mkdir -p debug 
cd debug 
cmake -DCMAKE_BUILD_TYPE=Debug -G "${BUILD_TYPE}" ../../
cd .. 
