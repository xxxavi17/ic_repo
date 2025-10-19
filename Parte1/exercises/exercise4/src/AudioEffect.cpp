#include "AudioEffect.h"
#include <stdexcept>

AudioEffect::AudioEffect(const std::string& name, int sampleRate, int channels) 
    : name_(name), sampleRate_(sampleRate), channels_(channels) {
    if (sampleRate <= 0) {
        throw std::invalid_argument("Sample rate must be positive");
    }
    if (channels <= 0) {
        throw std::invalid_argument("Number of channels must be positive");
    }
}