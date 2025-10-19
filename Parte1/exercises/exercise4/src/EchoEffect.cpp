#include "EchoEffect.h"
#include <algorithm>
#include <sstream>
#include <cmath>

EchoEffect::EchoEffect(int sampleRate, int channels, double delayTimeMs, double feedback)
    : AudioEffect("Echo", sampleRate, channels), 
      delayTimeMs_(delayTimeMs), 
      feedback_(std::clamp(feedback, 0.0, 0.99)) {
    
    delaySamples_ = calculateDelaySamples(delayTimeMs_);
    
    // Initialize delay buffers for each channel
    delayBuffer_.resize(channels_);
    writeIndex_.resize(channels_, 0);
    
    for (int ch = 0; ch < channels_; ++ch) {
        delayBuffer_[ch].resize(delaySamples_, 0.0);
    }
}

void EchoEffect::process(const std::vector<double>& input, 
                        std::vector<double>& output, 
                        size_t numSamples) {
    output.resize(input.size());
    
    for (size_t i = 0; i < numSamples; ++i) {
        for (int ch = 0; ch < channels_; ++ch) {
            size_t inputIndex = i * channels_ + ch;
            double inputSample = input[inputIndex];
            
            // Read from delay buffer (circular buffer)
            size_t readIndex = writeIndex_[ch];
            double delayedSample = delayBuffer_[ch][readIndex];
            
            // Apply echo effect: y[n] = x[n] + feedback * x[n - delay]
            output[inputIndex] = inputSample + feedback_ * delayedSample;
            
            // Write input to delay buffer
            delayBuffer_[ch][writeIndex_[ch]] = inputSample;
            
            // Advance write index (circular)
            writeIndex_[ch] = (writeIndex_[ch] + 1) % delaySamples_;
        }
    }
}

void EchoEffect::reset() {
    // Clear delay buffers
    for (int ch = 0; ch < channels_; ++ch) {
        std::fill(delayBuffer_[ch].begin(), delayBuffer_[ch].end(), 0.0);
        writeIndex_[ch] = 0;
    }
}

std::string EchoEffect::getDescription() const {
    return "Single echo effect with configurable delay time and feedback gain";
}

std::string EchoEffect::getParameters() const {
    std::ostringstream oss;
    oss << "Delay: " << delayTimeMs_ << "ms, "
        << "Feedback: " << feedback_ << ", "
        << "Delay samples: " << delaySamples_;
    return oss.str();
}

size_t EchoEffect::calculateDelaySamples(double delayTimeMs) const {
    size_t samples = static_cast<size_t>(std::round(delayTimeMs * sampleRate_ / 1000.0));
    return std::max(samples, size_t(1)); // Minimum 1 sample delay
}