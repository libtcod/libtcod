# Build script for AppVeyor
if ($env:MSYSTEM) {
    # Add MSYS2 to path.
    $env:PATH='C:\msys64\' + $env:MSYSTEM + '\bin;C:\msys64\usr\bin;' + $env:PATH

    # Remove alternative MinGW installations or else SCons will use them.
    Rename-Item -path C:\MinGW -newName removed_ -Force
    Rename-Item -path C:\MinGW-w64 -newName removed2_ -Force

    # Remove the unused toolchain for this job so that time is not spent
    # updating it.
    if ($env:MSYSTEM -eq "MINGW64") {
        pacman --noconfirm -Rs mingw-w64-i686-toolchain
    }
    if ($env:MSYSTEM -eq "MINGW32") {
        pacman --noconfirm -Rs mingw-w64-x86_64-toolchain
    }
	# Remove MSYS2 scons.
	pacman --noconfirm -Rs scons

    # Update MSYS2 toolset.
    pacman --noconfirm -Syu
    pacman --noconfirm -Su
}
if ($env:APPVEYOR_REPO_TAG_NAME) {
    $env:VERSION=$env:APPVEYOR_REPO_TAG_NAME
}
if ($env:SCONSOPTS) {
    & .ci\run-scons.bat
}
if ($env:CONAN_BUILD) {
    pip install -U conan_package_tools
}
