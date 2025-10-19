#ifndef AUDIO_EFFECT_H
#define AUDIO_EFFECT_H

#include <vector>
#include <string>
#include <memory>

/**
 * @brief Base class for all audio effects
 * 
 * This abstract class defines the interface for audio effects processing.
 * All specific effects inherit from this class and implement the process method.
 */
class AudioEffect {
public:
    /**
     * @brief Constructor
     * @param name Name of the effect
     * @param sampleRate Sample rate of the audio
     * @param channels Number of audio channels
     */
    AudioEffect(const std::string& name, int sampleRate, int channels);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~AudioEffect() = default;
    
    /**
     * @brief Process audio samples with the effect
     * @param input Input audio samples (interleaved)
     * @param output Output audio samples (interleaved)
     * @param numSamples Number of samples per channel
     */
    virtual void process(const std::vector<double>& input, 
                        std::vector<double>& output, 
                        size_t numSamples) = 0;
    
    /**
     * @brief Reset the effect state
     */
    virtual void reset() = 0;
    
    /**
     * @brief Get effect name
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief Get effect description
     */
    virtual std::string getDescription() const = 0;
    
    /**
     * @brief Get effect parameters as string
     */
    virtual std::string getParameters() const = 0;

protected:
    std::string name_;
    int sampleRate_;
    int channels_;
};

/**
 * @brief Factory class for creating audio effects
 */
class AudioEffectFactory {
public:
    /**
     * @brief Create an audio effect by name
     * @param effectName Name of the effect to create
     * @param sampleRate Sample rate
     * @param channels Number of channels
     * @param parameters Effect-specific parameters
     * @return Unique pointer to the created effect
     */
    static std::unique_ptr<AudioEffect> createEffect(
        const std::string& effectName,
        int sampleRate,
        int channels,
        const std::vector<double>& parameters = {}
    );
    
    /**
     * @brief Get list of available effects
     */
    static std::vector<std::string> getAvailableEffects();
    
    /**
     * @brief Get effect usage information
     */
    static std::string getEffectUsage(const std::string& effectName);
};

#endif // AUDIO_EFFECT_H