Instructions on how to build these sample files:

Install a common set of build tools such as [Microsoft Visual Studio](https://visualstudio.microsoft.com/vs/community/).
This will be used by Conan.

[Install Conan](https://docs.conan.io/en/latest/installation.html).

The `bincrafters` remote is for SDL2.  The `hexdecimal` remote is for
libtcod.  Add these remotes to Conan from the command line:

    conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
    conan remote add hexdecimal https://api.bintray.com/conan/hexdecimal/conan

From the command line, switch to this directory and run the following:

    conan install . -if build
    conan build . -bf build

`conan install` will download the dependencies required by the samples.
`conan build` will build the samples themselves.

You can now run the executable samples in `./build/bin`.
