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
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    python3 -m ensurepip
    python3 -m pip install --user virtualenv
    python3 -m virtualenv ~/venv
    source ~/venv/bin/activate
    if [[ "$BUILD_TOOL" == "autotools" ]]; then
        HOMEBREW_NO_AUTO_UPDATE=1 brew install sdl2
    fi
fi

if [[ "$BUILD_TOOL" == "conan" ]]; then
    pip install -U conan_package_tools
elif [[ "$BUILD_TOOL" == "scons" ]]; then
    pip install -U scons
fi
