#ifndef CHORUS_EFFECT_H
#define CHORUS_EFFECT_H

#include "AudioEffect.h"
#include <vector>
#include <cmath>

/**
 * @brief Chorus/Flanger effect with time-varying delay
 * 
 * Creates a chorus or flanger effect by modulating the delay time with an LFO.
 * The delay oscillates around a base delay time, creating pitch modulation.
 * 
 * Mathematical formula:
 * delay[n] = baseDelay + depth * sin(2π * f_lfo * n / fs)
 * y[n] = x[n] + feedback * x[n - delay[n]]
 * 
 * Where:
 * - x[n] is the input signal
 * - y[n] is the output signal
 * - baseDelay is the base delay in samples
 * - depth is the modulation depth in samples
 * - f_lfo is the LFO frequency in Hz
 */
class ChorusEffect : public AudioEffect {
public:
    /**
     * @brief Constructor
     * @param sampleRate Sample rate of the audio
     * @param channels Number of audio channels
     * @param baseDelayMs Base delay time in milliseconds
     * @param modulationFreq LFO frequency in Hz
     * @param modulationDepth Modulation depth (0.0 to 1.0)
     * @param feedback Feedback amount (0.0 to 0.99)
     * @param mix Dry/wet mix (0.0 = dry only, 1.0 = wet only)
     */
    ChorusEffect(int sampleRate, int channels, double baseDelayMs, 
                double modulationFreq, double modulationDepth, 
                double feedback, double mix);
    
    /**
     * @brief Process audio samples with chorus effect
     */
    void process(const std::vector<double>& input, 
                std::vector<double>& output, 
                size_t numSamples) override;
    
    /**
     * @brief Reset delay buffers and oscillator
     */
    void reset() override;
    
    /**
     * @brief Get effect description
     */
    std::string getDescription() const override;
    
    /**
     * @brief Get effect parameters
     */
    std::string getParameters() const override;

private:
    double baseDelayMs_;        // Base delay in milliseconds
    double modulationFreq_;     // LFO frequency in Hz
    double modulationDepth_;    // Modulation depth (0.0 to 1.0)
    double feedback_;           // Feedback amount
    double mix_;                // Dry/wet mix
    
    size_t baseDelaySamples_;   // Base delay in samples
    size_t maxDelaySamples_;    // Maximum delay (for buffer size)
    double modulationDepthSamples_; // Modulation depth in samples
    
    std::vector<std::vector<double>> delayBuffer_;  // Delay buffer for each channel
    std::vector<size_t> writeIndex_;               // Write position for each channel
    
    double lfoPhase_;           // LFO phase
    double lfoPhaseIncrement_;  // LFO phase increment per sample
    
    /**
     * @brief Get interpolated sample from delay buffer using fractional delay
     * @param channel Channel index
     * @param fractionalDelay Delay in samples (can be fractional)
     * @return Interpolated sample
     */
    double getInterpolatedSample(int channel, double fractionalDelay);
    
    /**
     * @brief Calculate delay in samples from milliseconds
     */
    size_t calculateDelaySamples(double delayTimeMs) const;
};

#endif // CHORUS_EFFECT_H