#ifndef WAVQUANTENC_H
#define WAVQUANTENC_H

#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <fstream>
#include <iomanip>
#include "bit_stream.h"

class WAVQuantEnc {
private:
    std::vector<short> samples;
    int sampleRate;
    int channels;
    int frames;
    int targetBits;

    // Quantização uniforme (baseada no wav_quant original)
    short quantizeSample(short sample) {
        if (targetBits >= 16) return sample;
        
        int numLevels = 1 << targetBits; // 2^targetBits
        double stepSize = 65536.0 / numLevels;
        
        // Normalizar para [0, numLevels-1]
        int quantized = static_cast<int>((sample + 32768.0) / stepSize);
        
        // Limitar ao range válido
        if (quantized >= numLevels) quantized = numLevels - 1;
        if (quantized < 0) quantized = 0;
        
        return quantized; // Retornar valor quantizado (0 a numLevels-1)
    }

public:
    WAVQuantEnc(SndfileHandle& sfh, int bits) : targetBits(bits) {
        frames = sfh.frames();
        sampleRate = sfh.samplerate();
        channels = sfh.channels();
        samples.resize(frames * channels);
        
        if (sfh.readf(samples.data(), frames) != frames) {
            throw std::runtime_error("Erro ao ler amostras do ficheiro WAV");
        }
    }

    void quantizeAndEncode(const std::string& outputFile) {
        if (targetBits < 1 || targetBits > 16) {
            throw std::invalid_argument("targetBits deve estar entre 1 e 16");
        }

        std::cout << "=== WAV Quantization Encoder ===" << std::endl;
        std::cout << "Input: " << frames << " frames, " << sampleRate << " Hz, " 
                  << channels << " channels" << std::endl;
        std::cout << "Quantization: 16-bit → " << targetBits << "-bit" << std::endl;

        std::fstream ofs(outputFile, std::ios::out | std::ios::binary);
        if (!ofs.is_open()) {
            throw std::runtime_error("Erro ao criar ficheiro de saída: " + outputFile);
        }

        BitStream bs(ofs, STREAM_WRITE);

        // Escrever header
        writeHeader(bs);

        // Quantizar e codificar
        std::cout << "Codificando " << samples.size() << " amostras..." << std::endl;

        for (size_t i = 0; i < samples.size(); i++) {
            int quantizedValue = quantizeSample(samples[i]);
            bs.write_n_bits(quantizedValue, targetBits);
        }

        bs.close();
        ofs.close();

        // Estatísticas
        size_t originalBits = samples.size() * 16;
        size_t encodedBits = 128 + samples.size() * targetBits; // header + dados
        double compressionRatio = static_cast<double>(originalBits) / encodedBits;

        std::cout << "Codificação concluída!" << std::endl;
        std::cout << "Tamanho original: " << originalBits << " bits" << std::endl;
        std::cout << "Tamanho codificado: " << encodedBits << " bits" << std::endl;
        std::cout << "Taxa de compressão: " << std::fixed << std::setprecision(2) 
                  << compressionRatio << ":1" << std::endl;
    }

private:
    void writeHeader(BitStream& bs) {
        // Header: 4 campos de 32 bits cada = 128 bits total
        bs.write_n_bits(sampleRate, 32);
        bs.write_n_bits(channels, 32);
        bs.write_n_bits(frames, 32);
        bs.write_n_bits(targetBits, 32);
    }
};

#endif