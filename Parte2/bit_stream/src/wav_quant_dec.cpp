#include <iostream>
#include "wav_quant_dec.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <input.bin> <output.wav>" << std::endl;
        return 1;
    }

    WAVQuantDec decoder;
    decoder.decodeFromFile(argv[1], argv[2]);

    return 0;
}