#!/bin/bash

mkdir -p m4

touch NEWS
touch README
touch AUTHORS
touch ChangeLog

wget https://hg.libsdl.org/SDL/raw-file/183936dd34d5/sdl2.m4 -O acinclude.m4

autoreconf -i
