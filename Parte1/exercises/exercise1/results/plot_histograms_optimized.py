#!/usr/bin/env python3
import matplotlib
matplotlib.use('Agg')  # Use non-interactive backend
import matplotlib.pyplot as plt
import numpy as np

def load_histogram(filename):
    """Load histogram data from file"""
    try:
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
    except FileNotFoundError:
        print(f"Warning: File {filename} not found")
        return np.array([]), np.array([])

def create_histograms():
    """Create histogram visualizations"""
    fig, axes = plt.subplots(2, 2, figsize=(15, 10))
    fig.suptitle('WAV Audio Histograms - Exercise 1 (Bin Size = 1)', fontsize=16)

    # File names for bin size 1
    files = [
        ('sample_left_bin1.txt', 'Left Channel'),
        ('sample_right_bin1.txt', 'Right Channel'), 
        ('sample_mid_bin1.txt', 'MID Channel ((L+R)/2)'),
        ('sample_side_bin1.txt', 'SIDE Channel ((L-R)/2)')
    ]
    
    positions = [(0, 0), (0, 1), (1, 0), (1, 1)]
    
    for i, ((filename, title), pos) in enumerate(zip(files, positions)):
        values, counts = load_histogram(filename)
        
        if len(values) > 0:
            # Use plot instead of bar for efficiency with many points
            axes[pos].plot(values, counts, linewidth=0.8, alpha=0.8)
            axes[pos].fill_between(values, counts, alpha=0.3)
            
        axes[pos].set_title(title)
        axes[pos].set_xlabel('Sample Value')
        axes[pos].set_ylabel('Count')
        axes[pos].grid(True, alpha=0.3)
        
        # Set reasonable limits to focus on the data
        if len(values) > 0:
            axes[pos].set_xlim(values.min(), values.max())

    plt.tight_layout()
    
    # Save the figure
    output_file = 'histograms_exercise1_bin1.png'
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f'Histogram visualization saved as {output_file}')
    plt.close()

def create_comparison_binning():
    """Create comparison of different bin sizes"""
    fig, axes = plt.subplots(2, 2, figsize=(15, 10))
    fig.suptitle('MID Channel Histogram - Different Bin Sizes Comparison', fontsize=16)

    bin_sizes = [1, 4, 16]
    colors = ['blue', 'red', 'green']
    
    for i, (bin_size, color) in enumerate(zip(bin_sizes, colors)):
        filename = f'sample_mid_bin{bin_size}.txt'
        values, counts = load_histogram(filename)
        
        if len(values) > 0:
            # Plot in the first subplot (comparison)
            axes[0, 0].plot(values, counts, linewidth=1.0, alpha=0.8, 
                           label=f'Bin Size = {bin_size}', color=color)
            
            # Plot individual histograms
            if i < 3:
                row = (i + 1) // 2
                col = (i + 1) % 2
                axes[row, col].plot(values, counts, linewidth=1.0, alpha=0.8, color=color)
                axes[row, col].fill_between(values, counts, alpha=0.3, color=color)
                axes[row, col].set_title(f'MID Channel - Bin Size = {bin_size}')
                axes[row, col].set_xlabel('Sample Value')
                axes[row, col].set_ylabel('Count')
                axes[row, col].grid(True, alpha=0.3)
    
    axes[0, 0].set_title('MID Channel - Bin Size Comparison')
    axes[0, 0].set_xlabel('Sample Value')
    axes[0, 0].set_ylabel('Count')
    axes[0, 0].legend()
    axes[0, 0].grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    # Save the figure
    output_file = 'histograms_binsize_comparison.png'
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f'Bin size comparison saved as {output_file}')
    plt.close()

if __name__ == "__main__":
    print("Generating histogram visualizations...")
    create_histograms()
    create_comparison_binning()
    print("Done!")