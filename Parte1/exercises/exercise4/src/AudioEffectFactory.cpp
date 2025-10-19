#include "AudioEffect.h"
#include "EchoEffect.h"
#include "MultiEchoEffect.h"
#include "AmplitudeModulationEffect.h"
#include "ChorusEffect.h"
#include "ReverbEffect.h"
#include <stdexcept>
#include <algorithm>

std::unique_ptr<AudioEffect> AudioEffectFactory::createEffect(
    const std::string& effectName,
    int sampleRate,
    int channels,
    const std::vector<double>& parameters) {
    
    std::string lowerName = effectName;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    if (lowerName == "echo") {
        double delayMs = parameters.size() > 0 ? parameters[0] : 250.0;
        double feedback = parameters.size() > 1 ? parameters[1] : 0.5;
        return std::make_unique<EchoEffect>(sampleRate, channels, delayMs, feedback);
    }
    else if (lowerName == "multiecho" || lowerName == "multi-echo") {
        double baseDelayMs = parameters.size() > 0 ? parameters[0] : 150.0;
        int numEchoes = parameters.size() > 1 ? static_cast<int>(parameters[1]) : 3;
        double feedbackDecay = parameters.size() > 2 ? parameters[2] : 0.6;
        return std::make_unique<MultiEchoEffect>(sampleRate, channels, baseDelayMs, numEchoes, feedbackDecay);
    }
    else if (lowerName == "amplitude" || lowerName == "am" || lowerName == "tremolo") {
        double modFreq = parameters.size() > 0 ? parameters[0] : 5.0;
        double depth = parameters.size() > 1 ? parameters[1] : 0.5;
        int waveform = parameters.size() > 2 ? static_cast<int>(parameters[2]) : 0;
        return std::make_unique<AmplitudeModulationEffect>(sampleRate, channels, modFreq, depth, waveform);
    }
    else if (lowerName == "chorus" || lowerName == "flanger") {
        double baseDelayMs = parameters.size() > 0 ? parameters[0] : 10.0;
        double modFreq = parameters.size() > 1 ? parameters[1] : 1.0;
        double modDepth = parameters.size() > 2 ? parameters[2] : 0.5;
        double feedback = parameters.size() > 3 ? parameters[3] : 0.3;
        double mix = parameters.size() > 4 ? parameters[4] : 0.5;
        return std::make_unique<ChorusEffect>(sampleRate, channels, baseDelayMs, modFreq, modDepth, feedback, mix);
    }
    else if (lowerName == "reverb") {
        double roomSize = parameters.size() > 0 ? parameters[0] : 0.5;
        double damping = parameters.size() > 1 ? parameters[1] : 0.5;
        double mix = parameters.size() > 2 ? parameters[2] : 0.3;
        return std::make_unique<ReverbEffect>(sampleRate, channels, roomSize, damping, mix);
    }
    else {
        throw std::invalid_argument("Unknown effect: " + effectName);
    }
}

std::vector<std::string> AudioEffectFactory::getAvailableEffects() {
    return {
        "echo",
        "multiecho", 
        "amplitude",
        "chorus",
        "reverb"
    };
}

std::string AudioEffectFactory::getEffectUsage(const std::string& effectName) {
    std::string lowerName = effectName;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    if (lowerName == "echo") {
        return "echo <delay_ms> <feedback>\n"
               "  delay_ms: Delay time in milliseconds (default: 250)\n"
               "  feedback: Feedback gain 0.0-0.99 (default: 0.5)";
    }
    else if (lowerName == "multiecho") {
        return "multiecho <base_delay_ms> <num_echoes> <feedback_decay>\n"
               "  base_delay_ms: Base delay time in milliseconds (default: 150)\n"
               "  num_echoes: Number of echoes (default: 3)\n"
               "  feedback_decay: Decay factor between echoes (default: 0.6)";
    }
    else if (lowerName == "amplitude") {
        return "amplitude <mod_freq> <depth> <waveform>\n"
               "  mod_freq: Modulation frequency in Hz (default: 5.0)\n"
               "  depth: Modulation depth 0.0-1.0 (default: 0.5)\n"
               "  waveform: 0=sine, 1=triangle, 2=square (default: 0)";
    }
    else if (lowerName == "chorus") {
        return "chorus <base_delay_ms> <mod_freq> <mod_depth> <feedback> <mix>\n"
               "  base_delay_ms: Base delay time in milliseconds (default: 10)\n"
               "  mod_freq: LFO frequency in Hz (default: 1.0)\n"
               "  mod_depth: Modulation depth 0.0-1.0 (default: 0.5)\n"
               "  feedback: Feedback amount 0.0-0.99 (default: 0.3)\n"
               "  mix: Dry/wet mix 0.0-1.0 (default: 0.5)";
    }
    else if (lowerName == "reverb") {
        return "reverb <room_size> <damping> <mix>\n"
               "  room_size: Room size 0.0-1.0 (default: 0.5)\n"
               "  damping: High frequency damping 0.0-1.0 (default: 0.5)\n"
               "  mix: Dry/wet mix 0.0-1.0 (default: 0.3)";
    }
    else {
        return "Unknown effect: " + effectName;
    }
}