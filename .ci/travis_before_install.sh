#!/bin/bash
set -e

# Fix shell_session_update errors.
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    wget -O - https://rvm.io/mpapis.asc | gpg --import -
    wget -O - https://rvm.io/pkuczynski.asc | gpg --import -
    set +e
    curl -sSL https://get.rvm.io | bash
    rvm reload
    rvm get 1.29.8
fi

set -e

# Start X11 display on Linux
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    if [[ "$BUILD_TOOL" != "conan" ]]; then
        # Update SDL2 to a recent version.
        wget -O - https://www.libsdl.org/release/SDL2-2.0.8.tar.gz | tar xz
        (cd SDL2-* && ./configure --prefix=$HOME/.local && make -j 3 install)
        PATH=~/.local/bin:$PATH
    fi
fi

# Install SCons on MacOS via pip
if [[ "$TRAVIS_OS_NAME" == "osx" && "$BUILD_TOOL" != "conan" ]]; then
    wget https://bootstrap.pypa.io/get-pip.py
    python get-pip.py --user
    python -m pip install --user virtualenv
    python -m virtualenv ~/venv
    source ~/venv/bin/activate
    if [[ "$BUILD_TOOL" == "scons" ]]; then
        python -m pip install scons
    elif [[ "$BUILD_TOOL" == "autotools" ]]; then
        HOMEBREW_NO_AUTO_UPDATE=1 brew install sdl2
    fi
fi

if [[ "$BUILD_TOOL" == "conan" ]]; then
    pip3 install -U conan conan_package_tools
fi
