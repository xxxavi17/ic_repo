#ifndef ECHO_EFFECT_H
#define ECHO_EFFECT_H

#include "AudioEffect.h"
#include <vector>

/**
 * @brief Single echo effect implementation
 * 
 * Creates a single delayed copy of the input signal mixed with the original.
 * The delay time and feedback gain can be configured.
 * 
 * Mathematical formula:
 * y[n] = x[n] + feedback * x[n - delay]
 * 
 * Where:
 * - x[n] is the input signal
 * - y[n] is the output signal  
 * - delay is the delay in samples
 * - feedback is the echo gain (0.0 to 1.0)
 */
class EchoEffect : public AudioEffect {
public:
    /**
     * @brief Constructor
     * @param sampleRate Sample rate of the audio
     * @param channels Number of audio channels
     * @param delayTimeMs Delay time in milliseconds
     * @param feedback Feedback gain (0.0 to 1.0)
     */
    EchoEffect(int sampleRate, int channels, double delayTimeMs, double feedback);
    
    /**
     * @brief Process audio samples with echo effect
     */
    void process(const std::vector<double>& input, 
                std::vector<double>& output, 
                size_t numSamples) override;
    
    /**
     * @brief Reset the delay buffer
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
    double delayTimeMs_;        // Delay time in milliseconds
    double feedback_;           // Feedback gain
    size_t delaySamples_;       // Delay in samples
    std::vector<std::vector<double>> delayBuffer_;  // Circular buffer for each channel
    std::vector<size_t> writeIndex_;               // Write position for each channel
    
    /**
     * @brief Calculate delay in samples from milliseconds
     */
    size_t calculateDelaySamples(double delayTimeMs) const;
};

#endif // ECHO_EFFECT_H