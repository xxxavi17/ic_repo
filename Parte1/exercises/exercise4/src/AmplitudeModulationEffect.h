#ifndef AMPLITUDE_MODULATION_EFFECT_H
#define AMPLITUDE_MODULATION_EFFECT_H

#include "AudioEffect.h"
#include <cmath>

/**
 * @brief Amplitude modulation effect implementation
 * 
 * Modulates the amplitude of the input signal using a low-frequency oscillator (LFO).
 * Creates tremolo, ring modulation, or amplitude modulation effects.
 * 
 * Mathematical formula:
 * y[n] = x[n] * (1 + depth * sin(2π * f_mod * n / fs))
 * 
 * Where:
 * - x[n] is the input signal
 * - y[n] is the output signal
 * - depth is the modulation depth (0.0 to 1.0)
 * - f_mod is the modulation frequency in Hz
 * - fs is the sample rate
 * - n is the sample index
 */
class AmplitudeModulationEffect : public AudioEffect {
public:
    /**
     * @brief Constructor
     * @param sampleRate Sample rate of the audio
     * @param channels Number of audio channels
     * @param modulationFreq Modulation frequency in Hz
     * @param depth Modulation depth (0.0 to 1.0)
     * @param waveform Waveform type (0=sine, 1=triangle, 2=square)
     */
    AmplitudeModulationEffect(int sampleRate, int channels, 
                             double modulationFreq, double depth, int waveform = 0);
    
    /**
     * @brief Process audio samples with amplitude modulation
     */
    void process(const std::vector<double>& input, 
                std::vector<double>& output, 
                size_t numSamples) override;
    
    /**
     * @brief Reset the oscillator phase
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
    double modulationFreq_;     // Modulation frequency in Hz
    double depth_;              // Modulation depth (0.0 to 1.0)
    int waveform_;              // Waveform type
    double phase_;              // Current oscillator phase
    double phaseIncrement_;     // Phase increment per sample
    
    /**
     * @brief Generate oscillator sample based on waveform type
     * @param phase Current phase (0.0 to 2π)
     * @return Oscillator sample (-1.0 to 1.0)
     */
    double generateOscillator(double phase) const;
    
    /**
     * @brief Get waveform name as string
     */
    std::string getWaveformName() const;
};

#endif // AMPLITUDE_MODULATION_EFFECT_H