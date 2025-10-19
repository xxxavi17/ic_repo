#!/usr/bin/env python3
"""
Visualization script for WAV Quantization Analysis
Author: Xavier
Date: October 2025
"""

import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
import argparse

# Set style for better plots
plt.style.use('default')

def create_quantization_comparison_plots():
    """Create comprehensive plots comparing different quantization levels"""
    
    # Data from our analysis
    data = {
        'Bits': [16, 8, 4, 2, 1],
        'SNR_dB': [float('inf'), 41.28, 16.67, 2.36, -9.78],
        'MSE': [0, 5510.13, 1592431.19, 43040145.29, 704271041.89],
        'PSNR_dB': [float('inf'), 52.90, 28.29, 13.97, 1.83],
        'Compression_Ratio': [1, 2, 4, 8, 16],
        'Size_KB': [2067.2] * 5  # All files same size in our test
    }
    
    # Convert inf to a large number for plotting
    snr_plot = [100 if x == float('inf') else x for x in data['SNR_dB']]
    psnr_plot = [100 if x == float('inf') else x for x in data['PSNR_dB']]
    mse_plot = [1e-6 if x == 0 else x for x in data['MSE']]  # Small value for log scale
    
    # Create figure with subplots
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))
    fig.suptitle('WAV Audio Quantization Analysis', fontsize=16, fontweight='bold')
    
    # Plot 1: SNR vs Bit Depth
    ax1.plot(data['Bits'], snr_plot, 'o-', linewidth=2, markersize=8, color='blue')
    ax1.set_xlabel('Bit Depth')
    ax1.set_ylabel('SNR (dB)')
    ax1.set_title('Signal-to-Noise Ratio vs Bit Depth')
    ax1.grid(True, alpha=0.3)
    ax1.set_ylim(-15, 105)
    
    # Add annotations
    for i, (x, y) in enumerate(zip(data['Bits'], snr_plot)):
        if y < 100:  # Don't annotate the infinite value
            ax1.annotate(f'{data["SNR_dB"][i]:.1f}', (x, y), 
                        textcoords="offset points", xytext=(0,10), ha='center')
    
    # Plot 2: MSE vs Bit Depth (log scale)
    ax2.semilogy(data['Bits'], mse_plot, 'o-', linewidth=2, markersize=8, color='red')
    ax2.set_xlabel('Bit Depth')
    ax2.set_ylabel('Mean Squared Error (log scale)')
    ax2.set_title('Mean Squared Error vs Bit Depth')
    ax2.grid(True, alpha=0.3)
    
    # Plot 3: Quality vs Compression Trade-off
    compression_plot = [x for x in data['Compression_Ratio']]
    snr_for_tradeoff = [x for x in data['SNR_dB'] if x != float('inf')]
    compression_for_tradeoff = compression_plot[1:]  # Skip 16-bit (no compression)
    
    ax3.plot(compression_for_tradeoff, snr_for_tradeoff, 'o-', linewidth=2, markersize=8, color='green')
    ax3.set_xlabel('Compression Ratio (x:1)')
    ax3.set_ylabel('SNR (dB)')
    ax3.set_title('Quality vs Compression Trade-off')
    ax3.grid(True, alpha=0.3)
    
    # Add bit depth labels
    for i, (x, y, bits) in enumerate(zip(compression_for_tradeoff, snr_for_tradeoff, data['Bits'][1:])):
        ax3.annotate(f'{bits}-bit', (x, y), 
                    textcoords="offset points", xytext=(0,10), ha='center')
    
    # Plot 4: PSNR vs Bit Depth
    ax4.plot(data['Bits'], psnr_plot, 'o-', linewidth=2, markersize=8, color='purple')
    ax4.set_xlabel('Bit Depth')
    ax4.set_ylabel('PSNR (dB)')
    ax4.set_title('Peak Signal-to-Noise Ratio vs Bit Depth')
    ax4.grid(True, alpha=0.3)
    ax4.set_ylim(0, 105)
    
    # Add annotations
    for i, (x, y) in enumerate(zip(data['Bits'], psnr_plot)):
        if y < 100:  # Don't annotate the infinite value
            ax4.annotate(f'{data["PSNR_dB"][i]:.1f}', (x, y), 
                        textcoords="offset points", xytext=(0,10), ha='center')
    
    plt.tight_layout()
    plt.savefig('quantization_analysis.png', dpi=300, bbox_inches='tight')
    print("Quantization analysis plot saved as: quantization_analysis.png")
    
    return fig

