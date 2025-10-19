#!/usr/bin/env python3
"""
Generate histograms for different quantization levels
Author: Xavier
Date: October 2025
"""

import matplotlib.pyplot as plt
import numpy as np
import struct
from pathlib import Path

def read_wav_samples(filename):
    """Read WAV file samples (simplified 16-bit PCM reader)"""
    with open(filename, 'rb') as f:
        # Skip WAV header (44 bytes for standard PCM)
        f.seek(44)
        
        # Read all remaining data as 16-bit samples
        data = f.read()
        samples = struct.unpack(f'<{len(data)//2}h', data)
        
    return np.array(samples)

def create_quantization_histograms():
    """Create histograms comparing different quantization levels"""
    
    # File paths
    files = {
        'Original (16-bit)': 'sample.wav',
        '8-bit': 'sample_8bit.wav',
        '4-bit': 'sample_4bit.wav',
        '2-bit': 'sample_2bit.wav'
    }
    
    # Create figure with subplots
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    fig.suptitle('Audio Sample Histograms - Quantization Comparison', fontsize=16, fontweight='bold')
    
    axes = axes.flatten()
    colors = ['blue', 'green', 'orange', 'red']
    
    for i, (label, filename) in enumerate(files.items()):
        try:
            # Read samples
            samples = read_wav_samples(filename)
            
            # Create histogram
            axes[i].hist(samples, bins=200, alpha=0.7, color=colors[i], density=True)
            axes[i].set_title(f'{label}', fontsize=12, fontweight='bold')
            axes[i].set_xlabel('Sample Value')
            axes[i].set_ylabel('Probability Density')
            axes[i].grid(True, alpha=0.3)
            
            # Add statistics
            mean_val = np.mean(samples)
            std_val = np.std(samples)
            unique_vals = len(np.unique(samples))
            
            stats_text = f'Mean: {mean_val:.1f}\nStd: {std_val:.1f}\nUnique: {unique_vals}'
            axes[i].text(0.02, 0.98, stats_text, transform=axes[i].transAxes, 
                        verticalalignment='top', bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
            
            print(f"Processed {filename}: {len(samples)} samples, {unique_vals} unique values")
            
        except Exception as e:
            print(f"Error processing {filename}: {e}")
            axes[i].text(0.5, 0.5, f'Error loading\n{filename}', 
                        transform=axes[i].transAxes, ha='center', va='center')
    
    plt.tight_layout()
    plt.savefig('quantization_histograms.png', dpi=300, bbox_inches='tight')
    print("Quantization histograms saved as: quantization_histograms.png")
    
    return fig

def create_quantization_levels_visualization():
    """Create visualization showing quantization levels"""
    
    fig, ax = plt.subplots(figsize=(12, 8))
    
    # Simulate quantization levels for different bit depths
    x = np.linspace(-32768, 32767, 1000)
    
    bit_depths = [16, 8, 4, 2]
    colors = ['blue', 'green', 'orange', 'red']
    
    for i, bits in enumerate(bit_depths):
        levels = 2**bits
        step = 65535 / (levels - 1)
        
        # Quantize the signal
        quantized = np.round((x + 32768) / step) * step - 32768
        quantized = np.clip(quantized, -32768, 32767)
        
        ax.plot(x, quantized, label=f'{bits}-bit ({levels} levels)', 
               color=colors[i], linewidth=2, alpha=0.8)
    
    ax.set_xlabel('Original Sample Value')
    ax.set_ylabel('Quantized Sample Value')
    ax.set_title('Quantization Function for Different Bit Depths')
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('quantization_levels.png', dpi=300, bbox_inches='tight')
    print("Quantization levels visualization saved as: quantization_levels.png")
    
    return fig

if __name__ == "__main__":
    print("Generating quantization histograms...")
    create_quantization_histograms()
    
    print("Generating quantization levels visualization...")
    create_quantization_levels_visualization()
    
    print("\nAll histogram visualizations generated successfully!")
    print("Files created:")
    print("- quantization_histograms.png")
    print("- quantization_levels.png")