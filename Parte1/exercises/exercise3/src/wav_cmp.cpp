#include <iostream>
#include <fstream>
#include <sndfile.h>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <algorithm>

class WAVComparator {
private:
    struct ChannelMetrics {
        double mse;           // L2 norm (Mean Squared Error)
        double maxAbsError;   // L∞ norm (Maximum Absolute Error)
        double snr;           // Signal-to-Noise Ratio
        std::string name;
    };
    
    std::vector<ChannelMetrics> channelMetrics;
    ChannelMetrics averageMetrics;
    
    SNDFILE* originalFile;
    SNDFILE* comparedFile;
    SF_INFO originalInfo;
    SF_INFO comparedInfo;
    
public:
    WAVComparator(const std::string& originalPath, const std::string& comparedPath) 
        : originalFile(nullptr), comparedFile(nullptr) {
        
        // Open original file
        originalInfo = {};
        originalFile = sf_open(originalPath.c_str(), SFM_READ, &originalInfo);
        if (!originalFile) {
            throw std::runtime_error("Error opening original file: " + std::string(sf_strerror(nullptr)));
        }
        
        // Open compared file
        comparedInfo = {};
        comparedFile = sf_open(comparedPath.c_str(), SFM_READ, &comparedInfo);
        if (!comparedFile) {
            sf_close(originalFile);
            throw std::runtime_error("Error opening compared file: " + std::string(sf_strerror(nullptr)));
        }
        
        // Validate compatibility
        if (originalInfo.channels != comparedInfo.channels) {
            sf_close(originalFile);
            sf_close(comparedFile);
            throw std::runtime_error("Files must have the same number of channels");
        }
        
        if (originalInfo.samplerate != comparedInfo.samplerate) {
            std::cerr << "Warning: Files have different sample rates" << std::endl;
        }
        
        // Initialize metrics structures
        channelMetrics.resize(originalInfo.channels);
        for (int i = 0; i < originalInfo.channels; i++) {
            channelMetrics[i].name = "Channel " + std::to_string(i + 1);
            channelMetrics[i].mse = 0.0;
            channelMetrics[i].maxAbsError = 0.0;
            channelMetrics[i].snr = 0.0;
        }
        
        averageMetrics.name = "Average";
        averageMetrics.mse = 0.0;
        averageMetrics.maxAbsError = 0.0;
        averageMetrics.snr = 0.0;
    }
    
    ~WAVComparator() {
        if (originalFile) sf_close(originalFile);
        if (comparedFile) sf_close(comparedFile);
    }
    
    void calculateMetrics() {
        const size_t bufferSize = 4096;
        std::vector<short> originalBuffer(bufferSize);
        std::vector<short> comparedBuffer(bufferSize);
        
        // Per-channel accumulators
        std::vector<double> channelMSE(originalInfo.channels, 0.0);
        std::vector<double> channelMaxError(originalInfo.channels, 0.0);
        std::vector<double> channelOriginalPower(originalInfo.channels, 0.0);
        std::vector<double> channelErrorPower(originalInfo.channels, 0.0);
        std::vector<size_t> channelSampleCount(originalInfo.channels, 0);
        
        // Overall accumulators for average
        double totalMSE = 0.0;
        double totalMaxError = 0.0;
        double totalOriginalPower = 0.0;
        double totalErrorPower = 0.0;
        size_t totalSamples = 0;
        
        sf_count_t framesRead;
        
        std::cout << "Calculating comparison metrics..." << std::endl;
        
        // Process files in chunks
        while (true) {
            sf_count_t originalFrames = sf_readf_short(originalFile, originalBuffer.data(), bufferSize / originalInfo.channels);
            sf_count_t comparedFrames = sf_readf_short(comparedFile, comparedBuffer.data(), bufferSize / comparedInfo.channels);
            
            if (originalFrames == 0 || comparedFrames == 0) {
                break;
            }
            
            // Use minimum frames to handle potential size differences
            sf_count_t framesToProcess = std::min(originalFrames, comparedFrames);
            size_t samplesToProcess = framesToProcess * originalInfo.channels;
            
            // Process samples
            for (size_t i = 0; i < samplesToProcess; i++) {
                int channel = i % originalInfo.channels;
                
                double originalSample = static_cast<double>(originalBuffer[i]);
                double comparedSample = static_cast<double>(comparedBuffer[i]);
                double error = originalSample - comparedSample;
                double absError = std::abs(error);
                
                // Per-channel metrics
                channelMSE[channel] += error * error;
                channelMaxError[channel] = std::max(channelMaxError[channel], absError);
                channelOriginalPower[channel] += originalSample * originalSample;
                channelErrorPower[channel] += error * error;
                channelSampleCount[channel]++;
                
                // Overall metrics for average
                totalMSE += error * error;
                totalMaxError = std::max(totalMaxError, absError);
                totalOriginalPower += originalSample * originalSample;
                totalErrorPower += error * error;
                totalSamples++;
            }
        }
        
        // Calculate final per-channel metrics
        for (int ch = 0; ch < originalInfo.channels; ch++) {
            if (channelSampleCount[ch] > 0) {
                channelMetrics[ch].mse = channelMSE[ch] / channelSampleCount[ch];
                channelMetrics[ch].maxAbsError = channelMaxError[ch];
                
                // Calculate SNR: 10 * log10(signal_power / noise_power)
                if (channelErrorPower[ch] > 0 && channelOriginalPower[ch] > 0) {
                    channelMetrics[ch].snr = 10.0 * std::log10(channelOriginalPower[ch] / channelErrorPower[ch]);
                } else if (channelErrorPower[ch] == 0) {
                    channelMetrics[ch].snr = std::numeric_limits<double>::infinity();
                } else {
                    channelMetrics[ch].snr = -std::numeric_limits<double>::infinity();
                }
            }
        }
        
        // Calculate average metrics
        if (totalSamples > 0) {
            averageMetrics.mse = totalMSE / totalSamples;
            averageMetrics.maxAbsError = totalMaxError;
            
            if (totalErrorPower > 0 && totalOriginalPower > 0) {
                averageMetrics.snr = 10.0 * std::log10(totalOriginalPower / totalErrorPower);
            } else if (totalErrorPower == 0) {
                averageMetrics.snr = std::numeric_limits<double>::infinity();
            } else {
                averageMetrics.snr = -std::numeric_limits<double>::infinity();
            }
        }
        
        std::cout << "Metrics calculation completed." << std::endl;
        std::cout << "Processed " << totalSamples << " samples (" << totalSamples / originalInfo.channels << " frames)" << std::endl;
    }
    
