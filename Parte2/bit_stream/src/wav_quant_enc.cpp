#include <iostream>
#include <sndfile.hh>
#include "wav_quant_enc.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " <input.wav> <output.bin> <target_bits>" << std::endl;
        std::cout << "  target_bits: número de bits para quantização (1-16)" << std::endl;
        return 1;
    }

    // Ler arquivo WAV de entrada
    SndfileHandle sndFileIn{argv[1]};
    int error = sndFileIn.error();
    if (error) {
        std::cout << "Erro ao ler o arquivo: " << sndFileIn.strError() << std::endl;
        return 1;
    }

    int target_bits = std::stoi(argv[3]);
    if (target_bits < 1 || target_bits > 16) {
        std::cout << "Erro: target_bits deve estar entre 1 e 16" << std::endl;
        return 1;
    }

    // Criar encoder e processar
    WAVQuantEnc encoder{sndFileIn, target_bits};
    encoder.quantizeAndEncode(argv[2]);

    return 0;
}