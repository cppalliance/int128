#!/usr/bin/env python3
"""Generate every Boost.Int128 benchmark graph and write it straight into the
documentation images tree.

Each entry produces two PNGs whose names match the image:: directives in the
.adoc pages:

    modules/ROOT/images/<sign>_graphs/<os>/<arch>_benchmarks.png
    modules/ROOT/images/<sign>_graphs/<os>/<arch>_relative_performance.png

To refresh a platform's numbers, edit its 'data' block here and re-run; the
right file is overwritten automatically.
"""

import os

import matplotlib
matplotlib.use('Agg')  # headless backend: write files, never open a window
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Operation order shared by every dataset (matches the x-axis of all charts).
OPERATIONS = ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo']

# One entry per published graph. Fields:
#   sign  : 'u128' (unsigned) or 'i128' (signed)  -> selects the *_graphs folder
#   os    : 'linux' | 'macos' | 'windows'         -> selects the sub-folder
#   arch  : file stem used by the .adoc image:: directives (casing must match the
#           image:: targets exactly; ARM stems are always upper-case, e.g. 'ARM64')
#   title : chart heading prefix, e.g. 'GCC 14 - x64'
#   data  : implementation -> timings in microseconds, in OPERATIONS order
# The normalization baseline for the relative chart is detected automatically
# (native type where present, otherwise Boost.Multiprecision).
DATASETS = [
    # ----------------------------- unsigned, Linux -----------------------------
    {
        'sign': 'u128', 'os': 'linux', 'arch': 'x64', 'title': 'GCC 16 - x64',
        'data': {
            'unsigned __int128': [2555576, 242772, 372481, 356366, 4481403, 3965562],
            'uint128_t': [2404372, 241336, 260064, 312736, 4498211, 4506879],
            'boost::mp::uint128_t': [3576079, 328546, 287267, 326328, 4602586, 4487023],
            'absl::uint128': [2099066, 301186, 282908, 277284, 4290212, 4247367],
        },
    },
    {
        'sign': 'u128', 'os': 'linux', 'arch': 'ARM64', 'title': 'GCC 13 - ARM64',
        'data': {
            'unsigned __int128': [4077924, 137276, 155498, 218009, 2254781, 2274294],
            'uint128_t': [2335044, 151553, 133470, 233811, 1819447, 1743274],
            'boost::mp::uint128_t': [5360167, 184406, 186793, 324341, 2211225, 2324356],
            'absl::uint128': [4184235, 151276, 149111, 293431, 2152312, 2381378],
        },
    },
    {
        'sign': 'u128', 'os': 'linux', 'arch': 's390x', 'title': 'GCC 13 - s390x',
        'data': {
            'unsigned __int128': [7293935, 636224, 572225, 1040424, 4191637, 4156643],
            'uint128_t': [6198402, 707436, 350035, 741789, 2593472, 2133029],
            'boost::mp::uint128_t': [8182815, 611849, 595266, 899957, 4106663, 4398856],
            'absl::uint128': [13820009, 1530136, 1211168, 1843000, 4883553, 5011442],
        },
    },
    {
        'sign': 'u128', 'os': 'linux', 'arch': 'ppc64le', 'title': 'GCC 14 - ppc64le',
        'data': {
            'unsigned __int128': [5242604, 221776, 222894, 194494, 4821119, 4955570],
            'uint128_t': [4450958, 193063, 175259, 192929, 4896360, 4273487],
            'boost::mp::uint128_t': [5704848, 847504, 786659, 795187, 5344637, 5407877],
        },
    },
    {
        'sign': 'u128', 'os': 'linux', 'arch': 'x86', 'title': 'GCC 16 - x86_32',
        'data': {
            'uint128_t': [9545542, 686648, 618456, 859253, 8271920, 9932867],
            'boost::mp::uint128_t': [8582001, 7261481, 7968678, 6746697, 15931092, 10242720],
        },
    },
    {
        'sign': 'u128', 'os': 'linux', 'arch': 'ARM32', 'title': 'GCC 14 - ARM32',
        'data': {
            'uint128_t': [5286033, 454715, 487190, 1471479, 19868087, 20332627],
            'boost::mp::uint128_t': [4538707, 5543856, 6465126, 8246098, 32820805, 27238658],
        },
    },
    # ---------------------------- unsigned, Windows ----------------------------
    {
        'sign': 'u128', 'os': 'windows', 'arch': 'x64', 'title': 'MSVC 14.5 - x64',
        'data': {
            'std::_Unsigned128': [2055229, 152603, 150576, 131223, 1476783, 1421066],
            'uint128_t': [1714007, 116444, 116367, 123694, 1489919, 1411521],
            'boost::mp::uint128_t': [2490543, 2596037, 2901567, 3300491, 4898388, 3793762],
        },
    },
    {
        'sign': 'u128', 'os': 'windows', 'arch': 'ARM64', 'title': 'MSVC 14.5 - ARM64',
        'data': {
            'std::_Unsigned128': [945196, 37403, 33927, 74384, 992963, 1087702],
            'uint128_t': [405891, 40039, 38887, 46406, 790846, 861121],
            'boost::mp::uint128_t': [1306884, 1351728, 1594845, 1281286, 2035065, 1702396],
        },
    },
    {
        'sign': 'u128', 'os': 'windows', 'arch': 'x86', 'title': 'MSVC 14.5 - x86_32',
        'data': {
            'std::_Unsigned128': [4806287, 254275, 1322877, 2327500, 5596877, 4616488],
            'uint128_t': [3940703, 202421, 207351, 2312040, 5629510, 5696116],
            'boost::mp::uint128_t': [2624013, 2961566, 3703369, 4375417, 6756883, 6409969],
        },
    },
    # ----------------------------- unsigned, macOS -----------------------------
    {
        'sign': 'u128', 'os': 'macos', 'arch': 'ARM64', 'title': 'Clang 22 - ARM64',
        'data': {
            'unsigned __int128': [134425, 20754, 20552, 20264, 685358, 733080],
            'uint128_t': [134742, 18389, 18573, 20150, 740877, 699666],
            'boost::mp::uint128_t': [133107, 20653, 20590, 20181, 913877, 951657],
            'absl::uint128': [135182, 20929, 20439, 20228, 718985, 719500],
        },
    },
    # ------------------------------ signed, Linux ------------------------------
    {
        'sign': 'i128', 'os': 'linux', 'arch': 'x64', 'title': 'GCC 16 - x64',
        'data': {
            '`__int128`': [2232997, 244246, 220957, 433431, 4462364, 4803576],
            'int128_t': [1970941, 292081, 196953, 321168, 4983165, 5257406],
            'boost::mp::int128_t': [5478483, 650160, 1625774, 1595688, 4992819, 4988844],
            'absl::int128': [1944089, 227720, 315611, 304069, 4986970, 5081814],
        },
    },
    {
        'sign': 'i128', 'os': 'linux', 'arch': 'ARM64', 'title': 'GCC 13 - ARM64',
        'data': {
            '`__int128`': [4115337, 194461, 151441, 334847, 2403064, 2235322],
            'int128_t': [2169531, 196244, 97565, 232518, 1848517, 2159401],
            'boost::mp::int128_t': [5914108, 543680, 1161677, 904461, 2493904, 2535438],
            'absl::int128': [3725321, 195216, 192729, 240980, 2431322, 2321638],
        },
    },
    {
        'sign': 'i128', 'os': 'linux', 'arch': 's390x', 'title': 'GCC 13 - s390x',
        'data': {
            '`__int128`': [5171094, 625328, 667538, 904480, 3758577, 4218409],
            'int128_t': [5069329, 785936, 356865, 729911, 2211087, 2330114],
            'boost::mp::int128_t': [7457296, 1286888, 2555881, 1562062, 3095993, 3684163],
            'absl::int128': [5343843, 670826, 741947, 786829, 3940264, 3849849],
        },
    },
    {
        'sign': 'i128', 'os': 'linux', 'arch': 'ppc64le', 'title': 'GCC 14 - ppc64le',
        'data': {
            '`__int128`': [4538094, 221708, 222629, 193315, 5607581, 5623562],
            'int128_t': [5796198, 191841, 174273, 191785, 4669820, 4750314],
            'boost::mp::int128_t': [13907323, 1177034, 1861166, 878393, 5616217, 5641480],
        },
    },
    {
        'sign': 'i128', 'os': 'linux', 'arch': 'x86', 'title': 'GCC 16 - x86_32',
        'data': {
            'int128_t': [10310201, 786499, 907051, 855780, 10254664, 10851123],
            'boost::mp::int128_t': [14160000, 7379646, 7890190, 10826565, 24702433, 17348307],
        },
    },
    {
        'sign': 'i128', 'os': 'linux', 'arch': 'ARM32', 'title': 'GCC 14 - ARM32',
        'data': {
            'int128_t': [6149439, 457850, 488321, 1793874, 17738614, 18064819],
            'boost::mp::int128_t': [6432579, 5669571, 7464427, 11410321, 38956122, 30144743],
        },
    },
    # ----------------------------- signed, Windows -----------------------------
    {
        'sign': 'i128', 'os': 'windows', 'arch': 'x64', 'title': 'MSVC 14.5 - x64',
        'data': {
            'std::_Signed128': [1879694, 141120, 157649, 266740, 1387560, 1616895],
            'int128_t': [1894168, 143877, 156965, 138754, 1752869, 1908345],
            'boost::mp::int128_t': [5198915, 2846799, 3027203, 4080611, 6924406, 6397442],
        },
    },
    {
        'sign': 'i128', 'os': 'windows', 'arch': 'ARM64', 'title': 'MSVC 14.3 - ARM64',
        'data': {
            'std::_Signed128': [991273, 34519, 34184, 126490, 1128432, 1427629],
            'int128_t': [391918, 48953, 36278, 36781, 1107571, 1310481],
            'boost::mp::int128_t': [2551137, 1243326, 1387708, 1632232, 2472959, 2926904],
        },
    },
    {
        'sign': 'i128', 'os': 'windows', 'arch': 'x86', 'title': 'MSVC 14.5 - x86_32',
        'data': {
            'std::_Signed128': [3832024, 232554, 1198377, 2921104, 7174578, 5528639],
            'int128_t': [3823023, 197092, 145823, 428925, 7189000, 7028725],
            'boost::mp::int128_t': [5568151, 3488510, 4011233, 6219931, 9748526, 9205892],
        },
    },
    # ------------------------------ signed, macOS ------------------------------
    {
        'sign': 'i128', 'os': 'macos', 'arch': 'ARM64', 'title': 'Clang 22 - ARM64',
        'data': {
            '`__int128`': [135259, 20399, 20156, 20654, 668004, 664356],
            'int128_t': [134127, 18575, 18983, 20860, 659823, 662282],
            'boost::mp::int128_t': [340037, 169575, 168041, 69443, 976248, 1026487],
            'absl::int128': [136845, 20429, 20875, 20651, 660963, 665474],
        },
    },
]

