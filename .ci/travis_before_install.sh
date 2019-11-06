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
