#ifndef WAVQUANTDEC_H
#define WAVQUANTDEC_H

#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <fstream>
#include "bit_stream.h"

class WAVQuantDec {
private:
    std::vector<short> samples;
    int sampleRate;
    int channels;
    int frames;
    int targetBits;

    // Dequantização (reconstrução do valor original)
    short dequantizeSample(int quantizedValue) {
        if (targetBits >= 16) return static_cast<short>(quantizedValue);
        
        int numLevels = 1 << targetBits;
        double stepSize = 65536.0 / numLevels;
        
        // Reconstruir valor no centro do intervalo de quantização
        double reconstructed = quantizedValue * stepSize - 32768.0 + stepSize / 2.0;
        
        // Limitar ao range de 16-bit signed
        if (reconstructed > 32767.0) reconstructed = 32767.0;
        if (reconstructed < -32768.0) reconstructed = -32768.0;
        
        return static_cast<short>(reconstructed);
    }

public:
    WAVQuantDec() = default;

    void decodeFromFile(const std::string& inputFile, const std::string& outputWav) {
        std::fstream ifs(inputFile, std::ios::in | std::ios::binary);
        if (!ifs.is_open()) {
            throw std::runtime_error("Erro ao abrir ficheiro: " + inputFile);
        }

        BitStream bs(ifs, STREAM_READ);

        // Ler header
        if (!readHeader(bs)) {
            throw std::runtime_error("Erro ao ler header do ficheiro");
        }

        std::cout << "=== WAV Quantization Decoder ===" << std::endl;
        std::cout << "Header: " << frames << " frames, " << sampleRate << " Hz, " 
                  << channels << " channels" << std::endl;
        std::cout << "Quantization: " << targetBits << "-bit → 16-bit" << std::endl;

        // Preparar vetor para amostras
        samples.resize(frames * channels);

        std::cout << "Descodificando " << samples.size() << " amostras..." << std::endl;

        // Ler e dequantizar amostras
        for (size_t i = 0; i < samples.size(); i++) {
            uint64_t quantizedValue = bs.read_n_bits(targetBits);
            samples[i] = dequantizeSample(static_cast<int>(quantizedValue));
        }

        bs.close();
        ifs.close();

        // Escrever ficheiro WAV
        writeWavFile(outputWav);
        std::cout << "Descodificação concluída: " << outputWav << std::endl;
    }

private:
    bool readHeader(BitStream& bs) {
        try {
            sampleRate = static_cast<int>(bs.read_n_bits(32));
            channels = static_cast<int>(bs.read_n_bits(32));
            frames = static_cast<int>(bs.read_n_bits(32));
            targetBits = static_cast<int>(bs.read_n_bits(32));
            
            // Validar valores
            if (sampleRate <= 0 || channels <= 0 || frames <= 0 || 
                targetBits < 1 || targetBits > 16) {
                return false;
            }
            
            return true;
        } catch (...) {
            return false;
        }
    }

    void writeWavFile(const std::string& outputWav) {
        SndfileHandle sndFileOut{outputWav, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16, 
                                channels, sampleRate};
        
        if (sndFileOut.error()) {
            throw std::runtime_error("Erro ao criar ficheiro WAV: " + std::string(sndFileOut.strError()));
        }

        sf_count_t written = sndFileOut.writef(samples.data(), frames);
        if (written != frames) {
            throw std::runtime_error("Erro ao escrever todas as amostras");
        }
    }
};

#endif