    void printMetrics() const {
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "AUDIO COMPARISON METRICS" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        std::cout << std::left 
                  << std::setw(12) << "Channel"
                  << std::setw(15) << "MSE (L2 norm)"
                  << std::setw(18) << "Max Abs Err (L∞)"
                  << std::setw(12) << "SNR (dB)"
                  << std::endl;
        
        std::cout << std::string(80, '-') << std::endl;
        
        // Print per-channel metrics
        for (const auto& metrics : channelMetrics) {
            std::cout << std::left 
                      << std::setw(12) << metrics.name
                      << std::setw(15) << std::fixed << std::setprecision(3) << metrics.mse
                      << std::setw(18) << std::fixed << std::setprecision(3) << metrics.maxAbsError;
            
            if (std::isinf(metrics.snr)) {
                if (metrics.snr > 0) {
                    std::cout << std::setw(12) << "∞";
                } else {
                    std::cout << std::setw(12) << "-∞";
                }
            } else {
                std::cout << std::setw(12) << std::fixed << std::setprecision(2) << metrics.snr;
            }
            std::cout << std::endl;
        }
        
        std::cout << std::string(80, '-') << std::endl;
        
        // Print average metrics
        std::cout << std::left 
                  << std::setw(12) << averageMetrics.name
                  << std::setw(15) << std::fixed << std::setprecision(3) << averageMetrics.mse
                  << std::setw(18) << std::fixed << std::setprecision(3) << averageMetrics.maxAbsError;
        
        if (std::isinf(averageMetrics.snr)) {
            if (averageMetrics.snr > 0) {
                std::cout << std::setw(12) << "∞";
            } else {
                std::cout << std::setw(12) << "-∞";
            }
        } else {
            std::cout << std::setw(12) << std::fixed << std::setprecision(2) << averageMetrics.snr;
        }
        std::cout << std::endl;
        
        std::cout << std::string(80, '=') << std::endl;
    }
    
    void printFileInfo() const {
        std::cout << "\nFile Information:" << std::endl;
        std::cout << "  Original file:" << std::endl;
        std::cout << "    Frames: " << originalInfo.frames << std::endl;
        std::cout << "    Sample rate: " << originalInfo.samplerate << " Hz" << std::endl;
        std::cout << "    Channels: " << originalInfo.channels << std::endl;
        std::cout << "    Duration: " << std::fixed << std::setprecision(2) 
                  << static_cast<double>(originalInfo.frames) / originalInfo.samplerate << " seconds" << std::endl;
        
        std::cout << "  Compared file:" << std::endl;
        std::cout << "    Frames: " << comparedInfo.frames << std::endl;
        std::cout << "    Sample rate: " << comparedInfo.samplerate << " Hz" << std::endl;
        std::cout << "    Channels: " << comparedInfo.channels << std::endl;
        std::cout << "    Duration: " << std::fixed << std::setprecision(2) 
                  << static_cast<double>(comparedInfo.frames) / comparedInfo.samplerate << " seconds" << std::endl;
    }
    