# Bar colors by speed rank within an operation: green best, yellow second, red rest.
RANK_COLORS = {1: '#90EE90', 2: '#FFFFE0'}
SLOW_COLOR = '#FFB6C1'

# Baseline candidates in priority order; first one present in a dataset wins.
BASELINE_PRIORITY = {
    'u128': ['unsigned __int128', 'std::_Unsigned128', 'boost::mp::uint128_t'],
    'i128': ['`__int128`', '__int128', 'std::_Signed128', 'boost::mp::int128_t'],
}


# Pick the column every other implementation is compared against.
def detect_baseline(impls, sign):
    for candidate in BASELINE_PRIORITY[sign]:
        if candidate in impls:
            return candidate
    return impls[0]


# 1-based speed rank per implementation for one operation row (1 == fastest).
def speed_ranks(values):
    return np.argsort(np.argsort(values)) + 1


def color_for_rank(rank):
    return RANK_COLORS.get(rank, SLOW_COLOR)


# Build the two-panel benchmark figure (linear + log) and save it.
def save_benchmark_chart(df, impls, x, width, title, path):
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 10))

    # Speed rank (1 == fastest) of each implementation, per operation row.
    rank_by_op = [speed_ranks(df.iloc[op][impls].values) for op in range(len(df))]

    # Linear panel: one rank-colored bar per implementation within each operation.
    for op_idx, (_, row) in enumerate(df.iterrows()):
        ranks = rank_by_op[op_idx]
        for j, impl in enumerate(impls):
            ax1.bar(x[op_idx] + (j - 1) * width, row[impl], width,
                    color=color_for_rank(ranks[j]), edgecolor='black', linewidth=0.5,
                    label=impl if op_idx == 0 else "")
            ax1.text(x[op_idx] + (j - 1) * width, row[impl], f'{row[impl]:,}',
                     ha='center', va='bottom', fontsize=8, rotation=90)

    ax1.set_xlabel('Operations', fontsize=12)
    ax1.set_ylabel('Time (microseconds)', fontsize=12)
    ax1.set_title(f'{title} Benchmark Results', fontsize=14, fontweight='bold')
    ax1.set_xticks(x)
    ax1.set_xticklabels(OPERATIONS, rotation=45, ha='right')
    ax1.legend(loc='upper left')
    ax1.grid(axis='y', alpha=0.3)

    # Log panel: same bars and rank colors, log y-axis for the wide dynamic range.
    # Draw once per implementation (carries the legend label), then recolor each
    # bar by its rank so the legend entry stays attached.
    for j, impl in enumerate(impls):
        bars = ax2.bar(x + (j - 1) * width, df[impl], width, label=impl,
                       edgecolor='black', linewidth=0.5)
        for op_idx, bar in enumerate(bars):
            bar.set_facecolor(color_for_rank(rank_by_op[op_idx][j]))

    ax2.set_xlabel('Operations', fontsize=12)
    ax2.set_ylabel('Time (microseconds) - Log Scale', fontsize=12)
    ax2.set_title(f'{title} Benchmark Results (Log Scale)', fontsize=14, fontweight='bold')
    ax2.set_yscale('log')
    ax2.set_xticks(x)
    ax2.set_xticklabels(OPERATIONS, rotation=45, ha='right')
    ax2.legend(loc='upper left')
    ax2.grid(axis='y', alpha=0.3, which='both')

    fig.tight_layout()
    fig.savefig(path, dpi=300, bbox_inches='tight')
    plt.close(fig)


