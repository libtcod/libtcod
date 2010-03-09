#!/bin/sh
mkdir -p release 
cd release
cmake -DCMAKE_BUILD_TYPE=Release -G "MSYS Makefiles" ../../
cd .. 

mkdir -p debug 
cd debug 
cmake -DCMAKE_BUILD_TYPE=Debug -G "MSYS Makefiles" ../../
cd .. 