def create_bit_depth_comparison_bar_chart():
    """Create a bar chart comparing all metrics across bit depths"""
    
    # Normalized data for comparison (all metrics scaled to 0-100)
    bits = [8, 4, 2, 1]  # Skip 16-bit for clarity
    snr_normalized = [41.28/41.28*100, 16.67/41.28*100, 2.36/41.28*100, -9.78/41.28*100]
    compression_normalized = [2/16*100, 4/16*100, 8/16*100, 16/16*100]
    
    # Make SNR values positive for visualization
    snr_positive = [max(0, x) for x in snr_normalized]
    
    x = np.arange(len(bits))
    width = 0.35
    
    fig, ax = plt.subplots(figsize=(12, 8))
    
    bars1 = ax.bar(x - width/2, snr_positive, width, label='SNR Quality', color='skyblue')
    bars2 = ax.bar(x + width/2, compression_normalized, width, label='Compression Ratio', color='lightcoral')
    
    ax.set_xlabel('Bit Depth')
    ax.set_ylabel('Normalized Value (%)')
    ax.set_title('Audio Quality vs Compression Efficiency')
    ax.set_xticks(x)
    ax.set_xticklabels([f'{b}-bit' for b in bits])
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    # Add value labels on bars
    for bar in bars1:
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2., height,
                f'{height:.1f}%', ha='center', va='bottom')
    
    for bar in bars2:
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2., height,
                f'{height:.1f}%', ha='center', va='bottom')
    
    plt.tight_layout()
    plt.savefig('bit_depth_comparison.png', dpi=300, bbox_inches='tight')
    print("Bit depth comparison chart saved as: bit_depth_comparison.png")
    
    return fig

def create_summary_table_plot():
    """Create a formatted table as an image"""
    
    data = {
        'Bit Depth': ['16-bit (Original)', '8-bit', '4-bit', '2-bit', '1-bit'],
        'SNR (dB)': ['∞ (Perfect)', '41.28', '16.67', '2.36', '-9.78'],
        'MSE': ['0', '5,510', '1,592,431', '43,040,145', '704,271,042'],
        'Compression': ['1:1', '2:1', '4:1', '8:1', '16:1'],
        'Quality': ['Perfect', 'Excellent', 'Good', 'Poor', 'Very Poor']
    }
    
    fig, ax = plt.subplots(figsize=(12, 6))
    ax.axis('tight')
    ax.axis('off')
    
    table = ax.table(cellText=[list(row) for row in zip(*data.values())],
                     colLabels=list(data.keys()),
                     cellLoc='center',
                     loc='center')
    
    table.auto_set_font_size(False)
    table.set_fontsize(12)
    table.scale(1.2, 1.5)
    
    # Color code the quality column
    quality_colors = ['lightgreen', 'lightblue', 'yellow', 'orange', 'lightcoral']
    for i, color in enumerate(quality_colors):
        table[(i+1, 4)].set_facecolor(color)
    
    # Header formatting
    for i in range(len(data)):
        table[(0, i)].set_facecolor('lightgray')
        table[(0, i)].set_text_props(weight='bold')
    
    plt.title('Audio Quantization Summary Table', fontsize=16, fontweight='bold', pad=20)
    plt.savefig('quantization_summary_table.png', dpi=300, bbox_inches='tight')
    print("Summary table saved as: quantization_summary_table.png")
    
    return fig

def main():
    parser = argparse.ArgumentParser(description='Generate quantization analysis visualizations')
    parser.add_argument('--all', action='store_true', help='Generate all plots')
    parser.add_argument('--comparison', action='store_true', help='Generate comparison plots')
    parser.add_argument('--bar-chart', action='store_true', help='Generate bar chart')
    parser.add_argument('--table', action='store_true', help='Generate summary table')
    
    args = parser.parse_args()
    
    if args.all or not any([args.comparison, args.bar_chart, args.table]):
        # Generate all plots by default
        create_quantization_comparison_plots()
        create_bit_depth_comparison_bar_chart()
        create_summary_table_plot()
    else:
        if args.comparison:
            create_quantization_comparison_plots()
        if args.bar_chart:
            create_bit_depth_comparison_bar_chart()
        if args.table:
            create_summary_table_plot()
    
    print("\nAll visualizations generated successfully!")
    print("Files created:")
    print("- quantization_analysis.png")
    print("- bit_depth_comparison.png") 
    print("- quantization_summary_table.png")

if __name__ == "__main__":
    main()