import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
"""
# Linux x64
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    '`__int128`': [879535, 92165, 92514, 115727, 1234838, 1193529],
    'int128_t': [748787, 92441, 88390, 90897, 1352795, 1256687],
    'boost::mp::int128_t': [2210502, 283528, 668953, 312723, 1320695, 1287093],
    'absl::int128': [741269, 92323, 90394, 89558, 1200439, 1293439],
}
"""
"""
# Linux ARM64
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    '`__int128`': [3495621, 191514, 131380, 236071, 2412757, 2501357],
    'int128_t': [2279914, 133319, 193984, 234594, 2434752, 2171828],
    'boost::mp::int128_t': [5910287, 566860, 1066509, 864526, 2508755, 2571959],
    'absl::int128': [3749448, 164848, 193467, 237676, 2484139, 2158203]
}

"""

# Linux s390x
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    '`__int128`': [14099505, 1151086, 1223119, 1904542, 8768877, 8661233],
    'int128_t': [12588237, 1374984, 753561, 2060986, 7080113, 7180650],
    'boost::mp::int128_t': [21074294, 3303931, 4224613, 3034387, 7306287, 8801605],
    'absl::int128': [13972778, 1195725, 1295929, 1733150, 7968543, 8175497],
}

"""
# Linux ppc64le
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    '`__int128`': [4538094, 221708, 222629, 193315, 5607581, 5623562],
    'int128_t': [5796198, 191841, 174273, 191785, 4669820, 4750314],
    'boost::mp::int128_t': [13907323, 1177034, 1861166, 878393, 5616217, 5641480]
}
"""
"""
# macos x64
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    '`__int128`': [1628142, 224648, 212849, 432205, 3924951, 3042060],
    'int128_t': [1748005, 180393, 131062, 407829, 2409106, 2423738],
    'boost::mp::int128_t': [4318109, 925013, 1876834, 651209, 3719183, 4443402]
}
"""
"""
# macos ARM
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    '`__int128`': [133275, 20203, 20203, 21496, 662767, 719179],
    'int128_t': [131953, 17797, 17832, 20202, 682891, 692509],
    'boost::mp::int128_t': [340555, 169909, 172497, 78269, 969277, 1026090],
    'absl::int128': [133509, 20208, 22199, 20364, 663602, 717897]
}
"""
"""
# MSVC 14.3 - ARM64
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    'std::_Signed128': [911829, 33233, 33411, 117586, 1127267, 1287100],
    'int128_t': [368104, 34001, 34130, 56324, 1500725, 1548073],
    'boost::mp::int128_t': [2376802, 121700, 1488822, 1564799, 2808293, 2997474]
}
"""
"""
# MSVC 14.3 - x64
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    'std::_Signed128': [2186843, 186771, 193660, 402806, 1612873, 1637135],
    'int128_t': [2142626, 184598, 186335, 117413, 2369701, 2218627],
    'boost::mp::int128_t': [4854983, 2645943, 2925784, 3887479, 6437280, 6236026]
}
"""
"""
# MSVC 14.3 - x86
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    'std::_Signed128': [3495288, 199936, 1089785, 2653505, 7267297, 5779771],
    'int128_t': [3520950, 212116, 210354, 2595285, 5516460, 5842785],
    'boost::mp::int128_t': [7877534, 3477656, 4108539, 7030276, 10229356, 9069360]
}
"""
df = pd.DataFrame(data)

# Function to determine color based on ranking
def get_colors_by_rank(row):
    values = row[1:].values
    ranks = np.argsort(values) + 1
    colors = []
    for rank in ranks:
        if rank == 1:
            colors.append('#90EE90')  # Light Green - Best
        elif rank == 2:
            colors.append('#FFFFE0')  # Light Yellow - Second
        else:
            colors.append('#FFB6C1')  # Light Red - Third
    return colors

# Create figure with subplots
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 10))

# Prepare data
operations = df['Operation']
x = np.arange(len(operations))
width = 0.25

# Get implementation names
implementations = df.columns[1:]

# Plot 1: Regular scale bar chart with color coding
for i, (idx, row) in enumerate(df.iterrows()):
    colors = get_colors_by_rank(row)
    for j, impl in enumerate(implementations):
        ax1.bar(x[i] + (j-1)*width, row[impl], width,
                color=colors[j], edgecolor='black', linewidth=0.5,
                label=impl if i == 0 else "")

