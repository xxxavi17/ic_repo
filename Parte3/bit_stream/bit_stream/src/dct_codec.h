//-------------------------------------------------------------------------------------------
//
// DCT Audio Codec - Header File
// Implementa codec com perdas baseado em Transformada Discreta do Cosseno (DCT)
//
//-------------------------------------------------------------------------------------------

#ifndef DCT_CODEC_H
#define DCT_CODEC_H

#include <vector>
#include <string>
#include <cmath>

/**
 * @brief Classe para codec de áudio com perdas baseado em DCT
 * 
 * Processa áudio mono em blocos, aplicando:
 * 1. DCT (Discrete Cosine Transform)
 * 2. Quantização dos coeficientes
 * 3. Escrita em ficheiro binário usando BitStream
 */
class DCTCodec {
private:
    int m_block_size;           // Tamanho do bloco (ex: 256, 512, 1024)
    int m_num_coeffs;           // Número de coeficientes DCT a guardar
    int m_quantization_factor;  // Fator de quantização (Q)
    
    /**
     * @brief Aplica DCT (Discrete Cosine Transform) a um bloco de amostras
     * @param samples Vetor com as amostras do bloco
     * @return Vetor com os coeficientes DCT
     */
    std::vector<double> apply_dct(const std::vector<short>& samples);
    
    /**
     * @brief Aplica IDCT (Inverse DCT) aos coeficientes
     * @param coeffs Vetor com os coeficientes DCT
     * @return Vetor com as amostras reconstruídas
     */
    std::vector<short> apply_idct(const std::vector<double>& coeffs);
    
    /**
     * @brief Quantiza um coeficiente DCT
     * @param coeff Coeficiente DCT
     * @return Coeficiente quantizado (inteiro)
     */
    int quantize(double coeff);
    
    /**
     * @brief Dequantiza um coeficiente
     * @param quantized_coeff Coeficiente quantizado
     * @return Coeficiente dequantizado
     */
    double dequantize(int quantized_coeff);

public:
    /**
     * @brief Construtor do codec
     * @param block_size Tamanho do bloco (ex: 256, 512, 1024)
     * @param num_coeffs Número de coeficientes a guardar (k)
     * @param quantization_factor Fator de quantização (Q)
     */
    DCTCodec(int block_size, int num_coeffs, int quantization_factor);
    
    /**
     * @brief Codifica um ficheiro de áudio WAV
     * @param input_file Nome do ficheiro de entrada (.wav)
     * @param output_file Nome do ficheiro de saída (.dct)
     * @return true se sucesso, false caso contrário
     */
    bool encode(const std::string& input_file, const std::string& output_file);
    
    /**
     * @brief Descodifica um ficheiro .dct
     * @param input_file Nome do ficheiro de entrada (.dct)
     * @param output_file Nome do ficheiro de saída (.wav)
     * @return true se sucesso, false caso contrário
     */
    bool decode(const std::string& input_file, const std::string& output_file);
    
    // Getters
    int get_block_size() const { return m_block_size; }
    int get_num_coeffs() const { return m_num_coeffs; }
    int get_quantization_factor() const { return m_quantization_factor; }
    
    // Método de teste público para validação
    double test_roundtrip(const std::vector<short>& samples);
};

/**
 * @brief Estrutura para armazenar cabeçalho WAV simplificado (mono, 16-bit)
 */
struct WAVHeader {
    char riff[4];              // "RIFF"
    int file_size;             // Tamanho do ficheiro - 8
    char wave[4];              // "WAVE"
    char fmt[4];               // "fmt "
    int fmt_size;              // 16 para PCM
    short audio_format;        // 1 para PCM
    short num_channels;        // 1 para mono
    int sample_rate;           // Ex: 44100
    int byte_rate;             // sample_rate * num_channels * bits_per_sample / 8
    short block_align;         // num_channels * bits_per_sample / 8
    short bits_per_sample;     // 16
    char data[4];              // "data"
    int data_size;             // Tamanho dos dados de áudio
};

/**
 * @brief Lê cabeçalho e amostras de um ficheiro WAV mono 16-bit
 * @param filename Nome do ficheiro WAV
 * @param header Estrutura para armazenar o cabeçalho
 * @param samples Vetor para armazenar as amostras
 * @return true se sucesso, false caso contrário
 */
bool read_wav_file(const std::string& filename, WAVHeader& header, std::vector<short>& samples);

/**
 * @brief Escreve cabeçalho e amostras para um ficheiro WAV mono 16-bit
 * @param filename Nome do ficheiro WAV
 * @param header Cabeçalho WAV
 * @param samples Vetor com as amostras
 * @return true se sucesso, false caso contrário
 */
bool write_wav_file(const std::string& filename, const WAVHeader& header, const std::vector<short>& samples);

#endif
