#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <iomanip>
#include <cstring>
#include <sndfile.h>
#include "AudioEffect.h"

/**
 * @brief WAV Effects Processor
 * 
 * A command-line program that applies various audio effects to WAV files.
 * Supports multiple effects including echo, reverb, chorus, amplitude modulation, etc.
 * 
 * Usage: wav_effects <input.wav> <output.wav> <effect> [parameters...]
 */
class WavEffectsProcessor {
public:
    /**
     * @brief Constructor
     */
    WavEffectsProcessor() = default;
    
    /**
     * @brief Process audio file with specified effect
     * @param inputFile Input WAV file path
     * @param outputFile Output WAV file path
     * @param effectName Name of the effect to apply
     * @param parameters Effect parameters
     * @return true if successful, false otherwise
     */
    bool processFile(const std::string& inputFile, 
                    const std::string& outputFile,
                    const std::string& effectName,
                    const std::vector<double>& parameters);

private:
    static const size_t BUFFER_SIZE = 4096;  // Process in blocks of 4096 samples
    
    /**
     * @brief Load WAV file
     * @param filename Path to WAV file
     * @param audioData Output vector for audio data
     * @param sfInfo Output SF_INFO structure
     * @return true if successful, false otherwise
     */
    bool loadWavFile(const std::string& filename, 
                    std::vector<double>& audioData, 
                    SF_INFO& sfInfo);
    
    /**
     * @brief Save WAV file
     * @param filename Path to output WAV file
     * @param audioData Audio data to save
     * @param sfInfo SF_INFO structure with file parameters
     * @return true if successful, false otherwise
     */
    bool saveWavFile(const std::string& filename, 
                    const std::vector<double>& audioData, 
                    const SF_INFO& sfInfo);
    
    /**
     * @brief Print file information
     */
    void printFileInfo(const SF_INFO& sfInfo);
    
    /**
     * @brief Print processing statistics
     */
    void printProcessingStats(const std::string& effectName,
                             const std::vector<double>& parameters,
                             size_t numSamples,
                             double processingTime);
};

bool WavEffectsProcessor::processFile(const std::string& inputFile, 
                                     const std::string& outputFile,
                                     const std::string& effectName,
                                     const std::vector<double>& parameters) {
    
    std::cout << "Loading input file: " << inputFile << std::endl;
    
    // Load input file
    std::vector<double> inputData;
    SF_INFO sfInfo;
    if (!loadWavFile(inputFile, inputData, sfInfo)) {
        std::cerr << "Error: Failed to load input file" << std::endl;
        return false;
    }
    
    printFileInfo(sfInfo);
    
    // Create effect
    std::unique_ptr<AudioEffect> effect;
    try {
        effect = AudioEffectFactory::createEffect(effectName, sfInfo.samplerate, sfInfo.channels, parameters);
        std::cout << "Created effect: " << effect->getName() << std::endl;
        std::cout << "Parameters: " << effect->getParameters() << std::endl;
        std::cout << "Description: " << effect->getDescription() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error creating effect: " << e.what() << std::endl;
        return false;
    }
    
    // Process audio
    std::cout << "Processing audio..." << std::endl;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<double> outputData;
    size_t totalSamples = inputData.size() / sfInfo.channels;
    size_t processedSamples = 0;
    
    while (processedSamples < totalSamples) {
        // Calculate samples to process in this block
        size_t samplesToProcess = std::min(BUFFER_SIZE, totalSamples - processedSamples);
        
        // Extract input block
        size_t startIndex = processedSamples * sfInfo.channels;
        size_t endIndex = startIndex + samplesToProcess * sfInfo.channels;
        std::vector<double> inputBlock(inputData.begin() + startIndex, 
                                      inputData.begin() + endIndex);
        
        // Process block
        std::vector<double> outputBlock;
        effect->process(inputBlock, outputBlock, samplesToProcess);
        
        // Append to output
        outputData.insert(outputData.end(), outputBlock.begin(), outputBlock.end());
        
        processedSamples += samplesToProcess;
        
        // Progress indicator
        if (processedSamples % (BUFFER_SIZE * 10) == 0 || processedSamples == totalSamples) {
            double progress = static_cast<double>(processedSamples) / totalSamples * 100.0;
            std::cout << "\rProgress: " << std::fixed << std::setprecision(1) 
                     << progress << "%" << std::flush;
        }
    }
    std::cout << std::endl;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    double processingTime = duration.count() / 1000.0;
    
    // Save output file
    std::cout << "Saving output file: " << outputFile << std::endl;
    if (!saveWavFile(outputFile, outputData, sfInfo)) {
        std::cerr << "Error: Failed to save output file" << std::endl;
        return false;
    }
    
    printProcessingStats(effectName, parameters, totalSamples, processingTime);
    
    std::cout << "Processing completed successfully!" << std::endl;
    return true;
}