ax1.set_xlabel('Operations', fontsize=12)
ax1.set_ylabel('Time (nanoseconds)', fontsize=12)
ax1.set_title('GCC 13 - s390x Benchmark Results', fontsize=14, fontweight='bold')
ax1.set_xticks(x)
ax1.set_xticklabels(operations, rotation=45, ha='right')
ax1.legend(loc='upper left')
ax1.grid(axis='y', alpha=0.3)

# Add value labels on bars
for i, (idx, row) in enumerate(df.iterrows()):
    for j, impl in enumerate(implementations):
        ax1.text(x[i] + (j-1)*width, row[impl], f'{row[impl]:,}',
                 ha='center', va='bottom', fontsize=8, rotation=90)

# Plot 2: Log scale for better visualization
for i, impl in enumerate(implementations):
    bars = ax2.bar(x + (i-1)*width, df[impl], width, label=impl, edgecolor='black', linewidth=0.5)

    # Color each bar based on its rank within operation
    for j, bar in enumerate(bars):
        operation_values = df.iloc[j, 1:].values
        rank = np.argsort(operation_values).tolist().index(i) + 1
        if rank == 1:
            bar.set_facecolor('#90EE90')
        elif rank == 2:
            bar.set_facecolor('#FFFFE0')
        else:
            bar.set_facecolor('#FFB6C1')

ax2.set_xlabel('Operations', fontsize=12)
ax2.set_ylabel('Time (nanoseconds) - Log Scale', fontsize=12)
ax2.set_title('GCC 13 - s390x Benchmark Results (Log Scale)', fontsize=14, fontweight='bold')
ax2.set_yscale('log')
ax2.set_xticks(x)
ax2.set_xticklabels(operations, rotation=45, ha='right')
ax2.legend(loc='upper left')
ax2.grid(axis='y', alpha=0.3, which='both')

plt.tight_layout()
plt.savefig('s390x_benchmarks.png', dpi=300, bbox_inches='tight')
plt.show()

# Create a normalized performance chart
fig3, ax3 = plt.subplots(figsize=(10, 6))

# Normalize data relative to __int128
normalized_df = df.copy()
for col in implementations:
    normalized_df[col] = df[col] / df['`__int128`']

# Plot normalized bars
for i, impl in enumerate(implementations):
    if impl == '`__int128`':
        continue  # Skip since it's always 1.0
    bars = ax3.bar(x + (i-1.5)*width, normalized_df[impl], width,
                   label=impl, edgecolor='black', linewidth=0.5)

    # Add value labels
    for j, bar in enumerate(bars):
        height = bar.get_height()
        ax3.text(bar.get_x() + bar.get_width()/2., height,
                 f'{height:.2f}x', ha='center', va='bottom', fontsize=9)

# Add reference line at 1.0
ax3.axhline(y=1.0, color='red', linestyle='--', alpha=0.5, label='`__int128` baseline')

ax3.set_xlabel('Operations', fontsize=12)
ax3.set_ylabel('Relative Performance (vs __int128)', fontsize=12)
ax3.set_title('Relative Performance Comparison - s390x', fontsize=14, fontweight='bold')
ax3.set_xticks(x)
ax3.set_xticklabels(operations, rotation=45, ha='right')
ax3.legend()
ax3.grid(axis='y', alpha=0.3)

# Add interpretation text
ax3.text(0.02, 0.98, 'Lower is better', transform=ax3.transAxes,
         fontsize=10, verticalalignment='top', style='italic')

plt.tight_layout()
plt.savefig('s390x_relative_performance.png', dpi=300, bbox_inches='tight')
plt.show()

# Generate summary statistics
print("\nPerformance Summary (x64):")
print("-" * 50)
for impl in implementations:
    if impl == '__int128':
        continue
    avg_ratio = normalized_df[impl].mean()
    print(f"{impl}: {avg_ratio:.2f}x average vs __int128")

print("\nBest performer by operation:")
print("-" * 50)
for i, op in enumerate(operations):
    row_data = df.iloc[i, 1:]
    best_impl = row_data.idxmin()
    best_time = row_data.min()
    print(f"{op}: {best_impl} ({best_time:,} ns)")

