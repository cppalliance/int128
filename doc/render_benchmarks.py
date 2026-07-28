#!/usr/bin/env python3
# Copyright 2026 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

"""Turn the committed benchmark data sets into the Boost.Int128 documentation.

Reads every JSON data set under doc/modules/ROOT/data, as written by

    benchmark_u128 --json <path>
    benchmark_i128 --json <path>

and then

  * renders both plots for each platform into
    doc/modules/ROOT/images/<sign>_graphs/<os>/<arch>_benchmarks.png and
    doc/modules/ROOT/images/<sign>_graphs/<os>/<arch>_relative_performance.png, and
  * regenerates the platform sections of
    doc/modules/ROOT/pages/u128_benchmarks.adoc and i128_benchmarks.adoc,
    replacing everything after the "// BENCHMARK-RESULTS-GENERATED" line.

Refreshing the documentation is therefore: download the artifacts of the
benchmarks workflow, unpack the benchmarks-<os>-<arch> folders into
doc/modules/ROOT/data, and run this script. Which platform, compiler, and
element count a data set describes is read out of the data set itself, so no
number, label, or file name is ever transcribed by hand.

    render_benchmarks.py                 # rewrite the pages and the plots
    render_benchmarks.py --check         # fail if the pages are out of date
"""

import argparse
import json
import os
import sys

# Rows of the published tables and the x-axis of every chart, in order:
# (data set key, documentation label).
OPERATIONS = [
    ('comparisons', 'Comparisons'),
    ('add', 'Addition'),
    ('sub', 'Subtraction'),
    ('mul', 'Multiplication'),
    ('div', 'Division'),
    ('mod', 'Modulo'),
]

# The published numbers come from the random width operands (each element is 32,
# 64, 96, or 128 bits wide), which is the most representative mix. The data sets
# also carry the fixed width groups, the narrow divisions, the shifts, and the
# bitwise operations for anyone digging into a specific result.
GROUP = 'random_width'

# Marks the start of the generated region of a page. Everything up to and
# including this line is preserved; everything after it is rewritten.
SENTINEL = '// BENCHMARK-RESULTS-GENERATED'

# The two pages, keyed by the "sign" field of a data set.
PAGES = {
    'u128': 'u128_benchmarks.adoc',
    'i128': 'i128_benchmarks.adoc',
}

# Section order and headings. The operating system anchors are linked from
# nav.adoc, so they have to keep their current spelling.
OS_ORDER = ['linux', 'windows', 'macos', 'freebsd', 'cygwin']
OS_TITLES = {'linux': 'Linux', 'windows': 'Windows', 'macos': 'macOS', 'freebsd': 'FreeBSD', 'cygwin': 'Cygwin'}
OS_ANCHORS = {'macos': 'mac'}

ARCH_ORDER = ['x64', 'ARM64', 's390x', 'ppc64le', 'ppc64', 'x86', 'x32', 'ARM32', 'riscv64', 'riscv32', 'loongarch64']
ARCH_HEADINGS = {'x64': 'x86_64', 'x86': 'x86_32', 's390x': 'S390x', 'ppc64le': 'PPC64LE', 'ppc64': 'PPC64', 'x32': 'x32'}

# __cplusplus (or _MSVC_LANG) to the year in the standard's name.
CXXSTD_LABELS = {201103: '11', 201402: '14', 201703: '17', 202002: '20', 202302: '23', 202612: '26'}

# Bar colors by speed rank within an operation: green best, yellow second, red rest.
RANK_COLORS = {1: '#90EE90', 2: '#FFFFE0'}
SLOW_COLOR = '#FFB6C1'

# Fallback when a data set names a baseline that it did not measure; first match wins.
BASELINE_PRIORITY = {
    'u128': ['unsigned __int128', 'std::_Unsigned128', 'boost::mp::uint128_t'],
    'i128': ['__int128', 'std::_Signed128', 'boost::mp::int128_t'],
}


