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
#   arch  : file stem used by the .adoc image:: directives (casing is significant:
#           windows unsigned uses lower-case 'arm64', everywhere else 'ARM64')
#   title : chart heading prefix, e.g. 'GCC 14 - x64'
#   data  : implementation -> timings in nanoseconds, in OPERATIONS order
# The normalization baseline for the relative chart is detected automatically
# (native type where present, otherwise Boost.Multiprecision).
DATASETS = [
    # ----------------------------- unsigned, Linux -----------------------------
    {
        'sign': 'u128', 'os': 'linux', 'arch': 'x64', 'title': 'GCC 14 - x64',
        'data': {
            'unsigned __int128': [785130, 90260, 91143, 111803, 1058435, 1003366],
            'uint128_t': [765065, 85758, 91449, 90069, 901516, 830830],
            'boost::mp::uint128_t': [1363581, 89958, 91224, 113559, 1040071, 1001701],
            'absl::uint128': [766205, 89255, 89716, 89660, 1044710, 978533],
        },
    },
    {
        'sign': 'u128', 'os': 'linux', 'arch': 'ARM64', 'title': 'GCC 14 - ARM64',
        'data': {
            'unsigned __int128': [3427201, 194968, 193067, 263187, 2338258, 2260200],
            'uint128_t': [2078586, 159662, 161903, 201333, 2247175, 2097760],
            'boost::mp::uint128_t': [5026689, 587373, 330052, 972009, 2190856, 2227961],
            'absl::uint128': [3753922, 194070, 140777, 244420, 2223032, 2186750],
        },
    },
    {
        'sign': 'u128', 'os': 'linux', 'arch': 's390x', 'title': 'GCC 13 - s390x',
        'data': {
            'unsigned __int128': [6803419, 546801, 590011, 891753, 3827125, 4925696],
            'uint128_t': [6280326, 618774, 359100, 1192196, 3201674, 3360251],
            'boost::mp::uint128_t': [7965082, 621572, 691515, 944289, 3997037, 5144403],
            'absl::uint128': [10515929, 1744226, 1527622, 1839038, 4913142, 5422155],
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
        'sign': 'u128', 'os': 'linux', 'arch': 'x86', 'title': 'GCC 14 - x86_32',
        'data': {
            'uint128_t': [9000979, 898718, 778881, 1778273, 8496503, 9081442],
            'boost::mp::uint128_t': [8722814, 9912175, 9773677, 8678420, 18133965, 11257837],
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
        'sign': 'u128', 'os': 'windows', 'arch': 'x64', 'title': 'MSVC 14.3 - x64',
        'data': {
            'std::_Unsigned128': [2060556, 261475, 178724, 146063, 1332838, 1465138],
            'uint128_t': [1921174, 106545, 124181, 136115, 1360295, 1471169],
            'boost::mp::uint128_t': [3009890, 2710279, 3059187, 3495634, 4852899, 3926336],
        },
    },
    {
        # NOTE: lower-case 'arm64' on purpose; the u128 windows .adoc uses that stem.
        'sign': 'u128', 'os': 'windows', 'arch': 'arm64', 'title': 'MSVC 14.3 - ARM64',
        'data': {
            'std::_Unsigned128': [878929, 32788, 33627, 68120, 925583, 1104772],
            'uint128_t': [259725, 33723, 36799, 35334, 1020148, 1143344],
            'boost::mp::uint128_t': [1246502, 1437452, 1648131, 1459418, 2216648, 2089105],
        },
    },
    {
        'sign': 'u128', 'os': 'windows', 'arch': 'x86', 'title': 'MSVC 14.3 - x86_32',
        'data': {
            'std::_Unsigned128': [4215438, 199945, 1206168, 2282869, 5516964, 4551146],
            'uint128_t': [3883846, 208436, 210874, 2680359, 4328917, 4330152],
            'boost::mp::uint128_t': [2852442, 3242910, 3851129, 5378001, 6948267, 6294325],
        },
    },
    # ----------------------------- unsigned, macOS -----------------------------
    {
        'sign': 'u128', 'os': 'macos', 'arch': 'x64', 'title': 'Clang - x64',
        'data': {
            'unsigned __int128': [688225, 104921, 129150, 120363, 2333812, 2621949],
            'uint128_t': [712352, 124992, 102302, 119652, 1981469, 2219481],
            'boost::mp::uint128_t': [689146, 137819, 153484, 164100, 2784139, 2736682],
        },
    },
    {
        'sign': 'u128', 'os': 'macos', 'arch': 'ARM64', 'title': 'Clang - ARM64',
        'data': {
            'unsigned __int128': [131902, 20613, 20484, 20160, 686521, 777084],
            'uint128_t': [133564, 17912, 18237, 20580, 699201, 724648],
            'boost::mp::uint128_t': [134182, 40176, 40311, 43285, 945928, 953117],
            'absl::uint128': [132366, 20178, 20207, 20049, 672398, 734229],
        },
    },
    # ------------------------------ signed, Linux ------------------------------
    {
        'sign': 'i128', 'os': 'linux', 'arch': 'x64', 'title': 'GCC 14 - x64',
        'data': {
            '`__int128`': [879535, 92165, 92514, 115727, 1234838, 1193529],
            'int128_t': [748787, 92441, 88390, 90897, 1352795, 1256687],
            'boost::mp::int128_t': [2210502, 283528, 668953, 312723, 1320695, 1287093],
            'absl::int128': [741269, 92323, 90394, 89558, 1200439, 1293439],
        },
    },
    {
        'sign': 'i128', 'os': 'linux', 'arch': 'ARM64', 'title': 'GCC 14 - ARM64',
        'data': {
            '`__int128`': [3495621, 191514, 131380, 236071, 2412757, 2501357],
            'int128_t': [2279914, 133319, 193984, 234594, 2434752, 2171828],
            'boost::mp::int128_t': [5910287, 566860, 1066509, 864526, 2508755, 2571959],
            'absl::int128': [3749448, 164848, 193467, 237676, 2484139, 2158203],
        },
    },
    {
        'sign': 'i128', 'os': 'linux', 'arch': 's390x', 'title': 'GCC 13 - s390x',
        'data': {
            '`__int128`': [14099505, 1151086, 1223119, 1904542, 8768877, 8661233],
            'int128_t': [12588237, 1374984, 753561, 2060986, 7080113, 7180650],
            'boost::mp::int128_t': [21074294, 3303931, 4224613, 3034387, 7306287, 8801605],
            'absl::int128': [13972778, 1195725, 1295929, 1733150, 7968543, 8175497],
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
        'sign': 'i128', 'os': 'linux', 'arch': 'x86', 'title': 'GCC 14 - x86_32',
        'data': {
            'int128_t': [9530060, 785799, 778881, 1148024, 10337258, 10438037],
            'boost::mp::int128_t': [12168353, 7777469, 8214089, 9477355, 22857709, 14848256],
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
        'sign': 'i128', 'os': 'windows', 'arch': 'x64', 'title': 'MSVC 14.3 - x64',
        'data': {
            'std::_Signed128': [2186843, 186771, 193660, 402806, 1612873, 1637135],
            'int128_t': [2142626, 184598, 186335, 117413, 2369701, 2218627],
            'boost::mp::int128_t': [4854983, 2645943, 2925784, 3887479, 6437280, 6236026],
        },
    },
    {
        # NOTE: upper-case 'ARM64'; the i128 windows .adoc uses that stem.
        'sign': 'i128', 'os': 'windows', 'arch': 'ARM64', 'title': 'MSVC 14.3 - ARM64',
        'data': {
            'std::_Signed128': [911829, 33233, 33411, 117586, 1127267, 1287100],
            'int128_t': [368104, 34001, 34130, 56324, 1500725, 1548073],
            'boost::mp::int128_t': [2376802, 121700, 1488822, 1564799, 2808293, 2997474],
        },
    },
    {
        'sign': 'i128', 'os': 'windows', 'arch': 'x86', 'title': 'MSVC 14.3 - x86_32',
        'data': {
            'std::_Signed128': [3495288, 199936, 1089785, 2653505, 7267297, 5779771],
            'int128_t': [3520950, 212116, 210354, 2595285, 5516460, 5842785],
            'boost::mp::int128_t': [7877534, 3477656, 4108539, 7030276, 10229356, 9069360],
        },
    },
    # ------------------------------ signed, macOS ------------------------------
    {
        'sign': 'i128', 'os': 'macos', 'arch': 'x64', 'title': 'Clang - x64',
        'data': {
            '`__int128`': [1628142, 224648, 212849, 432205, 3924951, 3042060],
            'int128_t': [1748005, 180393, 131062, 407829, 2409106, 2423738],
            'boost::mp::int128_t': [4318109, 925013, 1876834, 651209, 3719183, 4443402],
        },
    },
    {
        'sign': 'i128', 'os': 'macos', 'arch': 'ARM64', 'title': 'Clang - ARM64',
        'data': {
            '`__int128`': [133275, 20203, 20203, 21496, 662767, 719179],
            'int128_t': [131953, 17797, 17832, 20202, 682891, 692509],
            'boost::mp::int128_t': [340555, 169909, 172497, 78269, 969277, 1026090],
            'absl::int128': [133509, 20208, 22199, 20364, 663602, 717897],
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
    ax1.set_ylabel('Time (nanoseconds)', fontsize=12)
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
    ax2.set_ylabel('Time (nanoseconds) - Log Scale', fontsize=12)
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
