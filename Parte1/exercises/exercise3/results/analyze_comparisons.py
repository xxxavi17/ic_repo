#!/usr/bin/env python3
"""
Comprehensive analysis and visualization for WAV comparison metrics
Author: Xavier
Date: October 2025
"""

import matplotlib.pyplot as plt
import numpy as np
import re
from pathlib import Path
import argparse

def parse_comparison_reports(results_dir="./"):
    """Parse all comparison reports and extract metrics"""
    
    metrics_data = {}
    
    # Look for comparison report files
    report_files = Path(results_dir).glob("comparison_*_report.md")
    
    for report_file in report_files:
        # Extract quantization level from filename
        match = re.search(r'comparison_sample_(\w+)_report\.md', str(report_file))
        if not match:
            continue
            
        quant_level = match.group(1)
        
        # Parse the report file
        with open(report_file, 'r') as f:
            content = f.read()
            
        # Extract metrics using regex
        channel_pattern = r'\| Channel (\d+) \| ([0-9.]+) \| ([0-9.]+) \| ([0-9.-]+) \|'
        average_pattern = r'\| \*\*Average\*\* \| \*\*([0-9.]+)\*\* \| \*\*([0-9.]+)\*\* \| \*\*([0-9.-]+)\*\* \|'
        
        channels = []
        for match in re.finditer(channel_pattern, content):
            channel_num = int(match.group(1))
            mse = float(match.group(2))
            max_error = float(match.group(3))
            snr = float(match.group(4))
            
            channels.append({
                'channel': channel_num,
                'mse': mse,
                'max_error': max_error,
                'snr': snr
            })
        
        # Extract average metrics
        avg_match = re.search(average_pattern, content)
        if avg_match:
            avg_mse = float(avg_match.group(1))
            avg_max_error = float(avg_match.group(2))
            avg_snr = float(avg_match.group(3))
            
            metrics_data[quant_level] = {
                'channels': channels,
                'average': {
                    'mse': avg_mse,
                    'max_error': avg_max_error,
                    'snr': avg_snr
                }
            }
    
    return metrics_data

