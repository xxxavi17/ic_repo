#ifndef REVERB_EFFECT_H
#define REVERB_EFFECT_H

#include "AudioEffect.h"
#include <vector>

/**
 * @brief Reverb effect using Schroeder algorithm
 * 
 * Implements artificial reverb using parallel comb filters followed by
 * series allpass filters, based on Manfred Schroeder's classic design.
 * 
 * Structure:
 * Input -> [Comb1, Comb2, Comb3, Comb4] -> Sum -> Allpass1 -> Allpass2 -> Output
 * 
 * Mathematical formulas:
 * Comb filter: y[n] = x[n] + g * y[n - M]
 * Allpass filter: y[n] = -g * x[n] + x[n - M] + g * y[n - M]
 * 
 * Where:
 * - M is the delay length
 * - g is the feedback/feedforward gain
 */
class ReverbEffect : public AudioEffect {
public:
    /**
     * @brief Constructor
     * @param sampleRate Sample rate of the audio
     * @param channels Number of audio channels
     * @param roomSize Room size parameter (0.0 to 1.0)
     * @param damping High frequency damping (0.0 to 1.0)
     * @param mix Dry/wet mix (0.0 = dry only, 1.0 = wet only)
     */
    ReverbEffect(int sampleRate, int channels, double roomSize, 
                double damping, double mix);
    
    /**
     * @brief Process audio samples with reverb effect
     */
    void process(const std::vector<double>& input, 
                std::vector<double>& output, 
                size_t numSamples) override;
    
    /**
     * @brief Reset all delay lines
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
    static const int NUM_COMBS = 4;      // Number of parallel comb filters
    static const int NUM_ALLPASS = 2;    // Number of series allpass filters
    
    struct CombFilter {
        std::vector<std::vector<double>> delayBuffer;  // Buffer for each channel
        std::vector<size_t> writeIndex;               // Write position for each channel
        std::vector<double> feedback;                 // Feedback state for each channel
        size_t delayLength;
        double gain;
        double damping;
    };
    
    struct AllpassFilter {
        std::vector<std::vector<double>> delayBuffer;  // Buffer for each channel
        std::vector<size_t> writeIndex;               // Write position for each channel
        size_t delayLength;
        double gain;
    };
    
    double roomSize_;
    double damping_;
    double mix_;
    
    std::vector<CombFilter> combFilters_;
    std::vector<AllpassFilter> allpassFilters_;
    
    // Predefined delay lengths (in samples at 44.1kHz)
    static const int COMB_DELAYS[NUM_COMBS];
    static const int ALLPASS_DELAYS[NUM_ALLPASS];
    
    /**
     * @brief Initialize comb filters
     */
    void initializeCombFilters();
    
    /**
     * @brief Initialize allpass filters
     */
    void initializeAllpassFilters();
    
    /**
     * @brief Scale delay length for current sample rate
     */
    size_t scaleDelayLength(int baseDelay) const;
};

#endif // REVERB_EFFECT_H