# Build the relative-performance figure (everything normalized to baseline) and save it.
def save_relative_chart(df, impls, x, width, title, baseline, path):
    fig, ax = plt.subplots(figsize=(10, 6))

    normalized = df[impls].div(df[baseline], axis=0)
    for i, impl in enumerate(impls):
        if impl == baseline:
            continue
        bars = ax.bar(x + (i - 1.5) * width, normalized[impl], width,
                      label=impl, edgecolor='black', linewidth=0.5)
        for bar in bars:
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width() / 2., height,
                    f'{height:.2f}x', ha='center', va='bottom', fontsize=9)

    # Headroom above the tallest bar so its value label and the "lower is better"
    # note in the top-left corner never collide with the bars.
    plotted = [impl for impl in impls if impl != baseline]
    tallest = float(normalized[plotted].to_numpy().max())
    ax.set_ylim(top=max(tallest * 1.20, 1.12))

    ax.axhline(y=1.0, color='red', linestyle='--', alpha=0.5,
               label=f'{baseline} baseline')
    ax.set_xlabel('Operations', fontsize=12)
    ax.set_ylabel(f'Relative Performance (vs {baseline})', fontsize=12)
    ax.set_title(f'Relative Performance Comparison - {title}', fontsize=14, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(OPERATIONS, rotation=45, ha='right')
    ax.legend()
    ax.grid(axis='y', alpha=0.3)
    ax.text(0.02, 0.98, 'Lower is better', transform=ax.transAxes,
            fontsize=10, verticalalignment='top', style='italic')

    fig.tight_layout()
    fig.savefig(path, dpi=300, bbox_inches='tight')
    plt.close(fig)


# Render and save both charts for a single dataset; return the two output paths.
def render_dataset(entry, images_dir):
    impls = list(entry['data'].keys())
    df = pd.DataFrame({'Operation': OPERATIONS, **entry['data']})
    baseline = detect_baseline(impls, entry['sign'])

    x = np.arange(len(OPERATIONS))
    width = 0.25

    out_dir = os.path.join(images_dir, f"{entry['sign']}_graphs", entry['os'])
    os.makedirs(out_dir, exist_ok=True)
    bench_path = os.path.join(out_dir, f"{entry['arch']}_benchmarks.png")
    rel_path = os.path.join(out_dir, f"{entry['arch']}_relative_performance.png")

    save_benchmark_chart(df, impls, x, width, entry['title'], bench_path)
    save_relative_chart(df, impls, x, width, entry['title'], baseline, rel_path)
    return bench_path, rel_path


def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    images_dir = os.path.join(script_dir, 'modules', 'ROOT', 'images')

    written = 0
    for entry in DATASETS:
        bench_path, rel_path = render_dataset(entry, images_dir)
        for path in (bench_path, rel_path):
            print(f"wrote {os.path.relpath(path, script_dir)}")
            written += 1

    print(f"\nDone: {written} images across {len(DATASETS)} platforms.")


if __name__ == '__main__':
    main()
