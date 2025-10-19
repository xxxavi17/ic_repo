#include "ReverbEffect.h"
#include <algorithm>
#include <sstream>
#include <cmath>

// Schroeder reverb delay lengths (samples at 44.1kHz)
const int ReverbEffect::COMB_DELAYS[NUM_COMBS] = {1116, 1188, 1277, 1356};
const int ReverbEffect::ALLPASS_DELAYS[NUM_ALLPASS] = {556, 441};

ReverbEffect::ReverbEffect(int sampleRate, int channels, double roomSize, 
                          double damping, double mix)
    : AudioEffect("Reverb", sampleRate, channels),
      roomSize_(std::clamp(roomSize, 0.0, 1.0)),
      damping_(std::clamp(damping, 0.0, 1.0)),
      mix_(std::clamp(mix, 0.0, 1.0)) {
    
    initializeCombFilters();
    initializeAllpassFilters();
}

void ReverbEffect::process(const std::vector<double>& input, 
                          std::vector<double>& output, 
                          size_t numSamples) {
    output.resize(input.size());
    
    for (size_t i = 0; i < numSamples; ++i) {
        for (int ch = 0; ch < channels_; ++ch) {
            size_t inputIndex = i * channels_ + ch;
            double inputSample = input[inputIndex];
            
            // Process parallel comb filters
            double combSum = 0.0;
            for (auto& comb : combFilters_) {
                // Read from delay buffer
                size_t readIndex = comb.writeIndex[ch];
                double delayedSample = comb.delayBuffer[ch][readIndex];
                
                // Apply damping filter (simple lowpass)
                comb.feedback[ch] = delayedSample + damping_ * (comb.feedback[ch] - delayedSample);
                
                // Comb filter: y[n] = x[n] + g * y[n - M]
                double combOutput = inputSample + comb.gain * comb.feedback[ch];
                
                // Write to delay buffer
                comb.delayBuffer[ch][comb.writeIndex[ch]] = combOutput;
                
                // Advance write index
                comb.writeIndex[ch] = (comb.writeIndex[ch] + 1) % comb.delayLength;
                
                combSum += combOutput;
            }
            
            // Average the comb filter outputs
            double reverbSignal = combSum / NUM_COMBS;
            
            // Process series allpass filters
            for (auto& allpass : allpassFilters_) {
                // Read from delay buffer
                size_t readIndex = allpass.writeIndex[ch];
                double delayedSample = allpass.delayBuffer[ch][readIndex];
                
                // Allpass filter: y[n] = -g * x[n] + x[n - M] + g * y[n - M]
                double allpassOutput = -allpass.gain * reverbSignal + delayedSample;
                
                // Write input to delay buffer
                allpass.delayBuffer[ch][allpass.writeIndex[ch]] = reverbSignal + allpass.gain * allpassOutput;
                
                // Advance write index
                allpass.writeIndex[ch] = (allpass.writeIndex[ch] + 1) % allpass.delayLength;
                
                reverbSignal = allpassOutput;
            }
            
            // Mix dry and wet signals
            output[inputIndex] = (1.0 - mix_) * inputSample + mix_ * reverbSignal;
        }
    }
}

void ReverbEffect::reset() {
    // Clear comb filters
    for (auto& comb : combFilters_) {
        for (int ch = 0; ch < channels_; ++ch) {
            std::fill(comb.delayBuffer[ch].begin(), comb.delayBuffer[ch].end(), 0.0);
            comb.writeIndex[ch] = 0;
            comb.feedback[ch] = 0.0;
        }
    }
    
    // Clear allpass filters
    for (auto& allpass : allpassFilters_) {
        for (int ch = 0; ch < channels_; ++ch) {
            std::fill(allpass.delayBuffer[ch].begin(), allpass.delayBuffer[ch].end(), 0.0);
            allpass.writeIndex[ch] = 0;
        }
    }
}

std::string ReverbEffect::getDescription() const {
    return "Schroeder reverb with parallel comb filters and series allpass filters";
}

std::string ReverbEffect::getParameters() const {
    std::ostringstream oss;
    oss << "Room size: " << roomSize_ << ", "
        << "Damping: " << damping_ << ", "
        << "Mix: " << mix_;
    return oss.str();
}

void ReverbEffect::initializeCombFilters() {
    combFilters_.clear();
    combFilters_.reserve(NUM_COMBS);
    
    for (int i = 0; i < NUM_COMBS; ++i) {
        CombFilter comb;
        
        // Scale delay length for current sample rate
        comb.delayLength = scaleDelayLength(COMB_DELAYS[i]);
        
        // Calculate gain based on room size (larger room = more feedback)
        comb.gain = 0.5 + 0.3 * roomSize_;
        
        // Initialize damping
        comb.damping = damping_;
        
        // Initialize buffers for each channel
        comb.delayBuffer.resize(channels_);
        comb.writeIndex.resize(channels_, 0);
        comb.feedback.resize(channels_, 0.0);
        
        for (int ch = 0; ch < channels_; ++ch) {
            comb.delayBuffer[ch].resize(comb.delayLength, 0.0);
        }
        
        combFilters_.push_back(std::move(comb));
    }
}

void ReverbEffect::initializeAllpassFilters() {
    allpassFilters_.clear();
    allpassFilters_.reserve(NUM_ALLPASS);
    
    for (int i = 0; i < NUM_ALLPASS; ++i) {
        AllpassFilter allpass;
        
        // Scale delay length for current sample rate
        allpass.delayLength = scaleDelayLength(ALLPASS_DELAYS[i]);
        
        // Standard allpass gain
        allpass.gain = 0.7;
        
        // Initialize buffers for each channel
        allpass.delayBuffer.resize(channels_);
        allpass.writeIndex.resize(channels_, 0);
        
        for (int ch = 0; ch < channels_; ++ch) {
            allpass.delayBuffer[ch].resize(allpass.delayLength, 0.0);
        }
        
        allpassFilters_.push_back(std::move(allpass));
    }
}

size_t ReverbEffect::scaleDelayLength(int baseDelay) const {
    // Scale from 44.1kHz to current sample rate
    double scaleFactor = static_cast<double>(sampleRate_) / 44100.0;
    size_t scaledDelay = static_cast<size_t>(std::round(baseDelay * scaleFactor));
    return std::max(scaledDelay, size_t(1)); // Minimum 1 sample
}