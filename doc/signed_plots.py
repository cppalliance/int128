import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

"""
# Linux x64
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    '`__int128`': [2086545, 229772, 245300, 403481, 4315016, 4303454],
    'int128_t': [1807917, 249644, 172252, 246659, 4369512, 4686920],
    'boost::mp::int128_t': [6119951, 620566, 1522422, 898345, 4767180, 4855786]
}
"""

"""
# Linux ARM64
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    '`__int128`': [3524205, 109691, 195129, 286623, 2350225, 2345191],
    'int128_t': [2191692, 126544, 196092, 192214, 2163053, 2167260],
    'boost::mp::int128_t': [5559916, 553814, 1024231, 924637, 2718340, 2380277]
}
"""
"""
# Linux s390x
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    '`__int128`': [7033705, 558950, 534362, 911317, 4371582, 4375939],
    'int128_t': [6231386, 689575, 329127, 946090, 3574992, 3727994],
    'boost::mp::int128_t': [10322828, 1673032, 2149206, 1362947, 3669927, 4419901]
}
"""
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
    '`__int128`': [134803, 20133, 20156, 19974, 649380, 708247],
    'int128_t': [144313, 17820, 17864, 18572, 666749, 681991],
    'boost::mp::int128_t': [338665, 168326, 169666, 77514, 962183, 1014055]
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

# MSVC 14.3 - x64
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    'std::_Signed128': [2186843, 186771, 193660, 402806, 1612873, 1637135],
    'int128_t': [2142626, 184598, 186335, 117413, 2369701, 2218627],
    'boost::mp::int128_t': [4854983, 2645943, 2925784, 3887479, 6437280, 6236026]
}

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
ax1.set_title('MSVC 14.3 - x64 Benchmark Results', fontsize=14, fontweight='bold')
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
ax2.set_title('MSVC 14.3 - x64 Benchmark Results (Log Scale)', fontsize=14, fontweight='bold')
ax2.set_yscale('log')
ax2.set_xticks(x)
ax2.set_xticklabels(operations, rotation=45, ha='right')
ax2.legend(loc='upper left')
ax2.grid(axis='y', alpha=0.3, which='both')

plt.tight_layout()
plt.savefig('x64_benchmarks.png', dpi=300, bbox_inches='tight')
plt.show()

# Create a normalized performance chart
fig3, ax3 = plt.subplots(figsize=(10, 6))

# Normalize data relative to __int128
normalized_df = df.copy()
for col in implementations:
    normalized_df[col] = df[col] / df['std::_Signed128']

# Plot normalized bars
for i, impl in enumerate(implementations):
    if impl == 'std::_Signed128':
        continue  # Skip since it's always 1.0
    bars = ax3.bar(x + (i-1.5)*width, normalized_df[impl], width,
                   label=impl, edgecolor='black', linewidth=0.5)

    # Add value labels
    for j, bar in enumerate(bars):
        height = bar.get_height()
        ax3.text(bar.get_x() + bar.get_width()/2., height,
                 f'{height:.2f}x', ha='center', va='bottom', fontsize=9)

# Add reference line at 1.0
ax3.axhline(y=1.0, color='red', linestyle='--', alpha=0.5, label='std::_Signed128 baseline')

ax3.set_xlabel('Operations', fontsize=12)
ax3.set_ylabel('Relative Performance (vs __int128)', fontsize=12)
ax3.set_title('Relative Performance Comparison - x64', fontsize=14, fontweight='bold')
ax3.set_xticks(x)
ax3.set_xticklabels(operations, rotation=45, ha='right')
ax3.legend()
ax3.grid(axis='y', alpha=0.3)

# Add interpretation text
ax3.text(0.02, 0.98, 'Lower is better', transform=ax3.transAxes,
         fontsize=10, verticalalignment='top', style='italic')

plt.tight_layout()
plt.savefig('x64_relative_performance.png', dpi=300, bbox_inches='tight')
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

