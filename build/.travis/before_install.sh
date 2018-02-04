#!/bin/bash

# Start X11 display on Linux
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    export DISPLAY=:99.0
    sh -e /etc/init.d/xvfb start
fi

# Install SCons on MacOS via pip
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    sudo easy_install virtualenv
    python -m virtualenv ~/venv
    source ~/venv/bin/activate
    if [[ "$BUILD_TOOL" == "scons" ]]; then
        pip install --egg scons
    elif [[ "$BUILD_TOOL" == "autotools" ]]; then
        brew install sdl2
    fi
fi
