#include "AmplitudeModulationEffect.h"
#include <algorithm>
#include <sstream>

AmplitudeModulationEffect::AmplitudeModulationEffect(int sampleRate, int channels, 
                                                   double modulationFreq, double depth, int waveform)
    : AudioEffect("Amplitude Modulation", sampleRate, channels),
      modulationFreq_(std::max(0.1, modulationFreq)),
      depth_(std::clamp(depth, 0.0, 1.0)),
      waveform_(std::clamp(waveform, 0, 2)),
      phase_(0.0) {
    
    // Calculate phase increment per sample
    phaseIncrement_ = 2.0 * M_PI * modulationFreq_ / sampleRate_;
}

void AmplitudeModulationEffect::process(const std::vector<double>& input, 
                                       std::vector<double>& output, 
                                       size_t numSamples) {
    output.resize(input.size());
    
    for (size_t i = 0; i < numSamples; ++i) {
        // Generate modulation signal
        double modulation = generateOscillator(phase_);
        
        // Calculate amplitude multiplier: 1 + depth * modulation
        double amplitudeMultiplier = 1.0 + depth_ * modulation;
        
        // Apply amplitude modulation to all channels
        for (int ch = 0; ch < channels_; ++ch) {
            size_t index = i * channels_ + ch;
            output[index] = input[index] * amplitudeMultiplier;
        }
        
        // Advance oscillator phase
        phase_ += phaseIncrement_;
        
        // Wrap phase to [0, 2π]
        if (phase_ >= 2.0 * M_PI) {
            phase_ -= 2.0 * M_PI;
        }
    }
}

void AmplitudeModulationEffect::reset() {
    phase_ = 0.0;
}

std::string AmplitudeModulationEffect::getDescription() const {
    return "Amplitude modulation effect using low-frequency oscillator";
}

std::string AmplitudeModulationEffect::getParameters() const {
    std::ostringstream oss;
    oss << "Modulation frequency: " << modulationFreq_ << " Hz, "
        << "Depth: " << depth_ << ", "
        << "Waveform: " << getWaveformName();
    return oss.str();
}

double AmplitudeModulationEffect::generateOscillator(double phase) const {
    switch (waveform_) {
        case 0: // Sine wave
            return std::sin(phase);
            
        case 1: // Triangle wave
            {
                double normalizedPhase = phase / (2.0 * M_PI);
                if (normalizedPhase < 0.5) {
                    return 4.0 * normalizedPhase - 1.0;  // Rising edge
                } else {
                    return 3.0 - 4.0 * normalizedPhase;  // Falling edge
                }
            }
            
        case 2: // Square wave
            return (phase < M_PI) ? 1.0 : -1.0;
            
        default:
            return std::sin(phase);
    }
}

std::string AmplitudeModulationEffect::getWaveformName() const {
    switch (waveform_) {
        case 0: return "Sine";
        case 1: return "Triangle";
        case 2: return "Square";
        default: return "Unknown";
    }
}