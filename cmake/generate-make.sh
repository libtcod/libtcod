#!/bin/sh

if [ "$1" = "msys" ]; then
echo "Building MSYS/MinGW makefiles."
echo "------------------------------"
BUILD_TYPE="MSYS Makefiles"
elif [ "$1" = "unix" ]; then
echo "Building Unix makefiles."
echo "------------------------------"
BUILD_TYPE="Unix Makefiles"
else
echo "Not sure which makefiles to make, bailing..."
exit 1
fi

mkdir -p release 
cd release
cmake -DCMAKE_BUILD_TYPE=Release -G "${BUILD_TYPE}" ../../
cd .. 

mkdir -p debug 
cd debug 
cmake -DCMAKE_BUILD_TYPE=Debug -G "${BUILD_TYPE}" ../../
cd .. 
