#include "MultiEchoEffect.h"
#include <algorithm>
#include <sstream>
#include <cmath>

MultiEchoEffect::MultiEchoEffect(int sampleRate, int channels, double baseDelayMs, 
                                int numEchoes, double feedbackDecay)
    : AudioEffect("Multi-Echo", sampleRate, channels),
      baseDelayMs_(baseDelayMs),
      numEchoes_(std::max(1, numEchoes)),
      feedbackDecay_(std::clamp(feedbackDecay, 0.1, 0.9)) {
    
    initializeEchoTaps();
}

void MultiEchoEffect::process(const std::vector<double>& input, 
                             std::vector<double>& output, 
                             size_t numSamples) {
    output.resize(input.size());
    
    for (size_t i = 0; i < numSamples; ++i) {
        for (int ch = 0; ch < channels_; ++ch) {
            size_t inputIndex = i * channels_ + ch;
            double inputSample = input[inputIndex];
            double outputSample = inputSample;
            
            // Process each echo tap
            for (auto& tap : echoTaps_) {
                // Read from delay buffer (circular buffer)
                size_t readIndex = tap.writeIndex[ch];
                double delayedSample = tap.delayBuffer[ch][readIndex];
                
                // Add echo contribution
                outputSample += tap.feedback * delayedSample;
                
                // Write input to delay buffer
                tap.delayBuffer[ch][tap.writeIndex[ch]] = inputSample;
                
                // Advance write index (circular)
                tap.writeIndex[ch] = (tap.writeIndex[ch] + 1) % tap.delaySamples;
            }
            
            output[inputIndex] = outputSample;
        }
    }
}

void MultiEchoEffect::reset() {
    for (auto& tap : echoTaps_) {
        for (int ch = 0; ch < channels_; ++ch) {
            std::fill(tap.delayBuffer[ch].begin(), tap.delayBuffer[ch].end(), 0.0);
            tap.writeIndex[ch] = 0;
        }
    }
}

std::string MultiEchoEffect::getDescription() const {
    return "Multiple echo effect with exponentially decaying feedback";
}

std::string MultiEchoEffect::getParameters() const {
    std::ostringstream oss;
    oss << "Base delay: " << baseDelayMs_ << "ms, "
        << "Number of echoes: " << numEchoes_ << ", "
        << "Feedback decay: " << feedbackDecay_;
    
    oss << "\nEcho taps: ";
    for (size_t i = 0; i < echoTaps_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "[" << (echoTaps_[i].delaySamples * 1000.0 / sampleRate_) << "ms, "
            << echoTaps_[i].feedback << "]";
    }
    
    return oss.str();
}

void MultiEchoEffect::initializeEchoTaps() {
    echoTaps_.clear();
    echoTaps_.reserve(numEchoes_);
    
    for (int i = 0; i < numEchoes_; ++i) {
        EchoTap tap;
        
        // Calculate delay: each echo is spaced by base delay
        double delayMs = baseDelayMs_ * (i + 1);
        tap.delaySamples = calculateDelaySamples(delayMs);
        
        // Calculate feedback: exponentially decaying
        tap.feedback = std::pow(feedbackDecay_, i + 1);
        
        // Initialize delay buffers for each channel
        tap.delayBuffer.resize(channels_);
        tap.writeIndex.resize(channels_, 0);
        
        for (int ch = 0; ch < channels_; ++ch) {
            tap.delayBuffer[ch].resize(tap.delaySamples, 0.0);
        }
        
        echoTaps_.push_back(std::move(tap));
    }
}

size_t MultiEchoEffect::calculateDelaySamples(double delayTimeMs) const {
    size_t samples = static_cast<size_t>(std::round(delayTimeMs * sampleRate_ / 1000.0));
    return std::max(samples, size_t(1)); // Minimum 1 sample delay
}