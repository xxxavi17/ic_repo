//------------------------------------------------------------------------------
//
// Extended WAVHist class for Exercise 1
// Information and Coding (2025/26) - Lab work n° 1
// 
// Features:
// - MID channel histogram ((L + R)/2)
// - SIDE channel histogram ((L - R)/2) 
// - Coarser bins (group 2^k values together)
// - Visualization output for plotting
//
//------------------------------------------------------------------------------

#ifndef WAV_HIST_EXTENDED_H
#define WAV_HIST_EXTENDED_H

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <cmath>
#include <sndfile.hh>

enum class ChannelType {
    LEFT = 0,
    RIGHT = 1,
    MID = 2,     // (L + R) / 2
    SIDE = 3     // (L - R) / 2
};

class WAVHistExtended {
  private:
    std::vector<std::map<short, size_t>> counts;
    std::map<short, size_t> midCounts;   // MID channel histogram
    std::map<short, size_t> sideCounts;  // SIDE channel histogram
    size_t binSize;                      // For coarser bins (1, 2, 4, 8, ...)
    size_t numChannels;

    // Helper function to apply binning
    short applyBinning(short value) const {
        if (binSize == 1) return value;
        return (value / static_cast<short>(binSize)) * static_cast<short>(binSize);
    }

  public:
    WAVHistExtended(const SndfileHandle& sfh, size_t binSize = 1) 
        : binSize(binSize), numChannels(sfh.channels()) {
        counts.resize(sfh.channels());
    }

    void update(const std::vector<short>& samples) {
        size_t n = 0;
        
        // Update individual channel histograms
        for(auto s : samples) {
            short binnedValue = applyBinning(s);
            counts[n % counts.size()][binnedValue]++;
            n++;
        }
        
        // Update MID and SIDE histograms for stereo audio
        if (numChannels == 2 && samples.size() >= 2) {
            for (size_t i = 0; i < samples.size(); i += 2) {
                if (i + 1 < samples.size()) {
                    short left = samples[i];
                    short right = samples[i + 1];
                    
                    // MID channel: (L + R) / 2 (integer division)
                    short mid = static_cast<short>((static_cast<int>(left) + static_cast<int>(right)) / 2);
                    short binnedMid = applyBinning(mid);
                    midCounts[binnedMid]++;
                    
                    // SIDE channel: (L - R) / 2 (integer division)
                    short side = static_cast<short>((static_cast<int>(left) - static_cast<int>(right)) / 2);
                    short binnedSide = applyBinning(side);
                    sideCounts[binnedSide]++;
                }
            }
        }
    }

    void dump(const size_t channel) const {
        std::cout << "# Channel " << channel << " histogram (bin size: " << binSize << ")\n";
        std::cout << "# Value\tCount\n";
        for(auto [value, counter] : counts[channel]) {
            std::cout << value << '\t' << counter << '\n';
        }
    }

    void dumpMID() const {
        std::cout << "# MID channel histogram ((L+R)/2) (bin size: " << binSize << ")\n";
        std::cout << "# Value\tCount\n";
        for(auto [value, counter] : midCounts) {
            std::cout << value << '\t' << counter << '\n';
        }
    }

    void dumpSIDE() const {
        std::cout << "# SIDE channel histogram ((L-R)/2) (bin size: " << binSize << ")\n";
        std::cout << "# Value\tCount\n";
        for(auto [value, counter] : sideCounts) {
            std::cout << value << '\t' << counter << '\n';
        }
    }

    // Save histogram to file for visualization
    void saveToFile(ChannelType channelType, const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << " for writing\n";
            return;
        }

        switch(channelType) {
            case ChannelType::LEFT:
                if (counts.size() > 0) {
                    file << "# LEFT channel histogram (bin size: " << binSize << ")\n";
                    file << "# Value\tCount\n";
                    for(auto [value, counter] : counts[0]) {
                        file << value << '\t' << counter << '\n';
                    }
                }
                break;
            case ChannelType::RIGHT:
                if (counts.size() > 1) {
                    file << "# RIGHT channel histogram (bin size: " << binSize << ")\n";
                    file << "# Value\tCount\n";
                    for(auto [value, counter] : counts[1]) {
                        file << value << '\t' << counter << '\n';
                    }
                }
                break;
            case ChannelType::MID:
                file << "# MID channel histogram ((L+R)/2) (bin size: " << binSize << ")\n";
                file << "# Value\tCount\n";
                for(auto [value, counter] : midCounts) {
                    file << value << '\t' << counter << '\n';
                }
                break;
            case ChannelType::SIDE:
                file << "# SIDE channel histogram ((L-R)/2) (bin size: " << binSize << ")\n";
                file << "# Value\tCount\n";
                for(auto [value, counter] : sideCounts) {
                    file << value << '\t' << counter << '\n';
                }
                break;
        }
        file.close();
        std::cout << "Histogram saved to: " << filename << std::endl;
    }

    // Generate Python script for visualization
    void generateVisualizationScript(const std::string& scriptPath, 
                                   const std::vector<std::string>& dataFiles) const {
        std::ofstream script(scriptPath);
        if (!script.is_open()) {
            std::cerr << "Error: Could not create visualization script\n";
            return;
        }

        script << "#!/usr/bin/env python3\n";
        script << "import matplotlib.pyplot as plt\n";
        script << "import numpy as np\n\n";
        
        script << "def load_histogram(filename):\n";
        script << "    values, counts = [], []\n";
        script << "    with open(filename, 'r') as f:\n";
        script << "        for line in f:\n";
        script << "            if line.startswith('#'):\n";
        script << "                continue\n";
        script << "            parts = line.strip().split('\\t')\n";
        script << "            if len(parts) == 2:\n";
        script << "                values.append(int(parts[0]))\n";
        script << "                counts.append(int(parts[1]))\n";
        script << "    return np.array(values), np.array(counts)\n\n";

        script << "fig, axes = plt.subplots(2, 2, figsize=(15, 10))\n";
        script << "fig.suptitle('WAV Audio Histograms - Exercise 1', fontsize=16)\n\n";

        std::vector<std::string> titles = {"Left Channel", "Right Channel", "MID Channel ((L+R)/2)", "SIDE Channel ((L-R)/2)"};
        
        for (size_t i = 0; i < dataFiles.size() && i < 4; ++i) {
            int row = i / 2;
            int col = i % 2;
            script << "# Plot " << titles[i] << "\n";
            script << "values, counts = load_histogram('" << dataFiles[i] << "')\n";
            script << "axes[" << row << ", " << col << "].bar(values, counts, width=1, alpha=0.7)\n";
            script << "axes[" << row << ", " << col << "].set_title('" << titles[i] << "')\n";
            script << "axes[" << row << ", " << col << "].set_xlabel('Sample Value')\n";
            script << "axes[" << row << ", " << col << "].set_ylabel('Count')\n";
            script << "axes[" << row << ", " << col << "].grid(True, alpha=0.3)\n\n";
        }

        script << "plt.tight_layout()\n";
        script << "plt.savefig('histograms_exercise1.png', dpi=300, bbox_inches='tight')\n";
        script << "plt.show()\n";
        script << "print('Histogram visualization saved as histograms_exercise1.png')\n";

        script.close();
        std::cout << "Visualization script created: " << scriptPath << std::endl;
    }

    size_t getNumChannels() const { return numChannels; }
    size_t getBinSize() const { return binSize; }
};

#endif