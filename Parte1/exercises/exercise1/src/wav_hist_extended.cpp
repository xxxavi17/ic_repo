//------------------------------------------------------------------------------
//
// Extended WAV Histogram Program for Exercise 1
// Information and Coding (2025/26) - Lab work n° 1
//
// Usage: wav_hist_extended <input_file> <bin_size> [options]
// Options:
//   -v : verbose mode
//   -save : save histograms to files
//   -plot : generate visualization script
//
//------------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <string>
#include <sndfile.hh>
#include "wav_hist_extended.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536;

void printUsage(const char* progName) {
    cerr << "Usage: " << progName << " <input_file> <bin_size> [options]\n";
    cerr << "  bin_size: 1, 2, 4, 8, 16, ... (power of 2)\n";
    cerr << "  Options:\n";
    cerr << "    -v      : verbose mode\n";
    cerr << "    -save   : save histograms to files\n";
    cerr << "    -plot   : generate Python visualization script\n";
    cerr << "    -all    : display all histograms\n";
    cerr << "    -mid    : display only MID histogram\n";
    cerr << "    -side   : display only SIDE histogram\n";
}

bool isPowerOfTwo(size_t n) {
    return n > 0 && (n & (n - 1)) == 0;
}

int main(int argc, char *argv[]) {
    if(argc < 3) {
        printUsage(argv[0]);
        return 1;
    }

    string inputFile = argv[1];
    size_t binSize = stoul(argv[2]);
    
    if (!isPowerOfTwo(binSize)) {
        cerr << "Error: bin_size must be a power of 2 (1, 2, 4, 8, 16, ...)\n";
        return 1;
    }

    // Parse options
    bool verbose = false;
    bool saveFiles = false;
    bool generatePlot = false;
    bool showAll = false;
    bool showMidOnly = false;
    bool showSideOnly = false;

    for(int i = 3; i < argc; i++) {
        string arg = argv[i];
        if(arg == "-v") verbose = true;
        else if(arg == "-save") saveFiles = true;
        else if(arg == "-plot") generatePlot = true;
        else if(arg == "-all") showAll = true;
        else if(arg == "-mid") showMidOnly = true;
        else if(arg == "-side") showSideOnly = true;
    }

    // If no specific display option, show all
    if (!showAll && !showMidOnly && !showSideOnly) {
        showAll = true;
    }

    // Open input file
    SndfileHandle sndFile { inputFile };
    if(sndFile.error()) {
        cerr << "Error: invalid input file '" << inputFile << "'\n";
        return 1;
    }

    if((sndFile.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
        cerr << "Error: file is not in WAV format\n";
        return 1;
    }

    if((sndFile.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
        cerr << "Error: file is not in PCM_16 format\n";
        return 1;
    }

    if(verbose) {
        cout << "=== File Information ===\n";
        cout << "File: " << inputFile << "\n";
        cout << "Frames: " << sndFile.frames() << "\n";
        cout << "Sample rate: " << sndFile.samplerate() << " Hz\n";
        cout << "Channels: " << sndFile.channels() << "\n";
        cout << "Bin size: " << binSize << "\n";
        cout << "Duration: " << (double)sndFile.frames() / sndFile.samplerate() << " seconds\n\n";
    }

    // Create extended histogram object
    WAVHistExtended hist { sndFile, binSize };

    // Read and process audio data
    size_t nFrames;
    vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
    
    if(verbose) cout << "Processing audio samples...\n";
    
    while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
        samples.resize(nFrames * sndFile.channels());
        hist.update(samples);
    }

    if(verbose) cout << "Processing complete!\n\n";

    // Display histograms
    if (showAll || showMidOnly) {
        if (sndFile.channels() == 2) {
            cout << "\n=== MID Channel Histogram ((L+R)/2) ===\n";
            hist.dumpMID();
        } else {
            cout << "Note: MID channel histogram only available for stereo audio\n";
        }
    }

    if (showAll || showSideOnly) {
        if (sndFile.channels() == 2) {
            cout << "\n=== SIDE Channel Histogram ((L-R)/2) ===\n";
            hist.dumpSIDE();
        } else {
            cout << "Note: SIDE channel histogram only available for stereo audio\n";
        }
    }

    if (showAll && !showMidOnly && !showSideOnly) {
        for (int ch = 0; ch < sndFile.channels(); ch++) {
            cout << "\n=== Channel " << ch << " Histogram ===\n";
            hist.dump(ch);
        }
    }

    // Save histograms to files if requested
    if (saveFiles) {
        string baseName = inputFile.substr(0, inputFile.find_last_of('.'));
        string suffix = "_bin" + to_string(binSize);
        
        if (sndFile.channels() >= 1) {
            hist.saveToFile(ChannelType::LEFT, baseName + "_left" + suffix + ".txt");
        }
        if (sndFile.channels() >= 2) {
            hist.saveToFile(ChannelType::RIGHT, baseName + "_right" + suffix + ".txt");
            hist.saveToFile(ChannelType::MID, baseName + "_mid" + suffix + ".txt");
            hist.saveToFile(ChannelType::SIDE, baseName + "_side" + suffix + ".txt");
        }
    }

    // Generate visualization script if requested
    if (generatePlot && sndFile.channels() == 2) {
        string baseName = inputFile.substr(0, inputFile.find_last_of('.'));
        string suffix = "_bin" + to_string(binSize);
        
        vector<string> dataFiles = {
            baseName + "_left" + suffix + ".txt",
            baseName + "_right" + suffix + ".txt", 
            baseName + "_mid" + suffix + ".txt",
            baseName + "_side" + suffix + ".txt"
        };
        
        hist.generateVisualizationScript("plot_histograms.py", dataFiles);
        
        if (!saveFiles) {
            cout << "\nNote: To use the visualization script, run with -save option first to generate data files.\n";
        }
    }

    // Show recovery information
    if (sndFile.channels() == 2 && verbose) {
        cout << "\n=== Channel Recovery Information ===\n";
        cout << "From MID and SIDE channels, you can recover the original L and R channels:\n";
        cout << "Left (L)  = MID + SIDE = ((L+R)/2) + ((L-R)/2) = L\n";
        cout << "Right (R) = MID - SIDE = ((L+R)/2) - ((L-R)/2) = R\n";
        cout << "This is exact recovery when using integer arithmetic.\n";
    }

    return 0;
}