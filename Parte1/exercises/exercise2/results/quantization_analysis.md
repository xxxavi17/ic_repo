# Audio Quantization Analysis Report

## Summary Table

| Filename | Bits | MSE | SNR (dB) | PSNR (dB) | Size (KB) | Compression Ratio |
|----------|------|-----|----------|-----------|-----------|-------------------|
| sample.wav | 16 | 0.00 | 0.00 | 0.00 | 2067.2 | 1.0:1 |
| sample_8bit.wav | 8 | 5510.13 | 41.28 | 52.90 | 2067.2 | 2.0:1 |
| sample_4bit.wav | 4 | 1592431.19 | 16.67 | 28.29 | 2067.2 | 4.0:1 |
| sample_2bit.wav | 2 | 43040145.29 | 2.36 | 13.97 | 2067.2 | 8.0:1 |
| sample_1bit.wav | 1 | 704271041.89 | -9.78 | 1.83 | 2067.2 | 16.0:1 |

## Analysis

### Quality vs Compression Trade-off

- **16-bit quantization**: SNR = 0.00 dB, Compression = 1.0:1
- **8-bit quantization**: SNR = 41.28 dB, Compression = 2.0:1
- **4-bit quantization**: SNR = 16.67 dB, Compression = 4.0:1
- **2-bit quantization**: SNR = 2.36 dB, Compression = 8.0:1
- **1-bit quantization**: SNR = -9.78 dB, Compression = 16.0:1

### Observations

- Lower bit depths result in higher compression ratios but significantly reduced audio quality
- SNR decreases exponentially as bit depth is reduced
- 8-bit quantization provides reasonable quality with 2:1 compression
- 1-bit quantization results in severe quality degradation (negative SNR)
