#!/usr/bin/env python3
# Copyright 2025 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt
"""Amalgamate boost.int128 into a single self-contained header.

Starts from the top umbrella header and recursively inlines every internal
<boost/int128/...> include (DFS pre-order so declarations precede use).
System includes and optional external boost includes
(e.g. <boost/charconv.hpp>, guarded by __has_include) are left untouched.

A header is normally inlined once, at its first point of use, since include
guards make later includes redundant. The one subtlety is preprocessor
conditionals: an include that sits inside an #if block (deeper than the
file's own include guard) only takes effect when that condition holds. If a
header has so far only been inlined inside such a block, a later include of
it is expanded again instead of skipped, so its contents are never trapped
behind a condition the later user did not ask for. Repeated expansion is
harmless because every header carries an include guard.

Run with no arguments from anywhere in the repo to regenerate
extra/single_include/boost/int128.hpp from include/boost/int128.hpp.
"""

import argparse
import re
import sys
from pathlib import Path

INTERNAL_RE = re.compile(r'^\s*#\s*include\s*<(boost/int128(?:/[^>]+)?\.hpp)>\s*(?://.*)?$')
IF_RE = re.compile(r'^\s*#\s*(?:if|ifdef|ifndef)\b')
ENDIF_RE = re.compile(r'^\s*#\s*endif\b')
IFNDEF_RE = re.compile(r'^\s*#\s*ifndef\s+(\w+)\s*$')
DEFINE_RE = re.compile(r'^\s*#\s*define\s+(\w+)\s*$')

PREAMBLE = [
    '// boost.int128 - amalgamated single-header build.',
    '// Auto-generated: all internal boost/int128 headers inlined into one file',
    '// for use in Compiler Explorer and other single-file environments.',
    '// Do not edit by hand; regenerate with extra/amalgamate.py.',
    '//',
    '// Copyright 2025 Matt Borland',
    '// Distributed under the Boost Software License, Version 1.0.',
    '// https://www.boost.org/LICENSE_1_0.txt',
    '',
]


# Detect a classic #ifndef/#define include guard as the first significant lines
def has_include_guard(lines):
    significant = [line for line in lines if line.strip() and not line.strip().startswith('//')]
    if len(significant) < 2:
        return False
    guard = IFNDEF_RE.match(significant[0])
    definition = DEFINE_RE.match(significant[1])
    return bool(guard and definition and guard.group(1) == definition.group(1))


class Amalgamator:
    def __init__(self, include_root):
        self.include_root = include_root
        self.visited = {}  # rel path -> 'unconditional' or 'conditional'
        self.out_lines = []

    # Inline one header: emit its lines, recursing into internal includes.
    # stack holds the headers currently being expanded (cycle protection);
    # conditional is True when this expansion sits inside an #if block.
    def inline(self, rel_path, stack, conditional):
        full = self.include_root / rel_path
        if not full.exists():
            sys.exit(f'amalgamate: internal header {rel_path} not found under {self.include_root}')

        if not conditional:
            self.visited[rel_path] = 'unconditional'
        elif rel_path not in self.visited:
            self.visited[rel_path] = 'conditional'

        lines = full.read_text(encoding='utf-8').splitlines()
        guard_depth = 1 if has_include_guard(lines) else 0
        depth = 0

        self.out_lines.append(f'// ===== BEGIN {rel_path} =====')
        for line in lines:
            match = INTERNAL_RE.match(line)
            if match:
                dep = match.group(1)
                dep_conditional = conditional or depth > guard_depth
                if dep in stack or self.visited.get(dep) == 'unconditional':
                    self.out_lines.append(f'// [amalgamate] skipped duplicate include of {dep}')
                else:
                    self.inline(dep, stack + [rel_path], dep_conditional)
            else:
                if IF_RE.match(line):
                    depth += 1
                elif ENDIF_RE.match(line):
                    depth -= 1
                self.out_lines.append(line)
        self.out_lines.append(f'// ===== END {rel_path} =====')


def main():
    repo_root = Path(__file__).resolve().parent.parent
    parser = argparse.ArgumentParser(description='Amalgamate boost.int128 into a single header')
    parser.add_argument('include_root', nargs='?', type=Path, default=repo_root / 'include',
                        help='path to the include directory (default: <repo>/include)')
    parser.add_argument('top', nargs='?', default='boost/int128.hpp',
                        help='umbrella header to start from (default: boost/int128.hpp)')
    parser.add_argument('out', nargs='?', type=Path,
                        default=repo_root / 'extra' / 'single_include' / 'boost' / 'int128.hpp',
                        help='output file (default: <repo>/extra/single_include/boost/int128.hpp)')
    args = parser.parse_args()

    amalgamator = Amalgamator(args.include_root)
    amalgamator.out_lines.extend(PREAMBLE)
    amalgamator.inline(args.top, [], False)

    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text('\n'.join(amalgamator.out_lines) + '\n', encoding='utf-8')
    print(f'Wrote {args.out} ({len(amalgamator.out_lines)} lines, {len(amalgamator.visited)} headers inlined)')


if __name__ == '__main__':
    main()