def create_comprehensive_comparison_plots(metrics_data):
    """Create comprehensive comparison plots"""
    
    # Sort quantization levels
    quant_levels = ['8bit', '4bit', '2bit', '1bit']
    available_levels = [level for level in quant_levels if level in metrics_data]
    
    if not available_levels:
        print("No comparison data found!")
        return
    
    # Extract data for plotting
    bit_depths = [int(level.replace('bit', '')) for level in available_levels]
    mse_values = [metrics_data[level]['average']['mse'] for level in available_levels]
    max_error_values = [metrics_data[level]['average']['max_error'] for level in available_levels]
    snr_values = [metrics_data[level]['average']['snr'] for level in available_levels]
    
    # Create comprehensive figure
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))
    fig.suptitle('WAV Comparison Metrics Analysis - L2, L∞ Norms and SNR', fontsize=16, fontweight='bold')
    
    colors = ['blue', 'green', 'orange', 'red']
    
    # Plot 1: MSE (L2 norm) vs Bit Depth
    ax1.semilogy(bit_depths, mse_values, 'o-', linewidth=2, markersize=8, color='blue')
    ax1.set_xlabel('Bit Depth')
    ax1.set_ylabel('MSE (L2 norm) - Log Scale')
    ax1.set_title('Mean Squared Error vs Bit Depth')
    ax1.grid(True, alpha=0.3)
    ax1.invert_xaxis()  # Higher quality (more bits) on the right
    
    # Add annotations
    for i, (x, y) in enumerate(zip(bit_depths, mse_values)):
        ax1.annotate(f'{y:.0f}', (x, y), textcoords="offset points", xytext=(0,10), ha='center')
    
    # Plot 2: Max Absolute Error (L∞ norm) vs Bit Depth
    ax2.plot(bit_depths, max_error_values, 'o-', linewidth=2, markersize=8, color='red')
    ax2.set_xlabel('Bit Depth')
    ax2.set_ylabel('Max Absolute Error (L∞ norm)')
    ax2.set_title('Maximum Absolute Error vs Bit Depth')
    ax2.grid(True, alpha=0.3)
    ax2.invert_xaxis()
    
    # Add annotations
    for i, (x, y) in enumerate(zip(bit_depths, max_error_values)):
        ax2.annotate(f'{y:.0f}', (x, y), textcoords="offset points", xytext=(0,10), ha='center')
    
    # Plot 3: SNR vs Bit Depth
    ax3.plot(bit_depths, snr_values, 'o-', linewidth=2, markersize=8, color='green')
    ax3.set_xlabel('Bit Depth')
    ax3.set_ylabel('SNR (dB)')
    ax3.set_title('Signal-to-Noise Ratio vs Bit Depth')
    ax3.grid(True, alpha=0.3)
    ax3.invert_xaxis()
    
    # Add annotations
    for i, (x, y) in enumerate(zip(bit_depths, snr_values)):
        ax3.annotate(f'{y:.1f}', (x, y), textcoords="offset points", xytext=(0,10), ha='center')
    
    # Plot 4: All metrics normalized comparison
    # Normalize metrics to 0-100 scale for comparison
    mse_normalized = [100 * (max(mse_values) - mse) / max(mse_values) for mse in mse_values]
    max_error_normalized = [100 * (max(max_error_values) - err) / max(max_error_values) for err in max_error_values]
    snr_normalized = [100 * (snr - min(snr_values)) / (max(snr_values) - min(snr_values)) for snr in snr_values]
    
    x = np.arange(len(bit_depths))
    width = 0.25
    
    bars1 = ax4.bar(x - width, mse_normalized, width, label='MSE Quality', color='lightblue')
    bars2 = ax4.bar(x, max_error_normalized, width, label='Max Error Quality', color='lightgreen')
    bars3 = ax4.bar(x + width, snr_normalized, width, label='SNR Quality', color='lightcoral')
    
    ax4.set_xlabel('Quantization Level')
    ax4.set_ylabel('Normalized Quality Score (%)')
    ax4.set_title('Normalized Quality Metrics Comparison')
    ax4.set_xticks(x)
    ax4.set_xticklabels([f'{b}-bit' for b in bit_depths])
    ax4.legend()
    ax4.grid(True, alpha=0.3)
    
    # Add value labels on bars
    for bars in [bars1, bars2, bars3]:
        for bar in bars:
            height = bar.get_height()
            ax4.text(bar.get_x() + bar.get_width()/2., height,
                    f'{height:.0f}%', ha='center', va='bottom', fontsize=8)
    
    plt.tight_layout()
    plt.savefig('comprehensive_comparison_analysis.png', dpi=300, bbox_inches='tight')
    print("Comprehensive comparison analysis saved as: comprehensive_comparison_analysis.png")
    
    return fig

def create_error_norms_comparison(metrics_data):
    """Create specific comparison of L2 vs L∞ norms"""
    
    quant_levels = ['8bit', '4bit', '2bit', '1bit']
    available_levels = [level for level in quant_levels if level in metrics_data]
    
    if not available_levels:
        return
    
    bit_depths = [int(level.replace('bit', '')) for level in available_levels]
    mse_values = [metrics_data[level]['average']['mse'] for level in available_levels]
    max_error_values = [metrics_data[level]['average']['max_error'] for level in available_levels]
    
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
    fig.suptitle('Error Norms Comparison: L2 vs L∞', fontsize=16, fontweight='bold')
    
    # L2 vs L∞ scatter plot
    ax1.loglog(mse_values, max_error_values, 'o', markersize=10, color='purple')
    ax1.set_xlabel('MSE (L2 norm)')
    ax1.set_ylabel('Max Absolute Error (L∞ norm)')
    ax1.set_title('L2 vs L∞ Norm Relationship')
    ax1.grid(True, alpha=0.3)
    
    # Add labels for each point
    for i, (mse, max_err, bits) in enumerate(zip(mse_values, max_error_values, bit_depths)):
        ax1.annotate(f'{bits}-bit', (mse, max_err), 
                    textcoords="offset points", xytext=(10,5), ha='left')
    
    # Normalized comparison
    x = np.arange(len(bit_depths))
    width = 0.35
    
    ax2.bar(x - width/2, mse_values, width, label='MSE (L2 norm)', color='skyblue', alpha=0.7)
    ax2_twin = ax2.twinx()
    ax2_twin.bar(x + width/2, max_error_values, width, label='Max Error (L∞ norm)', color='lightcoral', alpha=0.7)
    
    ax2.set_xlabel('Quantization Level')
    ax2.set_ylabel('MSE (L2 norm)', color='blue')
    ax2_twin.set_ylabel('Max Absolute Error (L∞ norm)', color='red')
    ax2.set_title('L2 and L∞ Norms by Quantization Level')
    ax2.set_xticks(x)
    ax2.set_xticklabels([f'{b}-bit' for b in bit_depths])
    
    # Add legends
    ax2.legend(loc='upper left')
    ax2_twin.legend(loc='upper right')
    
    plt.tight_layout()
    plt.savefig('error_norms_comparison.png', dpi=300, bbox_inches='tight')
    print("Error norms comparison saved as: error_norms_comparison.png")
    
    return fig

