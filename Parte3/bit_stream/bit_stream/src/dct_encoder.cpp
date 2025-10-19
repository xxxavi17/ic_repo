//-------------------------------------------------------------------------------------------
//
// DCT Encoder - Programa para codificar ficheiros WAV
//
//-------------------------------------------------------------------------------------------

#include "dct_codec.h"
#include <iostream>
#include <string>

using namespace std;

/**
 * @brief Programa principal do encoder
 */
int main(int argc, char* argv[]) {
    cout << "DCT Audio Encoder" << endl;
    cout << "=================" << endl;
    
    // Verificar argumentos
    if (argc < 3) {
        cout << "\nUso: " << argv[0] << " <input.wav> <output.dct> [block_size] [num_coeffs] [quant_factor]" << endl;
        cout << "\nParâmetros opcionais:" << endl;
        cout << "  block_size     - Tamanho do bloco (default: 512)" << endl;
        cout << "  num_coeffs     - Número de coeficientes DCT (default: 256)" << endl;
        cout << "  quant_factor   - Fator de quantização (default: 2)" << endl;
        cout << "\nExemplos:" << endl;
        cout << "  " << argv[0] << " audio.wav audio.dct" << endl;
        cout << "  " << argv[0] << " audio.wav audio.dct 1024 128 10" << endl;
        return 1;
    }
    
    string input_file = argv[1];
    string output_file = argv[2];
    
    // Parâmetros do codec (com valores default)
    int block_size = (argc > 3) ? atoi(argv[3]) : 512;
    int num_coeffs = (argc > 4) ? atoi(argv[4]) : 256;
    int quant_factor = (argc > 5) ? atoi(argv[5]) : 2;
    
    // Validar parâmetros
    if (block_size <= 0 || num_coeffs <= 0 || quant_factor <= 0) {
        cerr << "Erro: parâmetros devem ser positivos!" << endl;
        return 1;
    }
    
    if (num_coeffs > block_size) {
        cerr << "Aviso: num_coeffs > block_size, ajustando para " << block_size << endl;
        num_coeffs = block_size;
    }
    
    cout << "\nParâmetros:" << endl;
    cout << "  Input:  " << input_file << endl;
    cout << "  Output: " << output_file << endl;
    cout << "  Block size: " << block_size << endl;
    cout << "  Num coeffs: " << num_coeffs << endl;
    cout << "  Quant factor: " << quant_factor << endl;
    cout << endl;
    
    // Criar codec e codificar
    DCTCodec codec(block_size, num_coeffs, quant_factor);
    
    if (!codec.encode(input_file, output_file)) {
        cerr << "\nErro durante a codificação!" << endl;
        return 1;
    }
    
    cout << "\nCodificação concluída com sucesso!" << endl;
    return 0;
}