class data_set:
    """One platform's measurements for one of the two types."""

    def __init__(self, doc, path):
        self.path = path
        self.sign = doc['sign']
        self.os = doc['os']
        self.arch = doc['arch']
        self.compiler = doc.get('compiler', '')
        self.cxxstd = doc.get('cxxstd')
        self.elements = doc.get('elements')
        self.repetitions = doc.get('repetitions')
        self.timings = {}

        for entry in doc.get('results', []):
            key = (entry['group'], entry['operation'], entry['implementation'])
            self.timings[key] = entry['microseconds']

        measured = doc.get('implementations') or first_seen(doc.get('results', []))
        self.implementations = [name for name in measured if self.complete(name)]

        dropped = [name for name in measured if name not in self.implementations]
        for name in dropped:
            print(f"warning: {path}: no {GROUP} timings for '{name}', leaving it out")

        self.baseline = self.pick_baseline(doc.get('baseline', ''))

    # True when the implementation has a timing for every published row.
    def complete(self, name):
        return all((GROUP, op, name) in self.timings for op, _ in OPERATIONS)

    def pick_baseline(self, named):
        if named in self.implementations:
            return named

        for candidate in BASELINE_PRIORITY.get(self.sign, []):
            if candidate in self.implementations:
                return candidate

        # An empty data set is reported by the caller, which owns the file name.
        return self.implementations[0] if self.implementations else ''

    def value(self, operation, implementation):
        return self.timings[(GROUP, operation, implementation)]

    def row(self, implementation):
        return [self.value(op, implementation) for op, _ in OPERATIONS]

    # 'GCC 16.0 - x64', the heading of both charts.
    def title(self):
        return f'{self.compiler} - {self.arch}' if self.compiler else self.arch

    def key(self):
        os_rank = OS_ORDER.index(self.os) if self.os in OS_ORDER else len(OS_ORDER)
        arch_rank = ARCH_ORDER.index(self.arch) if self.arch in ARCH_ORDER else len(ARCH_ORDER)
        return (os_rank, self.os, arch_rank, self.arch)


# Implementation names in the order they were first measured.
def first_seen(results):
    order = []
    for entry in results:
        if entry['implementation'] not in order:
            order.append(entry['implementation'])
    return order


def load_data_sets(data_dir):
    if not os.path.isdir(data_dir):
        sys.exit(f'error: no data directory {data_dir}')

    by_sign = {sign: {} for sign in PAGES}
    for root, _, files in os.walk(data_dir):
        for name in sorted(files):
            if not name.endswith('.json'):
                continue

            path = os.path.join(root, name)
            with open(path) as handle:
                doc = json.load(handle)

            if doc.get('sign') not in by_sign:
                sys.exit(f"error: {path}: unknown sign '{doc.get('sign')}'")

            entry = data_set(doc, path)
            if not entry.implementations:
                sys.exit(f'error: {path}: no usable timings')

            platform = (entry.os, entry.arch)
            existing = by_sign[entry.sign].get(platform)
            if existing is not None:
                sys.exit(f'error: {path} and {existing.path} both describe {entry.sign} on {entry.os}/{entry.arch}')

            by_sign[entry.sign][platform] = entry

    for sign, found in by_sign.items():
        if not found:
            sys.exit(f'error: no {sign} data sets under {data_dir}')

    return {sign: sorted(found.values(), key=data_set.key) for sign, found in by_sign.items()}


# ------------------------------- documentation -------------------------------

def heading(entry):
    text = ARCH_HEADINGS.get(entry.arch, entry.arch)
    if entry.os == 'macos' and entry.arch == 'ARM64':
        text += ' (Apple Silicon)'
    return text


def measurement_note(entry):
    parts = []
    if entry.compiler:
        standard = CXXSTD_LABELS.get(entry.cxxstd)
        suffix = f' (pass:[C++]{standard})' if standard else ''
        parts.append(f'Measured with {entry.compiler}{suffix}')

    if entry.elements and entry.repetitions:
        total = entry.elements * entry.repetitions
        passes = 'pass' if entry.repetitions == 1 else 'passes'
        parts.append(f'{entry.repetitions} {passes} over {entry.elements:,} element pairs '
                     f'({total:,} operations per cell)')

    return ': '.join(parts) + '.' if parts else ''


def render_page(sign, platforms):
    lines = []
    current_os = None

    for entry in platforms:
        if entry.os != current_os:
            current_os = entry.os
            anchor = OS_ANCHORS.get(current_os, current_os)
            lines.append(f'[#{sign}_{anchor}]')
            lines.append(f'== {OS_TITLES.get(current_os, current_os)}')
            lines.append('')

        lines.append(f'=== {heading(entry)}')
        lines.append('')

        if entry.baseline.startswith('boost::mp::'):
            lines.append(f'NOTE: This platform has no hardware type so we compare relative to `{entry.baseline}`')
            lines.append('')

        note = measurement_note(entry)
        if note:
            lines.append(note)
            lines.append('')

        lines.append('[cols="1' + ',>1' * len(entry.implementations) + '"]')
        lines.append('|===')
        lines.append('| Operation | ' + ' | '.join(f'`{name}`' for name in entry.implementations))
        lines.append('')

        for op, label in OPERATIONS:
            cells = ' | '.join(f'{entry.value(op, name):,}' for name in entry.implementations)
            lines.append(f'| {label} | {cells}')

        lines.append('|===')
        lines.append('')
        lines.append(f'image::{sign}_graphs/{entry.os}/{entry.arch}_relative_performance.png'
                     f'[{entry.arch} Relative Performance, width=100%]')
        lines.append('')

    return '\n'.join(lines).rstrip() + '\n'