def create_summary_table(metrics_data):
    """Create a visual summary table"""
    
    quant_levels = ['8bit', '4bit', '2bit', '1bit']
    available_levels = [level for level in quant_levels if level in metrics_data]
    
    if not available_levels:
        return
    
    # Prepare data for table
    table_data = []
    headers = ['Quantization', 'MSE (L2)', 'Max Error (L∞)', 'SNR (dB)', 'Quality Assessment']
    
    for level in available_levels:
        data = metrics_data[level]['average']
        
        # Quality assessment based on SNR
        if data['snr'] > 30:
            quality = 'Excellent'
        elif data['snr'] > 15:
            quality = 'Good'
        elif data['snr'] > 0:
            quality = 'Fair'
        else:
            quality = 'Poor'
        
        table_data.append([
            level.replace('bit', '-bit'),
            f"{data['mse']:.0f}",
            f"{data['max_error']:.0f}",
            f"{data['snr']:.1f}",
            quality
        ])
    
    fig, ax = plt.subplots(figsize=(12, 6))
    ax.axis('tight')
    ax.axis('off')
    
    table = ax.table(cellText=table_data,
                     colLabels=headers,
                     cellLoc='center',
                     loc='center')
    
    table.auto_set_font_size(False)
    table.set_fontsize(12)
    table.scale(1.2, 2)
    
    # Color code by quality
    quality_colors = {'Excellent': 'lightgreen', 'Good': 'lightblue', 'Fair': 'yellow', 'Poor': 'lightcoral'}
    
    for i, row in enumerate(table_data):
        quality = row[4]
        color = quality_colors.get(quality, 'white')
        table[(i+1, 4)].set_facecolor(color)
    
    # Header formatting
    for j in range(len(headers)):
        table[(0, j)].set_facecolor('lightgray')
        table[(0, j)].set_text_props(weight='bold')
    
    plt.title('WAV Comparison Metrics Summary', fontsize=16, fontweight='bold', pad=20)
    plt.savefig('comparison_metrics_summary.png', dpi=300, bbox_inches='tight')
    print("Comparison metrics summary saved as: comparison_metrics_summary.png")
    
    return fig

def main():
    parser = argparse.ArgumentParser(description='Generate WAV comparison analysis visualizations')
    parser.add_argument('--dir', default='./', help='Directory containing comparison reports')
    parser.add_argument('--all', action='store_true', help='Generate all plots')
    parser.add_argument('--comprehensive', action='store_true', help='Generate comprehensive analysis')
    parser.add_argument('--norms', action='store_true', help='Generate error norms comparison')
    parser.add_argument('--summary', action='store_true', help='Generate summary table')
    
    args = parser.parse_args()
    
    print("Parsing comparison reports...")
    metrics_data = parse_comparison_reports(args.dir)
    
    if not metrics_data:
        print("No comparison reports found! Make sure to run wav_cmp first.")
        return
    
    print(f"Found data for: {list(metrics_data.keys())}")
    
    if args.all or not any([args.comprehensive, args.norms, args.summary]):
        # Generate all plots by default
        create_comprehensive_comparison_plots(metrics_data)
        create_error_norms_comparison(metrics_data)
        create_summary_table(metrics_data)
    else:
        if args.comprehensive:
            create_comprehensive_comparison_plots(metrics_data)
        if args.norms:
            create_error_norms_comparison(metrics_data)
        if args.summary:
            create_summary_table(metrics_data)
    
    print("\nAll comparison visualizations generated successfully!")
    print("Files created:")
    print("- comprehensive_comparison_analysis.png")
    print("- error_norms_comparison.png")
    print("- comparison_metrics_summary.png")

if __name__ == "__main__":
    main()