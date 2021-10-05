#!/usr/bin/env python3
"""Print the description used for GitHub Releases."""
import re

TAG_BANNER = r"## \[\d+\.\d+\.\d+\S*\] - \d+-\d+-\d+\n"

RE_BODY = re.compile(fr".*?{TAG_BANNER}(.*?){TAG_BANNER}", re.DOTALL)
RE_SECTION = re.compile(r"^### (\w+)$", re.MULTILINE)


def main() -> None:
    # Get the most recent tag.
    with open("CHANGELOG.md", "r", encoding="utf-8") as f:
        match = RE_BODY.match(f.read())
    assert match
    body = match.groups()[0].strip()

    # Add Markdown formatting to sections.
    body = RE_SECTION.sub(r"### \1", body)
    print(body)


if __name__ == "__main__":
    main()