def update_page(path, body, check):
    with open(path) as handle:
        original = handle.read()

    marker = original.find(SENTINEL)
    if marker < 0:
        sys.exit(f"error: sentinel '{SENTINEL}' not found in {path}")

    head = original[:marker + len(SENTINEL)]
    updated = head + '\n\n' + body

    if check:
        return updated == original

    with open(path, 'w') as handle:
        handle.write(updated)

    return True


# ----------------------------------- plots -----------------------------------

# Imported on demand, and always headless: --check then needs no plotting stack.
def pyplot():
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt
    return plt


# 1-based speed rank per implementation for one operation row (1 == fastest).
def speed_ranks(values):
    import numpy as np

    return np.argsort(np.argsort(values)) + 1


def color_for_rank(rank):
    return RANK_COLORS.get(rank, SLOW_COLOR)


# Offsets that center a group of bars on its tick.
def offsets(count, width):
    return [(index - (count - 1) / 2.0) * width for index in range(count)]


def swatches(colors):
    from matplotlib.patches import Patch

    return [Patch(facecolor=color, edgecolor='black', linewidth=0.5) for color in colors]


# Draws a legend showing the labels exactly as given. matplotlib drops any label
# that starts with an underscore, which would silently hide '__int128', so the
# legend is built with placeholders and the text is filled in afterwards; the box
# is laid out when the figure is drawn, so it still fits the real labels.
def legend(ax, handles, labels, **kwargs):
    drawn = ax.legend(handles, [str(index) for index in range(len(labels))], **kwargs)
    for text, label in zip(drawn.get_texts(), labels):
        text.set_text(label)

    return drawn


# Absolute timings: the same bars on a linear and on a logarithmic axis.
def save_benchmark_chart(entry, path):
    import numpy as np

    plt = pyplot()
    impls = entry.implementations
    labels = [label for _, label in OPERATIONS]
    rows = [[entry.value(op, name) for name in impls] for op, _ in OPERATIONS]
    ranks = [speed_ranks(row) for row in rows]

    x = np.arange(len(labels))
    width = min(0.25, 0.8 / len(impls))
    shift = offsets(len(impls), width)

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 10))

    for op_index, row in enumerate(rows):
        for impl_index in range(len(impls)):
            ax1.bar(x[op_index] + shift[impl_index], row[impl_index], width,
                    color=color_for_rank(ranks[op_index][impl_index]), edgecolor='black', linewidth=0.5)
            ax1.text(x[op_index] + shift[impl_index], row[impl_index], f'{row[impl_index]:,}',
                     ha='center', va='bottom', fontsize=8, rotation=90)

    # The bars are colored by rank, so the legend keys the implementations by
    # their position within a group, using the first row's colors.
    colors = [color_for_rank(rank) for rank in ranks[0]]

    ax1.set_xlabel('Operations', fontsize=12)
    ax1.set_ylabel('Time (microseconds)', fontsize=12)
    ax1.set_title(f'{entry.title()} Benchmark Results', fontsize=14, fontweight='bold')
    ax1.set_xticks(x)
    ax1.set_xticklabels(labels, rotation=45, ha='right')
    legend(ax1, swatches(colors), impls, loc='upper left', title='In bar order; green is fastest')
    ax1.grid(axis='y', alpha=0.3)

    for impl_index in range(len(impls)):
        bars = ax2.bar(x + shift[impl_index], [row[impl_index] for row in rows], width,
                       edgecolor='black', linewidth=0.5)
        for op_index, bar in enumerate(bars):
            bar.set_facecolor(color_for_rank(ranks[op_index][impl_index]))

    legend(ax2, swatches(colors), impls, loc='upper left', title='In bar order; green is fastest')
    ax2.set_xlabel('Operations', fontsize=12)
    ax2.set_ylabel('Time (microseconds) - Log Scale', fontsize=12)
    ax2.set_title(f'{entry.title()} Benchmark Results (Log Scale)', fontsize=14, fontweight='bold')
    ax2.set_yscale('log')
    ax2.set_xticks(x)
    ax2.set_xticklabels(labels, rotation=45, ha='right')
    ax2.grid(axis='y', alpha=0.3, which='both')

    fig.tight_layout()
    fig.savefig(path, dpi=300, bbox_inches='tight')
    plt.close(fig)


