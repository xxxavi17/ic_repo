#include "ChorusEffect.h"
#include <algorithm>
#include <sstream>

ChorusEffect::ChorusEffect(int sampleRate, int channels, double baseDelayMs, 
                          double modulationFreq, double modulationDepth, 
                          double feedback, double mix)
    : AudioEffect("Chorus", sampleRate, channels),
      baseDelayMs_(std::max(1.0, baseDelayMs)),
      modulationFreq_(std::max(0.1, modulationFreq)),
      modulationDepth_(std::clamp(modulationDepth, 0.0, 1.0)),
      feedback_(std::clamp(feedback, 0.0, 0.99)),
      mix_(std::clamp(mix, 0.0, 1.0)),
      lfoPhase_(0.0) {
    
    baseDelaySamples_ = calculateDelaySamples(baseDelayMs_);
    
    // Calculate modulation depth in samples (typically 1-5ms modulation)
    modulationDepthSamples_ = modulationDepth_ * baseDelaySamples_ * 0.5;
    
    // Maximum delay needed for the buffer
    maxDelaySamples_ = baseDelaySamples_ + static_cast<size_t>(modulationDepthSamples_) + 1;
    
    // Initialize delay buffers for each channel
    delayBuffer_.resize(channels_);
    writeIndex_.resize(channels_, 0);
    
    for (int ch = 0; ch < channels_; ++ch) {
        delayBuffer_[ch].resize(maxDelaySamples_, 0.0);
    }
    
    // Calculate LFO phase increment
    lfoPhaseIncrement_ = 2.0 * M_PI * modulationFreq_ / sampleRate_;
}

void ChorusEffect::process(const std::vector<double>& input, 
                          std::vector<double>& output, 
                          size_t numSamples) {
    output.resize(input.size());
    
    for (size_t i = 0; i < numSamples; ++i) {
        // Generate LFO modulation
        double lfoValue = std::sin(lfoPhase_);
        
        // Calculate time-varying delay
        double currentDelay = baseDelaySamples_ + modulationDepthSamples_ * lfoValue;
        
        for (int ch = 0; ch < channels_; ++ch) {
            size_t inputIndex = i * channels_ + ch;
            double inputSample = input[inputIndex];
            
            // Get modulated delayed sample using interpolation
            double delayedSample = getInterpolatedSample(ch, currentDelay);
            
            // Apply feedback
            double feedbackSample = inputSample + feedback_ * delayedSample;
            
            // Write to delay buffer
            delayBuffer_[ch][writeIndex_[ch]] = feedbackSample;
            
            // Calculate output with dry/wet mix
            double wetSignal = delayedSample;
            double drySignal = inputSample;
            output[inputIndex] = (1.0 - mix_) * drySignal + mix_ * wetSignal;
            
            // Advance write index (circular)
            writeIndex_[ch] = (writeIndex_[ch] + 1) % maxDelaySamples_;
        }
        
        // Advance LFO phase
        lfoPhase_ += lfoPhaseIncrement_;
        if (lfoPhase_ >= 2.0 * M_PI) {
            lfoPhase_ -= 2.0 * M_PI;
        }
    }
}

void ChorusEffect::reset() {
    // Clear delay buffers
    for (int ch = 0; ch < channels_; ++ch) {
        std::fill(delayBuffer_[ch].begin(), delayBuffer_[ch].end(), 0.0);
        writeIndex_[ch] = 0;
    }
    
    // Reset LFO
    lfoPhase_ = 0.0;
}

std::string ChorusEffect::getDescription() const {
    return "Chorus effect with time-varying delay modulation";
}

std::string ChorusEffect::getParameters() const {
    std::ostringstream oss;
    oss << "Base delay: " << baseDelayMs_ << "ms, "
        << "LFO frequency: " << modulationFreq_ << "Hz, "
        << "Modulation depth: " << modulationDepth_ << ", "
        << "Feedback: " << feedback_ << ", "
        << "Mix: " << mix_;
    return oss.str();
}

double ChorusEffect::getInterpolatedSample(int channel, double fractionalDelay) {
    // Calculate read position (fractional)
    double readPos = writeIndex_[channel] - fractionalDelay;
    
    // Handle negative indices (wrap around)
    while (readPos < 0) {
        readPos += maxDelaySamples_;
    }
    
    // Get integer and fractional parts
    size_t readIndex1 = static_cast<size_t>(readPos) % maxDelaySamples_;
    size_t readIndex2 = (readIndex1 + 1) % maxDelaySamples_;
    double fraction = readPos - std::floor(readPos);
    
    // Linear interpolation between two samples
    double sample1 = delayBuffer_[channel][readIndex1];
    double sample2 = delayBuffer_[channel][readIndex2];
    
    return sample1 + fraction * (sample2 - sample1);
}

size_t ChorusEffect::calculateDelaySamples(double delayTimeMs) const {
    size_t samples = static_cast<size_t>(std::round(delayTimeMs * sampleRate_ / 1000.0));
    return std::max(samples, size_t(1)); // Minimum 1 sample delay
}