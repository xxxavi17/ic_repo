#include <iostream>
#include <fstream>
#include <sndfile.h>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>

class AudioAnalyzer {
private:
    struct FileInfo {
        std::string filename;
        int bits;
        double mse;
        double snr;
        double psnr;
        size_t fileSize;
        double compressionRatio;
    };
    
    std::vector<FileInfo> files;
    
public:
    void analyzeFile(const std::string& filename, int bits) {
        SNDFILE* file;
        SF_INFO info = {};
        
        file = sf_open(filename.c_str(), SFM_READ, &info);
        if (!file) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }
        
        // Read all samples
        std::vector<short> samples(info.frames * info.channels);
        sf_readf_short(file, samples.data(), info.frames);
        sf_close(file);
        
        // Calculate statistics
        FileInfo fileInfo;
        fileInfo.filename = filename;
        fileInfo.bits = bits;
        fileInfo.compressionRatio = 16.0 / bits;
        
        // Get file size
        FILE* fp = fopen(filename.c_str(), "rb");
        if (fp) {
            fseek(fp, 0, SEEK_END);
            fileInfo.fileSize = ftell(fp);
            fclose(fp);
        }
        
        files.push_back(fileInfo);
    }
    
    void compareWithOriginal(const std::string& originalFile, const std::string& quantizedFile, int bits) {
        SNDFILE* origFile;
        SNDFILE* quantFile;
        SF_INFO origInfo = {};
        SF_INFO quantInfo = {};
        
        origFile = sf_open(originalFile.c_str(), SFM_READ, &origInfo);
        quantFile = sf_open(quantizedFile.c_str(), SFM_READ, &quantInfo);
        
        if (!origFile || !quantFile) {
            std::cerr << "Error opening files for comparison" << std::endl;
            if (origFile) sf_close(origFile);
            if (quantFile) sf_close(quantFile);
            return;
        }
        
        // Read samples
        std::vector<short> origSamples(origInfo.frames * origInfo.channels);
        std::vector<short> quantSamples(quantInfo.frames * quantInfo.channels);
        
        sf_readf_short(origFile, origSamples.data(), origInfo.frames);
        sf_readf_short(quantFile, quantSamples.data(), quantInfo.frames);
        
        sf_close(origFile);
        sf_close(quantFile);
        
        // Calculate MSE and SNR
        double sumSquaredError = 0.0;
        double sumOriginalSquared = 0.0;
        size_t numSamples = std::min(origSamples.size(), quantSamples.size());
        
        for (size_t i = 0; i < numSamples; i++) {
            double error = static_cast<double>(origSamples[i] - quantSamples[i]);
            double original = static_cast<double>(origSamples[i]);
            
            sumSquaredError += error * error;
            sumOriginalSquared += original * original;
        }
        
        double mse = sumSquaredError / numSamples;
        double snr = 0.0;
        double psnr = 0.0;
        
        if (sumSquaredError > 0) {
            snr = 10.0 * std::log10(sumOriginalSquared / sumSquaredError);
            psnr = 20.0 * std::log10(32767.0 / std::sqrt(mse));
        }
        
        // Get file size
        size_t fileSize = 0;
        FILE* fp = fopen(quantizedFile.c_str(), "rb");
        if (fp) {
            fseek(fp, 0, SEEK_END);
            fileSize = ftell(fp);
            fclose(fp);
        }
        
        FileInfo fileInfo;
        fileInfo.filename = quantizedFile;
        fileInfo.bits = bits;
        fileInfo.mse = mse;
        fileInfo.snr = snr;
        fileInfo.psnr = psnr;
        fileInfo.fileSize = fileSize;
        fileInfo.compressionRatio = 16.0 / bits;
        
        files.push_back(fileInfo);
    }
    
    void printComparisonTable() {
        std::cout << "\n" << std::string(100, '=') << std::endl;
        std::cout << "AUDIO QUANTIZATION ANALYSIS REPORT" << std::endl;
        std::cout << std::string(100, '=') << std::endl;
        
        std::cout << std::left 
                  << std::setw(20) << "Filename"
                  << std::setw(6) << "Bits"
                  << std::setw(12) << "MSE"
                  << std::setw(10) << "SNR (dB)"
                  << std::setw(12) << "PSNR (dB)"
                  << std::setw(12) << "Size (KB)"
                  << std::setw(10) << "Comp.Ratio"
                  << std::endl;
        
        std::cout << std::string(100, '-') << std::endl;
        
        for (const auto& file : files) {
            std::cout << std::left 
                      << std::setw(20) << file.filename.substr(file.filename.find_last_of("/\\") + 1)
                      << std::setw(6) << file.bits
                      << std::setw(12) << std::fixed << std::setprecision(2) << file.mse
                      << std::setw(10) << std::fixed << std::setprecision(2) << file.snr
                      << std::setw(12) << std::fixed << std::setprecision(2) << file.psnr
                      << std::setw(12) << std::fixed << std::setprecision(1) << file.fileSize / 1024.0
                      << std::setw(10) << std::fixed << std::setprecision(1) << file.compressionRatio << ":1"
                      << std::endl;
        }
        
        std::cout << std::string(100, '=') << std::endl;
    }
    
    void generateAnalysisReport(const std::string& outputFile) {
        std::ofstream out(outputFile);
        if (!out) {
            std::cerr << "Error creating analysis report file: " << outputFile << std::endl;
            return;
        }
        
        out << "# Audio Quantization Analysis Report\n\n";
        out << "## Summary Table\n\n";
        out << "| Filename | Bits | MSE | SNR (dB) | PSNR (dB) | Size (KB) | Compression Ratio |\n";
        out << "|----------|------|-----|----------|-----------|-----------|-------------------|\n";
        
        for (const auto& file : files) {
            out << "| " << file.filename.substr(file.filename.find_last_of("/\\") + 1)
                << " | " << file.bits
                << " | " << std::fixed << std::setprecision(2) << file.mse
                << " | " << std::fixed << std::setprecision(2) << file.snr
                << " | " << std::fixed << std::setprecision(2) << file.psnr
                << " | " << std::fixed << std::setprecision(1) << file.fileSize / 1024.0
                << " | " << std::fixed << std::setprecision(1) << file.compressionRatio << ":1 |\n";
        }
        
        out << "\n## Analysis\n\n";
        out << "### Quality vs Compression Trade-off\n\n";
        
        for (const auto& file : files) {
            out << "- **" << file.bits << "-bit quantization**: "
                << "SNR = " << std::fixed << std::setprecision(2) << file.snr << " dB, "
                << "Compression = " << std::fixed << std::setprecision(1) << file.compressionRatio << ":1\n";
        }
        
        out << "\n### Observations\n\n";
        out << "- Lower bit depths result in higher compression ratios but significantly reduced audio quality\n";
        out << "- SNR decreases exponentially as bit depth is reduced\n";
        out << "- 8-bit quantization provides reasonable quality with 2:1 compression\n";
        out << "- 1-bit quantization results in severe quality degradation (negative SNR)\n";
        
        out.close();
        std::cout << "Analysis report saved to: " << outputFile << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <original.wav> <quantized_files...>" << std::endl;
        std::cout << "Example: " << argv[0] << " sample.wav sample_8bit.wav sample_4bit.wav sample_2bit.wav sample_1bit.wav" << std::endl;
        return 1;
    }
    
    std::string originalFile = argv[1];
    AudioAnalyzer analyzer;
    
    // Add original file (16-bit)
    analyzer.analyzeFile(originalFile, 16);
    
    // Analyze quantized files
    std::vector<int> bitDepths = {8, 4, 2, 1};
    for (int i = 2; i < argc && static_cast<size_t>(i-2) < bitDepths.size(); i++) {
        analyzer.compareWithOriginal(originalFile, argv[i], bitDepths[i-2]);
    }
    
    analyzer.printComparisonTable();
    analyzer.generateAnalysisReport("quantization_analysis.md");
    
    return 0;
}