import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

"""
# ARM64 MSVC
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    'std::_Unsigned128': [878929, 32788, 33627, 68120, 925583, 1104772],
    'uint128_t': [259725, 33723, 36799, 35334, 1020148, 1143344],
    'boost::mp::uint128_t': [1246502, 1437452, 1648131, 1459418, 2216648, 2089105]
}
"""

"""
# x86 MSVC
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    'std::_Unsigned128': [4215438, 199945, 1206168, 2282869, 5516964, 4551146],
    'uint128_t': [3883846, 208436, 210874, 2680359, 4328917, 4330152],
    'boost::mp::uint128_t': [2852442, 3242910, 3851129, 5378001, 6948267, 6294325]
}
"""

"""
# x64 MSVC
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    'std::_Unsigned128': [2060556, 261475, 178724, 146063, 1332838, 1465138],
    'uint128_t': [1921174, 106545, 124181, 136115, 1360295, 1471169],
    'boost::mp::uint128_t': [3009890, 2710279, 3059187, 3495634, 4852899, 3926336]
}
"""
"""
# ARM64 macOS
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    'unsigned __int128': [131902, 20613, 20484, 20160, 686521, 777084],
    'uint128_t': [133564, 17912, 18237, 20580, 699201, 724648],
    'boost::mp::uint128_t': [134182, 40176, 40311, 43285, 945928, 953117],
    'absl::uint128': [132366, 20178, 20207, 20049, 672398, 734229]
}
"""
"""
# x64 macOS
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    'unsigned __int128': [688225, 104921, 129150, 120363, 2333812, 2621949],
    'uint128_t': [712352, 124992, 102302, 119652, 1981469, 2219481],
    'boost::mp::uint128_t': [689146, 137819, 153484, 164100, 2784139, 2736682]
}
"""

# Linux x64
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    'unsigned __int128': [785130, 90260, 91143, 111803, 1058435, 1003366],
    'uint128_t': [765065, 85758, 91449, 90069, 901516, 830830],
    'boost::mp::uint128_t': [1363581, 89958, 91224, 113559, 1040071, 1001701],
    'absl::uint128': [766205, 89255, 89716, 89660, 1044710, 978533]
}

"""
# Linux ARM64
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    'unsigned __int128': [3427201, 194968, 193067, 263187, 2338258, 2260200],
    'uint128_t': [2078586, 159662, 161903, 201333, 2247175, 2097760],
    'boost::mp::uint128_t': [5026689, 587373, 330052, 972009, 2190856, 2227961],
    'absl::uint128': [3753922, 194070, 140777, 244420, 2223032, 2186750]
}
"""
"""
# Linux S390x
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    'unsigned __int128': [6803419, 546801, 590011, 891753, 3827125, 4925696],
    'uint128_t': [6280326, 618774, 359100, 1192196, 3201674, 3360251],
    'boost::mp::uint128_t': [7965082, 621572, 691515, 944289, 3997037, 5144403],
    'absl::uint128': [10515929, 1744226, 1527622, 1839038, 4913142, 5422155]
}
"""
"""
# Linux ppc64le
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    'unsigned __int128': [5242604, 221776, 222894, 194494, 4821119, 4955570],
    'uint128_t': [4450958, 193063, 175259, 192929, 4896360, 4273487],
    'boost::mp::uint128_t': [5704848, 847504, 786659, 795187, 5344637, 5407877]
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
ax1.set_title('GCC 14 - x64 Benchmark Results', fontsize=14, fontweight='bold')
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
ax2.set_title('GCC 14 - x64 Benchmark Results (Log Scale)', fontsize=14, fontweight='bold')
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

# Normalize data relative to unsigned __int128
normalized_df = df.copy()
for col in implementations:
    normalized_df[col] = df[col] / df['unsigned __int128']

# Plot normalized bars
for i, impl in enumerate(implementations):
    if impl == 'unsigned __int128':
        continue  # Skip since it's always 1.0
    bars = ax3.bar(x + (i-1.5)*width, normalized_df[impl], width,
                   label=impl, edgecolor='black', linewidth=0.5)

    # Add value labels
    for j, bar in enumerate(bars):
        height = bar.get_height()
        ax3.text(bar.get_x() + bar.get_width()/2., height,
                 f'{height:.2f}x', ha='center', va='bottom', fontsize=9)

# Add reference line at 1.0
ax3.axhline(y=1.0, color='red', linestyle='--', alpha=0.5, label='unsigned __int128 baseline')

ax3.set_xlabel('Operations', fontsize=12)
ax3.set_ylabel('Relative Performance (vs unsigned __int128)', fontsize=12)
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
    if impl == 'unsigned __int128':
        continue
    avg_ratio = normalized_df[impl].mean()
    print(f"{impl}: {avg_ratio:.2f}x average vs unsigned __int128")

print("\nBest performer by operation:")
print("-" * 50)
for i, op in enumerate(operations):
    row_data = df.iloc[i, 1:]
    best_impl = row_data.idxmin()
    best_time = row_data.min()
    print(f"{op}: {best_impl} ({best_time:,} ns)")

