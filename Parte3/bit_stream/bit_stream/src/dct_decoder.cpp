//-------------------------------------------------------------------------------------------
//
// DCT Decoder - Programa para descodificar ficheiros .dct
//
//-------------------------------------------------------------------------------------------

#include "dct_codec.h"
#include <iostream>
#include <string>

using namespace std;

/**
 * @brief 
 */
int main(int argc, char* argv[]) {
    cout << "DCT Audio Decoder" << endl;
    cout << "=================" << endl;
    
    // Verificar argumentos
    if (argc < 3) {
        cout << "\nUso: " << argv[0] << " <input.dct> <output.wav>" << endl;
        cout << "\nExemplo:" << endl;
        cout << "  " << argv[0] << " audio.dct audio_decoded.wav" << endl;
        return 1;
    }
    
    string input_file = argv[1];
    string output_file = argv[2];
    
    cout << "\nParâmetros:" << endl;
    cout << "  Input:  " << input_file << endl;
    cout << "  Output: " << output_file << endl;
    cout << endl;
    
    // Criar codec e descodificar
    // Os parâmetros serão lidos do ficheiro .dct
    DCTCodec codec(512, 256, 2); // Valores temporários, serão substituídos
    
    if (!codec.decode(input_file, output_file)) {
        cerr << "\nErro durante a descodificação!" << endl;
        return 1;
    }
    
    cout << "\nDescodificação concluída com sucesso!" << endl;
    return 0;
}
