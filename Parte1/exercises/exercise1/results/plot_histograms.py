#!/usr/bin/env python3
import matplotlib
matplotlib.use('Agg')  # Use non-interactive backend
import matplotlib.pyplot as plt
import numpy as np

def load_histogram(filename):
    values, counts = [], []
    with open(filename, 'r') as f:
        for line in f:
            if line.startswith('#'):
                continue
            parts = line.strip().split('\t')
            if len(parts) == 2:
                values.append(int(parts[0]))
                counts.append(int(parts[1]))
    return np.array(values), np.array(counts)

fig, axes = plt.subplots(2, 2, figsize=(15, 10))
fig.suptitle('WAV Audio Histograms - Exercise 1', fontsize=16)

# Plot Left Channel
values, counts = load_histogram('sample_left_bin1.txt')
axes[0, 0].bar(values, counts, width=1, alpha=0.7)
axes[0, 0].set_title('Left Channel')
axes[0, 0].set_xlabel('Sample Value')
axes[0, 0].set_ylabel('Count')
axes[0, 0].grid(True, alpha=0.3)

# Plot Right Channel
values, counts = load_histogram('sample_right_bin1.txt')
axes[0, 1].bar(values, counts, width=1, alpha=0.7)
axes[0, 1].set_title('Right Channel')
axes[0, 1].set_xlabel('Sample Value')
axes[0, 1].set_ylabel('Count')
axes[0, 1].grid(True, alpha=0.3)

# Plot MID Channel ((L+R)/2)
values, counts = load_histogram('sample_mid_bin1.txt')
axes[1, 0].bar(values, counts, width=1, alpha=0.7)
axes[1, 0].set_title('MID Channel ((L+R)/2)')
axes[1, 0].set_xlabel('Sample Value')
axes[1, 0].set_ylabel('Count')
axes[1, 0].grid(True, alpha=0.3)

# Plot SIDE Channel ((L-R)/2)
values, counts = load_histogram('sample_side_bin1.txt')
axes[1, 1].bar(values, counts, width=1, alpha=0.7)
axes[1, 1].set_title('SIDE Channel ((L-R)/2)')
axes[1, 1].set_xlabel('Sample Value')
axes[1, 1].set_ylabel('Count')
axes[1, 1].grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('histograms_exercise1.png', dpi=300, bbox_inches='tight')
print('Histogram visualization saved as histograms_exercise1.png')
plt.close()  # Close the figure to free memory