bool WavEffectsProcessor::loadWavFile(const std::string& filename, 
                                     std::vector<double>& audioData, 
                                     SF_INFO& sfInfo) {
    // Initialize SF_INFO
    std::memset(&sfInfo, 0, sizeof(SF_INFO));
    
    // Open file
    SNDFILE* sndfile = sf_open(filename.c_str(), SFM_READ, &sfInfo);
    if (!sndfile) {
        std::cerr << "Error opening file: " << sf_strerror(nullptr) << std::endl;
        return false;
    }
    
    // Read audio data
    sf_count_t totalFrames = sfInfo.frames * sfInfo.channels;
    audioData.resize(totalFrames);
    
    sf_count_t readFrames = sf_read_double(sndfile, audioData.data(), totalFrames);
    if (readFrames != totalFrames) {
        std::cerr << "Warning: Expected " << totalFrames << " samples, read " << readFrames << std::endl;
        audioData.resize(readFrames);
    }
    
    sf_close(sndfile);
    return true;
}

bool WavEffectsProcessor::saveWavFile(const std::string& filename, 
                                     const std::vector<double>& audioData, 
                                     const SF_INFO& sfInfo) {
    // Create output file info
    SF_INFO outputInfo = sfInfo;
    outputInfo.frames = audioData.size() / sfInfo.channels;
    
    // Open output file
    SNDFILE* sndfile = sf_open(filename.c_str(), SFM_WRITE, &outputInfo);
    if (!sndfile) {
        std::cerr << "Error creating output file: " << sf_strerror(nullptr) << std::endl;
        return false;
    }
    
    // Write audio data
    sf_count_t totalSamples = audioData.size();
    sf_count_t writtenSamples = sf_write_double(sndfile, audioData.data(), totalSamples);
    
    if (writtenSamples != totalSamples) {
        std::cerr << "Warning: Expected to write " << totalSamples 
                 << " samples, wrote " << writtenSamples << std::endl;
    }
    
    sf_close(sndfile);
    return true;
}

void WavEffectsProcessor::printFileInfo(const SF_INFO& sfInfo) {
    std::cout << "File information:" << std::endl;
    std::cout << "  Sample rate: " << sfInfo.samplerate << " Hz" << std::endl;
    std::cout << "  Channels: " << sfInfo.channels << std::endl;
    std::cout << "  Frames: " << sfInfo.frames << std::endl;
    std::cout << "  Duration: " << std::fixed << std::setprecision(2) 
             << static_cast<double>(sfInfo.frames) / sfInfo.samplerate << " seconds" << std::endl;
    
    // Format information
    std::string format;
    switch (sfInfo.format & SF_FORMAT_TYPEMASK) {
        case SF_FORMAT_WAV: format = "WAV"; break;
        case SF_FORMAT_AIFF: format = "AIFF"; break;
        case SF_FORMAT_FLAC: format = "FLAC"; break;
        default: format = "Unknown"; break;
    }
    std::cout << "  Format: " << format << std::endl;
}

void WavEffectsProcessor::printProcessingStats(const std::string& effectName,
                                              const std::vector<double>& parameters,
                                              size_t numSamples,
                                              double processingTime) {
    std::cout << "\nProcessing statistics:" << std::endl;
    std::cout << "  Effect: " << effectName << std::endl;
    std::cout << "  Parameters: ";
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << parameters[i];
    }
    std::cout << std::endl;
    std::cout << "  Samples processed: " << numSamples << std::endl;
    std::cout << "  Processing time: " << std::fixed << std::setprecision(3) 
             << processingTime << " seconds" << std::endl;
    
    if (processingTime > 0) {
        double realTimeRatio = (static_cast<double>(numSamples) / 44100.0) / processingTime;
        std::cout << "  Real-time performance: " << std::fixed << std::setprecision(1) 
                 << realTimeRatio << "x" << std::endl;
    }
}

// Command-line interface functions
void printUsage(const char* programName) {
    std::cout << "WAV Effects Processor" << std::endl;
    std::cout << "Usage: " << programName << " <input.wav> <output.wav> <effect> [parameters...]" << std::endl;
    std::cout << std::endl;
    std::cout << "Available effects:" << std::endl;
    
    auto effects = AudioEffectFactory::getAvailableEffects();
    for (const auto& effect : effects) {
        std::cout << "  " << AudioEffectFactory::getEffectUsage(effect) << std::endl << std::endl;
    }
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " input.wav output.wav echo 300 0.6" << std::endl;
    std::cout << "  " << programName << " input.wav output.wav reverb 0.8 0.3 0.4" << std::endl;
    std::cout << "  " << programName << " input.wav output.wav chorus 15 1.5 0.7 0.2 0.5" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "WAV Effects Processor v1.0" << std::endl;
    std::cout << "========================================" << std::endl;
    
    if (argc < 4) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    std::string effectName = argv[3];
    
    // Parse effect parameters
    std::vector<double> parameters;
    for (int i = 4; i < argc; ++i) {
        try {
            parameters.push_back(std::stod(argv[i]));
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid parameter '" << argv[i] << "'" << std::endl;
            return 1;
        }
    }
    
    // Process file
    WavEffectsProcessor processor;
    bool success = processor.processFile(inputFile, outputFile, effectName, parameters);
    
    return success ? 0 : 1;
}