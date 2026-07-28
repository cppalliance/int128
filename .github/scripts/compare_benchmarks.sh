#!/bin/bash
# Copyright 2026 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt
#
# Reports the data sets in $1 against the numbers committed under
# doc/modules/ROOT/data and appends the report to the job summary. Run it from
# the root of the library checkout.
#
# This is informational only: shared runners drift by a few percent from run to
# run, so a slower number is a prompt to look, not a build failure.

set -e

RESULTS=${1:-bench-results}

PYTHON=python3
if ! command -v python3 > /dev/null 2>&1; then
    PYTHON=python
fi

"$PYTHON" doc/render_benchmarks.py --compare "$RESULTS" --summary "${GITHUB_STEP_SUMMARY:-/dev/null}"
