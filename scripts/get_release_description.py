#!/usr/bin/env python3
"""Print the description used for GitHub Releases."""
from __future__ import annotations

import re
from pathlib import Path

TAG_BANNER = r"## \[\d+\.\d+\.\d+\S*\] - \d+-\d+-\d+\n"

RE_BODY = re.compile(rf".*?{TAG_BANNER}(.*?){TAG_BANNER}", re.DOTALL)
RE_SECTION = re.compile(r"^### (\w+)$", re.MULTILINE)


def main() -> None:
    # Get the most recent tag.
    match = RE_BODY.match(Path("CHANGELOG.md").read_text(encoding="utf-8"))
    assert match
    body = match.groups()[0].strip()

    # Add Markdown formatting to sections.
    body = RE_SECTION.sub(r"### \1", body)
    print(body)


if __name__ == "__main__":
    main()
