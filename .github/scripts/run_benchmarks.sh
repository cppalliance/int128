#!/bin/bash
# Copyright 2026 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt
#
# Builds both benchmark drivers in release mode and runs them, leaving u128.json
# and i128.json in the --out directory. Those two files are exactly what
# doc/render_benchmarks.py consumes.
#
# Run it from the root of a Boost tree that has this library in libs/int128; the
# include path is every libs/*/include in that tree, so no b2 headers step is
# needed. The drivers always exit non-zero (the Jamfile declares them run-fail),
# so success is judged on the data set actually being written.
#
# --sources names extra translation units to build alongside the driver, which is
# how the platforms with no packaged Abseil get one: point --cxxflags at a source
# checkout and --sources at absl/numeric/int128.cc, the only file of it that
# absl::uint128 and absl::int128 need.
#
#   .github/scripts/run_benchmarks.sh --compiler g++-14 --out /tmp/results
#   .github/scripts/run_benchmarks.sh --msvc --elements 5000000 --out C:/results

set -e

COMPILER=""
EXTRA_CXXFLAGS=""
EXTRA_LDFLAGS=""
EXTRA_SOURCES=""
ELEMENTS=20000000
REPETITIONS=5
OUT=""
MSVC=0

while [ $# -gt 0 ]; do
    case "$1" in
        --compiler)    COMPILER="$2"; shift 2 ;;
        --cxxflags)    EXTRA_CXXFLAGS="$2"; shift 2 ;;
        --ldflags)     EXTRA_LDFLAGS="$2"; shift 2 ;;
        --sources)     EXTRA_SOURCES="$2"; shift 2 ;;
        --elements)    ELEMENTS="$2"; shift 2 ;;
        --repetitions) REPETITIONS="$2"; shift 2 ;;
        --out)         OUT="$2"; shift 2 ;;
        --msvc)        MSVC=1; shift ;;
        *) echo "unknown argument: $1" >&2; exit 2 ;;
    esac
done

if [ -z "$OUT" ]; then
    echo "error: --out is required" >&2
    exit 2
fi

if [ ! -d libs/int128/test ]; then
    echo "error: run this from the root of a Boost tree holding libs/int128" >&2
    exit 2
fi

mkdir -p "$OUT"

# Every dependency is header only from the benchmarks' point of view, so the
# include path is simply all of the libraries checked out in this tree.
INCLUDES=""
for dir in libs/*/include libs/*/*/include; do
    if [ -d "$dir" ]; then
        INCLUDES="$INCLUDES -I$dir"
    fi
done

for sign in u128 i128; do
    source_file="libs/int128/test/benchmark_$sign.cpp"

    if [ "$MSVC" = 1 ]; then
        # LARGEADDRESSAWARE lets the 32-bit build address more than 2GB, which
        # the widest vectors need.
        executable="./benchmark_$sign.exe"
        # shellcheck disable=SC2086
        cl -nologo -std:c++20 -O2 -EHsc -DNDEBUG $EXTRA_CXXFLAGS $INCLUDES \
            "$source_file" $EXTRA_SOURCES \
            -Fe:"benchmark_$sign.exe" -link -LARGEADDRESSAWARE $EXTRA_LDFLAGS
    else
        executable="./benchmark_$sign"
        # shellcheck disable=SC2086
        $COMPILER -std=c++20 -O2 -DNDEBUG $EXTRA_CXXFLAGS $INCLUDES \
            "$source_file" $EXTRA_SOURCES \
            -o "benchmark_$sign" $EXTRA_LDFLAGS
    fi

    echo "==================== benchmark_$sign ===================="
    "$executable" --elements "$ELEMENTS" --repetitions "$REPETITIONS" --json "$OUT/$sign.json" || true

    if [ ! -s "$OUT/$sign.json" ]; then
        echo "error: benchmark_$sign did not write $OUT/$sign.json" >&2
        exit 1
    fi
done

echo "==================== data sets ===================="
ls -l "$OUT"
