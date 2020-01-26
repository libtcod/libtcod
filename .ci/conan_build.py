#!/usr/bin/env python3
"""Build script for conan-package-tools:
https://github.com/conan-io/conan-package-tools
"""
import subprocess

from cpt.packager import ConanMultiPackager

if __name__ == "__main__":
    builder = ConanMultiPackager(
        username="hexdecimal",
        channel="conan",
        upload="https://api.bintray.com/conan/hexdecimal/conan",
        upload_only_when_tag=True,
        reference="libtcod/"
        + subprocess.check_output(["git", "describe"], universal_newlines=True),
        remotes=[
            "https://conan.bintray.com",
            "https://api.bintray.com/conan/bincrafters/public-conan",
        ],
        cppstds=["14"],
        visual_runtimes=["MD", "MDd"],
        test_folder="tests/",
    )
    builder.add_common_builds(pure_c=False)
    builder.run()
