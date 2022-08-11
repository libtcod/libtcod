#!/usr/bin/env python3
from __future__ import annotations

import argparse
import re
from pathlib import Path

PROJECT_DIR = Path(__file__).parents[1]  # Project directory relative to this script.
SOURCE_DIR = PROJECT_DIR / "src/libtcod"  # Libtcod main source directory.

# Source files to access.
C_SOURCE = SOURCE_DIR / "renderer_gl2.c"
FRAGMENT_SHADER = SOURCE_DIR / "renderer_gl2.glslf"
VERTEX_SHADER = SOURCE_DIR / "renderer_gl2.glslv"


def replace_shader(source: str, shader: str, var_name: str) -> str:
    shader = shader.replace("\n", r"\\n").replace('"', r"\"")
    shader = f'static const char* {var_name} = "{shader}";'
    source, count = re.subn(rf"^static const char\* {var_name} =.*$", shader, source, 1, flags=re.MULTILINE)
    assert count == 1
    return source


def main() -> None:
    parser = argparse.ArgumentParser(description="Update C sources with shader sources.")
    parser.add_argument("--check", action="store_true", help="verify sources are correct")
    parser.add_argument("--dry-run", "-n", action="store_true", help="do not update sources")
    args = parser.parse_args()

    source = original = C_SOURCE.read_text(encoding="utf-8")
    source = replace_shader(source, shader=VERTEX_SHADER.read_text(encoding="utf-8"), var_name="VERTEX_SOURCE")
    source = replace_shader(source, shader=FRAGMENT_SHADER.read_text(encoding="utf-8"), var_name="FRAGMENT_SOURCE")
    if source == original:
        print("C sources match the current shader sources.")
        return
    if args.check:
        print("C sources are not up-to-date with the shaders.")
        raise SystemExit(1)
    print("C sources will be modified.")
    if not args.check and not args.dry_run:
        C_SOURCE.write_text(source, encoding="utf-8")


if __name__ == "__main__":
    main()