# Every implementation normalized to the platform's reference type.
def save_relative_chart(entry, path):
    import numpy as np

    plt = pyplot()
    impls = [name for name in entry.implementations if name != entry.baseline]
    if not impls:
        return False

    labels = [label for _, label in OPERATIONS]
    base = entry.row(entry.baseline)
    normalized = {name: [value / reference for value, reference in zip(entry.row(name), base)] for name in impls}

    x = np.arange(len(labels))
    width = min(0.25, 0.8 / len(impls))
    shift = offsets(len(impls), width)

    fig, ax = plt.subplots(figsize=(10, 6))
    handles = []

    for impl_index, name in enumerate(impls):
        bars = ax.bar(x + shift[impl_index], normalized[name], width,
                      edgecolor='black', linewidth=0.5)
        handles.append(bars)
        for bar in bars:
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width() / 2.0, height,
                    f'{height:.2f}x', ha='center', va='bottom', fontsize=9)

    # Headroom above the tallest bar so its value label and the "lower is better"
    # note in the top-left corner never collide with the bars.
    tallest = max(max(values) for values in normalized.values())
    ax.set_ylim(top=max(tallest * 1.20, 1.12))

    reference = ax.axhline(y=1.0, color='red', linestyle='--', alpha=0.5)
    ax.set_xlabel('Operations', fontsize=12)
    ax.set_ylabel(f'Relative Performance (vs {entry.baseline})', fontsize=12)
    ax.set_title(f'Relative Performance Comparison - {entry.title()}', fontsize=14, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(labels, rotation=45, ha='right')
    legend(ax, [reference] + handles, [f'{entry.baseline} baseline'] + impls)
    ax.grid(axis='y', alpha=0.3)
    ax.text(0.02, 0.98, 'Lower is better', transform=ax.transAxes,
            fontsize=10, verticalalignment='top', style='italic')

    fig.tight_layout()
    fig.savefig(path, dpi=300, bbox_inches='tight')
    plt.close(fig)
    return True


def render_plots(sign, platforms, images_dir):
    written = []
    for entry in platforms:
        out_dir = os.path.join(images_dir, f'{sign}_graphs', entry.os)
        os.makedirs(out_dir, exist_ok=True)

        absolute = os.path.join(out_dir, f'{entry.arch}_benchmarks.png')
        relative = os.path.join(out_dir, f'{entry.arch}_relative_performance.png')

        save_benchmark_chart(entry, absolute)
        written.append(absolute)

        if save_relative_chart(entry, relative):
            written.append(relative)
        else:
            print(f'warning: {entry.path}: only the baseline was measured, no relative chart')

    return written


# Plots left over from a data set that is no longer present.
def report_stale_plots(images_dir, written):
    expected = {os.path.abspath(path) for path in written}
    for root, _, files in os.walk(images_dir):
        for name in sorted(files):
            if not name.endswith('.png'):
                continue

            path = os.path.abspath(os.path.join(root, name))
            if path not in expected and '_graphs' in root:
                print(f'warning: {os.path.relpath(path, images_dir)} has no data set behind it')


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    parser = argparse.ArgumentParser()
    parser.add_argument('--data', default=os.path.join(here, 'modules', 'ROOT', 'data'),
                        help='directory holding the benchmarks-<os>-<arch> folders')
    parser.add_argument('--images', default=os.path.join(here, 'modules', 'ROOT', 'images'),
                        help='documentation images tree')
    parser.add_argument('--pages', default=os.path.join(here, 'modules', 'ROOT', 'pages'),
                        help='documentation pages tree')
    parser.add_argument('--check', action='store_true',
                        help='write nothing; exit 1 if a page is not what the data sets say it should be')
    args = parser.parse_args()

    data_sets = load_data_sets(args.data)
    stale = []
    written = []

    for sign, platforms in sorted(data_sets.items()):
        page = os.path.join(args.pages, PAGES[sign])
        up_to_date = update_page(page, render_page(sign, platforms), args.check)

        if args.check:
            if not up_to_date:
                stale.append(page)
            continue

        plots = render_plots(sign, platforms, args.images)
        written.extend(plots)
        print(f'{PAGES[sign]}: {len(platforms)} platform(s), {len(plots)} plot(s)')
        for entry in platforms:
            print(f'  - {entry.os}/{entry.arch}: {entry.title()}, baseline {entry.baseline}')

    if args.check:
        for page in stale:
            print(f'{page} is out of date; re-run render_benchmarks.py')

        if stale:
            return 1

        print('the pages match the committed data sets')
        return 0

    report_stale_plots(args.images, written)
    return 0


if __name__ == '__main__':
    sys.exit(main())