    void saveReport(const std::string& outputFile) const {
        std::ofstream out(outputFile);
        if (!out) {
            std::cerr << "Error creating report file: " << outputFile << std::endl;
            return;
        }
        
        out << "# Audio Comparison Report\n\n";
        out << "## Metrics Summary\n\n";
        out << "| Channel | MSE (L2 norm) | Max Abs Error (L∞ norm) | SNR (dB) |\n";
        out << "|---------|---------------|--------------------------|----------|\n";
        
        // Per-channel metrics
        for (const auto& metrics : channelMetrics) {
            out << "| " << metrics.name
                << " | " << std::fixed << std::setprecision(3) << metrics.mse
                << " | " << std::fixed << std::setprecision(3) << metrics.maxAbsError
                << " | ";
            
            if (std::isinf(metrics.snr)) {
                out << (metrics.snr > 0 ? "∞" : "-∞");
            } else {
                out << std::fixed << std::setprecision(2) << metrics.snr;
            }
            out << " |\n";
        }
        
        // Average metrics
        out << "| **" << averageMetrics.name << "**"
            << " | **" << std::fixed << std::setprecision(3) << averageMetrics.mse << "**"
            << " | **" << std::fixed << std::setprecision(3) << averageMetrics.maxAbsError << "**"
            << " | **";
        
        if (std::isinf(averageMetrics.snr)) {
            out << (averageMetrics.snr > 0 ? "∞" : "-∞");
        } else {
            out << std::fixed << std::setprecision(2) << averageMetrics.snr;
        }
        out << "** |\n\n";
        
        out << "## Definitions\n\n";
        out << "- **MSE (L2 norm)**: Mean Squared Error - average of squared differences\n";
        out << "- **Max Abs Error (L∞ norm)**: Maximum absolute difference between samples\n";
        out << "- **SNR**: Signal-to-Noise Ratio in dB - 10 * log10(signal_power / noise_power)\n";
        
        out.close();
        std::cout << "Comparison report saved to: " << outputFile << std::endl;
    }
    
    // Getters for further analysis
    const std::vector<ChannelMetrics>& getChannelMetrics() const { return channelMetrics; }
    const ChannelMetrics& getAverageMetrics() const { return averageMetrics; }
};

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <original.wav> <compared.wav> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Parameters:" << std::endl;
    std::cout << "  original.wav  - Original/reference WAV file" << std::endl;
    std::cout << "  compared.wav  - File to compare against original" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -v, --verbose    - Show detailed file information" << std::endl;
    std::cout << "  -s, --save       - Save comparison report to file" << std::endl;
    std::cout << "  -h, --help       - Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Output includes per-channel and average:" << std::endl;
    std::cout << "  • MSE (L2 norm) - Mean Squared Error" << std::endl;
    std::cout << "  • Max Abs Error (L∞ norm) - Maximum absolute difference" << std::endl;
    std::cout << "  • SNR - Signal-to-Noise Ratio in dB" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " original.wav quantized.wav" << std::endl;
    std::cout << "  " << programName << " sample.wav sample_8bit.wav -v -s" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string originalPath = argv[1];
    std::string comparedPath = argv[2];
    
    bool verbose = false;
    bool saveReport = false;
    
    // Parse options
    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg == "-s" || arg == "--save") {
            saveReport = true;
        } else if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
    }
    
    try {
        std::cout << "WAV Comparator - Audio File Comparison Tool" << std::endl;
        std::cout << "Original: " << originalPath << std::endl;
        std::cout << "Compared: " << comparedPath << std::endl;
        std::cout << "===========================================" << std::endl;
        
        WAVComparator comparator(originalPath, comparedPath);
        
        if (verbose) {
            comparator.printFileInfo();
        }
        
        comparator.calculateMetrics();
        comparator.printMetrics();
        
        if (saveReport) {
            std::string reportName = "comparison_" + 
                                   comparedPath.substr(comparedPath.find_last_of("/\\") + 1, 
                                                     comparedPath.find_last_of('.') - comparedPath.find_last_of("/\\") - 1) + 
                                   "_report.md";
            comparator.saveReport(reportName);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}