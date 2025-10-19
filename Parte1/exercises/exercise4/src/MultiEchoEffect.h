#ifndef MULTI_ECHO_EFFECT_H
#define MULTI_ECHO_EFFECT_H

#include "AudioEffect.h"
#include <vector>

/**
 * @brief Multiple echo effect implementation
 * 
 * Creates multiple delayed copies of the input signal with decreasing amplitude.
 * Each echo has its own delay time and feedback gain.
 * 
 * Mathematical formula:
 * y[n] = x[n] + Σ(feedback_i * x[n - delay_i]) for i = 1 to numEchoes
 * 
 * Where:
 * - x[n] is the input signal
 * - y[n] is the output signal
 * - delay_i is the delay for echo i
 * - feedback_i is the feedback gain for echo i
 */
class MultiEchoEffect : public AudioEffect {
public:
    /**
     * @brief Constructor
     * @param sampleRate Sample rate of the audio
     * @param channels Number of audio channels
     * @param baseDelayMs Base delay time in milliseconds
     * @param numEchoes Number of echoes
     * @param feedbackDecay Feedback decay factor between echoes
     */
    MultiEchoEffect(int sampleRate, int channels, double baseDelayMs, 
                   int numEchoes, double feedbackDecay);
    
    /**
     * @brief Process audio samples with multi-echo effect
     */
    void process(const std::vector<double>& input, 
                std::vector<double>& output, 
                size_t numSamples) override;
    
    /**
     * @brief Reset all delay buffers
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
    struct EchoTap {
        size_t delaySamples;
        double feedback;
        std::vector<std::vector<double>> delayBuffer;  // Buffer for each channel
        std::vector<size_t> writeIndex;               // Write position for each channel
    };
    
    double baseDelayMs_;
    int numEchoes_;
    double feedbackDecay_;
    std::vector<EchoTap> echoTaps_;
    
    /**
     * @brief Initialize echo taps with calculated delays and feedback values
     */
    void initializeEchoTaps();
    
    /**
     * @brief Calculate delay in samples from milliseconds
     */
    size_t calculateDelaySamples(double delayTimeMs) const;
};

#endif // MULTI_ECHO_EFFECT_H