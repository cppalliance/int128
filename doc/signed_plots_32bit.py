import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

"""
# Linux x86_32
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    'int128_t': [9530060, 785799, 778881, 1148024, 10337258, 10438037],
    'boost::mp::int128_t': [12168353, 7777469, 8214089, 9477355, 22857709, 14848256]
}
"""

# Linux ARM32
data = {
    'Operation': ['Comparisons', 'Addition', 'Subtraction', 'Multiplication', 'Division', 'Modulo'],
    'int128_t': [6149439, 457850, 488321, 1793874, 17738614, 18064819],
    'boost::mp::int128_t': [6432579, 5669571, 7464427, 11410321, 38956122, 30144743]
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
ax1.set_title('GCC 14 - ARM32 Benchmark Results', fontsize=14, fontweight='bold')
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
ax2.set_title('GCC 14 - ARM32 Benchmark Results (Log Scale)', fontsize=14, fontweight='bold')
ax2.set_yscale('log')
ax2.set_xticks(x)
ax2.set_xticklabels(operations, rotation=45, ha='right')
ax2.legend(loc='upper left')
ax2.grid(axis='y', alpha=0.3, which='both')

plt.tight_layout()
plt.savefig('ARM32_benchmarks.png', dpi=300, bbox_inches='tight')
plt.show()

# Create a normalized performance chart
fig3, ax3 = plt.subplots(figsize=(10, 6))

# Normalize data relative to boost::mp::int128_t
normalized_df = df.copy()
for col in implementations:
    normalized_df[col] = df[col] / df['boost::mp::int128_t']

# Plot normalized bars
for i, impl in enumerate(implementations):
    if impl == 'boost::mp::int128_t':
        continue  # Skip since it's always 1.0
    bars = ax3.bar(x + (i-1.5)*width, normalized_df[impl], width,
                   label=impl, edgecolor='black', linewidth=0.5)

    # Add value labels
    for j, bar in enumerate(bars):
        height = bar.get_height()
        ax3.text(bar.get_x() + bar.get_width()/2., height,
                 f'{height:.2f}x', ha='center', va='bottom', fontsize=9)

# Add reference line at 1.0
ax3.axhline(y=1.0, color='red', linestyle='--', alpha=0.5, label='boost::mp::int128_t baseline')

ax3.set_xlabel('Operations', fontsize=12)
ax3.set_ylabel('Relative Performance (vs boost::mp::int128_t)', fontsize=12)
ax3.set_title('Relative Performance Comparison - ARM32', fontsize=14, fontweight='bold')
ax3.set_xticks(x)
ax3.set_xticklabels(operations, rotation=45, ha='right')
ax3.legend()
ax3.grid(axis='y', alpha=0.3)

# Add interpretation text
ax3.text(0.02, 0.98, 'Lower is better', transform=ax3.transAxes,
         fontsize=10, verticalalignment='top', style='italic')

plt.tight_layout()
plt.savefig('ARM32_relative_performance.png', dpi=300, bbox_inches='tight')
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

