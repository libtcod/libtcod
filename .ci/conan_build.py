#!/usr/bin/env python3
"""Build script for conan-package-tools:
https://github.com/conan-io/conan-package-tools
"""
import os
import subprocess

from cpt.packager import ConanMultiPackager

try:
    version = subprocess.check_output(
        ["git", "describe", "--abbrev=0"], universal_newlines=True
    )
except subprocess.CalledProcessError:
    version = "0.0"

if __name__ == "__main__":
    if "CI" in os.environ:
        os.environ["CONAN_SYSREQUIRES_MODE"] = "enabled"
        # Fix GitHub Actions version tag.
        if os.environ.get("GITHUB_REF", "").startswith("refs/tags/"):
            version = os.environ["GITHUB_REF"].replace("refs/tags/", "")

    builder = ConanMultiPackager(
        username="hexdecimal",
        channel="conan",
        upload="https://api.bintray.com/conan/hexdecimal/conan",
        upload_only_when_tag=True,
        reference="libtcod/" + version,
        remotes=[
            "https://conan.bintray.com",
            "https://api.bintray.com/conan/bincrafters/public-conan",
        ],
        cppstds=["17"],
        visual_runtimes=["MD", "MDd"],
        # test_folder="tests/",
        build_policy="missing",
        upload_dependencies="all",
    )
    builder.add_common_builds(pure_c=False)
    builder.run()
