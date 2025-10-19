#include <iostream>
#include <sndfile.h>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <iomanip>

class WAVQuantizer {
private:
    SNDFILE* inputFile;
    SNDFILE* outputFile;
    SF_INFO inputInfo;
    SF_INFO outputInfo;
    int targetBits;
    double quantizationStep;
    double maxValue;
    double minValue;
    
    // Statistics
    size_t totalSamples;
    double mse;
    double snr;
    
public:
    WAVQuantizer(const std::string& inputPath, const std::string& outputPath, int bits) 
        : inputFile(nullptr), outputFile(nullptr), targetBits(bits), totalSamples(0), mse(0.0), snr(0.0) {
        
        // Open input file
        inputInfo = {};
        inputFile = sf_open(inputPath.c_str(), SFM_READ, &inputInfo);
        if (!inputFile) {
            throw std::runtime_error("Error opening input file: " + std::string(sf_strerror(nullptr)));
        }
        
        // Setup output info (same as input but potentially different format)
        outputInfo = inputInfo;
        
        // Open output file
        outputFile = sf_open(outputPath.c_str(), SFM_WRITE, &outputInfo);
        if (!outputFile) {
            sf_close(inputFile);
            throw std::runtime_error("Error opening output file: " + std::string(sf_strerror(nullptr)));
        }
        
        // Calculate quantization parameters
        calculateQuantizationParameters();
    }
    
    ~WAVQuantizer() {
        if (inputFile) sf_close(inputFile);
        if (outputFile) sf_close(outputFile);
    }
    
    void calculateQuantizationParameters() {
        // For 16-bit input, range is [-32768, 32767]
        // For target bits, we need 2^targetBits levels
        int numLevels = 1 << targetBits;  // 2^targetBits
        
        // Full range of 16-bit signed integer
        minValue = -32768.0;
        maxValue = 32767.0;
        
        // Quantization step
        quantizationStep = (maxValue - minValue) / (numLevels - 1);
        
        std::cout << "Quantization parameters:" << std::endl;
        std::cout << "  Target bits: " << targetBits << std::endl;
        std::cout << "  Quantization levels: " << numLevels << std::endl;
        std::cout << "  Quantization step: " << quantizationStep << std::endl;
        std::cout << "  Range: [" << minValue << ", " << maxValue << "]" << std::endl;
    }
    
    short quantizeSample(short originalSample) {
        double sample = static_cast<double>(originalSample);
        
        // Normalize to [0, numLevels-1]
        double normalized = (sample - minValue) / quantizationStep;
        
        // Round to nearest quantization level
        int quantizedLevel = static_cast<int>(std::round(normalized));
        
        // Clamp to valid range
        quantizedLevel = std::max(0, std::min(quantizedLevel, (1 << targetBits) - 1));
        
        // Convert back to sample value
        double quantizedSample = minValue + quantizedLevel * quantizationStep;
        
        // Convert back to short, clamping to 16-bit range
        return static_cast<short>(std::max(-32768.0, std::min(32767.0, quantizedSample)));
    }
    
    void processFile() {
        const size_t bufferSize = 4096;
        std::vector<short> inputBuffer(bufferSize);
        std::vector<short> outputBuffer(bufferSize);
        
        sf_count_t framesRead;
        double sumSquaredError = 0.0;
        double sumOriginalSquared = 0.0;
        
        std::cout << "Processing audio file..." << std::endl;
        
        while ((framesRead = sf_readf_short(inputFile, inputBuffer.data(), bufferSize / inputInfo.channels)) > 0) {
            size_t samplesRead = framesRead * inputInfo.channels;
            
            // Quantize each sample
            for (size_t i = 0; i < samplesRead; i++) {
                short original = inputBuffer[i];
                short quantized = quantizeSample(original);
                outputBuffer[i] = quantized;
                
                // Calculate error statistics
                double error = static_cast<double>(original - quantized);
                sumSquaredError += error * error;
                sumOriginalSquared += static_cast<double>(original) * static_cast<double>(original);
                totalSamples++;
            }
            
            // Write quantized samples
            sf_writef_short(outputFile, outputBuffer.data(), framesRead);
        }
        
        // Calculate final statistics
        if (totalSamples > 0) {
            mse = sumSquaredError / totalSamples;
            if (sumOriginalSquared > 0) {
                snr = 10.0 * std::log10(sumOriginalSquared / sumSquaredError);
            }
        }
        
        std::cout << "Processing complete!" << std::endl;
        std::cout << "Total samples processed: " << totalSamples << std::endl;
        std::cout << "Mean Squared Error (MSE): " << mse << std::endl;
        std::cout << "Signal-to-Noise Ratio (SNR): " << snr << " dB" << std::endl;
    }
    
    void printFileInfo() {
        std::cout << "\nInput file information:" << std::endl;
        std::cout << "  Frames: " << inputInfo.frames << std::endl;
        std::cout << "  Sample rate: " << inputInfo.samplerate << " Hz" << std::endl;
        std::cout << "  Channels: " << inputInfo.channels << std::endl;
        std::cout << "  Duration: " << std::fixed << std::setprecision(2) 
                  << static_cast<double>(inputInfo.frames) / inputInfo.samplerate << " seconds" << std::endl;
        std::cout << "  Format: " << std::hex << "0x" << inputInfo.format << std::dec << std::endl;
    }
    
    double getMSE() const { return mse; }
    double getSNR() const { return snr; }
    size_t getTotalSamples() const { return totalSamples; }
};

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <input.wav> <output.wav> <target_bits> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Parameters:" << std::endl;
    std::cout << "  input.wav    - Input WAV file (16-bit PCM)" << std::endl;
    std::cout << "  output.wav   - Output quantized WAV file" << std::endl;
    std::cout << "  target_bits  - Target bits per sample (1-16)" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -v, --verbose    - Verbose output with file information" << std::endl;
    std::cout << "  -h, --help       - Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " input.wav output_8bit.wav 8" << std::endl;
    std::cout << "  " << programName << " input.wav output_4bit.wav 4 -v" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string inputPath = argv[1];
    std::string outputPath = argv[2];
    int targetBits = std::atoi(argv[3]);
    
    bool verbose = false;
    
    // Parse options
    for (int i = 4; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
    }
    
    // Validate target bits
    if (targetBits < 1 || targetBits > 16) {
        std::cerr << "Error: Target bits must be between 1 and 16" << std::endl;
        return 1;
    }
    
    try {
        std::cout << "WAV Quantizer - Reducing audio to " << targetBits << " bits per sample" << std::endl;
        std::cout << "Input: " << inputPath << std::endl;
        std::cout << "Output: " << outputPath << std::endl;
        std::cout << "===========================================" << std::endl;
        
        WAVQuantizer quantizer(inputPath, outputPath, targetBits);
        
        if (verbose) {
            quantizer.printFileInfo();
            std::cout << std::endl;
        }
        
        quantizer.processFile();
        
        std::cout << "\nQuantization Results Summary:" << std::endl;
        std::cout << "  Original bits: 16" << std::endl;
        std::cout << "  Target bits: " << targetBits << std::endl;
        std::cout << "  Compression ratio: " << std::fixed << std::setprecision(2) 
                  << 16.0 / targetBits << ":1" << std::endl;
        std::cout << "  MSE: " << quantizer.getMSE() << std::endl;
        std::cout << "  SNR: " << quantizer.getSNR() << " dB" << std::endl;
        
        std::cout << "\nOutput file saved: " << outputPath << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}