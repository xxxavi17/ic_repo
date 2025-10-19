//-------------------------------------------------------------------------------------------
//
// DCT Audio Codec - Implementation
// Codec com perdas baseado em DCT para áudio mono
//
//-------------------------------------------------------------------------------------------

#include "dct_codec.h"
#include "bit_stream.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <cstring>

using namespace std;

// Constantes
const double PI = 3.14159265358979323846;

//-------------------------------------------------------------------------------------------
// Construtor
//-------------------------------------------------------------------------------------------
DCTCodec::DCTCodec(int block_size, int num_coeffs, int quantization_factor)
    : m_block_size(block_size), 
      m_num_coeffs(num_coeffs), 
      m_quantization_factor(quantization_factor) {
    
    // Validação dos parâmetros
    if (m_num_coeffs > m_block_size) {
        m_num_coeffs = m_block_size;
    }
}

//-------------------------------------------------------------------------------------------
// DCT - Transformada Discreta do Cosseno
//-------------------------------------------------------------------------------------------
vector<double> DCTCodec::apply_dct(const vector<short>& samples) {
    vector<double> coeffs(m_block_size, 0.0);
    int N = m_block_size;
    
    for (int k = 0; k < N; k++) {
        double sum = 0.0;
        for (int n = 0; n < N; n++) {
            sum += samples[n] * cos(PI * k * (2.0 * n + 1.0) / (2.0 * N));
        }
        
        // Fator de normalização
        double alpha = (k == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N);
        coeffs[k] = alpha * sum;
    }
    
    return coeffs;
}

//-------------------------------------------------------------------------------------------
// IDCT - Transformada Inversa
//-------------------------------------------------------------------------------------------
vector<short> DCTCodec::apply_idct(const vector<double>& coeffs) {
    vector<short> samples(m_block_size, 0);
    int N = m_block_size;
    
    for (int n = 0; n < N; n++) {
        double sum = 0.0;
        for (int k = 0; k < N; k++) {
            double alpha = (k == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N);
            sum += alpha * coeffs[k] * cos(PI * k * (2.0 * n + 1.0) / (2.0 * N));
        }
        
        // Limitar valores ao intervalo de 16-bit signed
        if (sum > 32767.0) sum = 32767.0;
        if (sum < -32768.0) sum = -32768.0;
        samples[n] = static_cast<short>(round(sum));
    }
    
    return samples;
}

//-------------------------------------------------------------------------------------------
// Quantização
//-------------------------------------------------------------------------------------------
int DCTCodec::quantize(double coeff) {
    return static_cast<int>(round(coeff / m_quantization_factor));
}

//-------------------------------------------------------------------------------------------
// Dequantização
//-------------------------------------------------------------------------------------------
double DCTCodec::dequantize(int quantized_coeff) {
    return quantized_coeff * m_quantization_factor;
}

//-------------------------------------------------------------------------------------------
// Método de teste - Roundtrip DCT+Quantização+IDCT
//-------------------------------------------------------------------------------------------
double DCTCodec::test_roundtrip(const vector<short>& samples) {
    // Aplicar DCT
    vector<double> dct_coeffs = apply_dct(samples);
    
    // Quantizar todos os coeficientes
    vector<double> quantized_coeffs(m_block_size, 0.0);
    for (int k = 0; k < m_block_size; k++) {
        int q = quantize(dct_coeffs[k]);
        quantized_coeffs[k] = dequantize(q);
    }
    
    // Aplicar IDCT
    vector<short> reconstructed = apply_idct(quantized_coeffs);
    
    // Calcular MSE
    double mse = 0.0;
    for (size_t i = 0; i < samples.size(); i++) {
        double diff = samples[i] - reconstructed[i];
        mse += diff * diff;
    }
    mse /= samples.size();
    
    // Calcular potência do sinal
    double signal_power = 0.0;
    for (size_t i = 0; i < samples.size(); i++) {
        signal_power += samples[i] * samples[i];
    }
    signal_power /= samples.size();
    
    // SNR em dB
    if (mse < 1e-10) return 100.0;  // Praticamente perfeito
    return 10.0 * log10(signal_power / mse);
}

//-------------------------------------------------------------------------------------------
// Leitura de ficheiro WAV
//-------------------------------------------------------------------------------------------
bool read_wav_file(const string& filename, WAVHeader& header, vector<short>& samples) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Erro: não foi possível abrir " << filename << endl;
        return false;
    }
    
    // Ler RIFF header
    file.read(header.riff, 4);
    file.read(reinterpret_cast<char*>(&header.file_size), 4);
    file.read(header.wave, 4);
    
    // Validar RIFF/WAVE
    if (strncmp(header.riff, "RIFF", 4) != 0 || strncmp(header.wave, "WAVE", 4) != 0) {
        cerr << "Erro: formato WAV inválido" << endl;
        return false;
    }
    
    // Procurar chunk "fmt "
    char chunk_id[4];
    int chunk_size;
    bool found_fmt = false;
    
    while (file.read(chunk_id, 4)) {
        file.read(reinterpret_cast<char*>(&chunk_size), 4);
        
        if (strncmp(chunk_id, "fmt ", 4) == 0) {
            found_fmt = true;
            file.read(reinterpret_cast<char*>(&header.audio_format), 2);
            file.read(reinterpret_cast<char*>(&header.num_channels), 2);
            file.read(reinterpret_cast<char*>(&header.sample_rate), 4);
            file.read(reinterpret_cast<char*>(&header.byte_rate), 4);
            file.read(reinterpret_cast<char*>(&header.block_align), 2);
            file.read(reinterpret_cast<char*>(&header.bits_per_sample), 2);
            header.fmt_size = chunk_size;
            
            // Pular bytes extras do fmt se houver
            if (chunk_size > 16) {
                file.seekg(chunk_size - 16, ios::cur);
            }
            break;
        } else {
            // Pular este chunk
            file.seekg(chunk_size, ios::cur);
        }
    }
    
    if (!found_fmt) {
        cerr << "Erro: chunk 'fmt ' não encontrado" << endl;
        return false;
    }
    
    if (header.num_channels != 1) {
        cerr << "Erro: apenas suportado áudio mono (1 canal)" << endl;
        return false;
    }
    
    if (header.bits_per_sample != 16) {
        cerr << "Erro: apenas suportado 16 bits por amostra" << endl;
        return false;
    }
    
    // Procurar chunk "data"
    bool found_data = false;
    file.clear();  // Limpar possíveis flags de erro
    file.seekg(12, ios::beg);  // Voltar ao início dos chunks (depois de RIFF/size/WAVE)
    
    while (file.read(chunk_id, 4)) {
        file.read(reinterpret_cast<char*>(&chunk_size), 4);
        
        if (strncmp(chunk_id, "data", 4) == 0) {
            found_data = true;
            memcpy(header.data, "data", 4);
            header.data_size = chunk_size;
            
            // Ler amostras
            int num_samples = chunk_size / 2;  // 2 bytes por amostra (16-bit)
            samples.resize(num_samples);
            file.read(reinterpret_cast<char*>(samples.data()), chunk_size);
            break;
        } else {
            // Pular este chunk
            file.seekg(chunk_size, ios::cur);
        }
    }
    
    if (!found_data) {
        cerr << "Erro: chunk 'data' não encontrado" << endl;
        return false;
    }
    
    file.close();
    return true;
}

//-------------------------------------------------------------------------------------------
// Escrita de ficheiro WAV
//-------------------------------------------------------------------------------------------
bool write_wav_file(const string& filename, const WAVHeader& header, const vector<short>& samples) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Erro: não foi possível criar " << filename << endl;
        return false;
    }
    
    // Escrever cabeçalho
    file.write(reinterpret_cast<const char*>(&header), sizeof(WAVHeader));
    
    // Escrever amostras
    file.write(reinterpret_cast<const char*>(samples.data()), samples.size() * sizeof(short));
    
    file.close();
    return true;
}

//-------------------------------------------------------------------------------------------
// ENCODER - Codifica ficheiro WAV para formato .dct
//-------------------------------------------------------------------------------------------
bool DCTCodec::encode(const string& input_file, const string& output_file) {
    // Ler ficheiro WAV
    WAVHeader header;
    vector<short> samples;
    
    if (!read_wav_file(input_file, header, samples)) {
        return false;
    }
    
    cout << "Codificando: " << input_file << endl;
    cout << "  Sample rate: " << header.sample_rate << " Hz" << endl;
    cout << "  Amostras: " << samples.size() << endl;
    cout << "  Tamanho do bloco: " << m_block_size << endl;
    cout << "  Coeficientes guardados: " << m_num_coeffs << endl;
    cout << "  Fator de quantização: " << m_quantization_factor << endl;
    
    // Abrir ficheiro de saída com BitStream
    fstream output(output_file, ios::out | ios::binary);
    if (!output.is_open()) {
        cerr << "Erro: não foi possível criar " << output_file << endl;
        return false;
    }
    
    BitStream bs(output, STREAM_WRITE);
    
    // Escrever cabeçalho do formato .dct
    bs.write_n_bits(m_block_size, 16);           // Tamanho do bloco
    bs.write_n_bits(m_num_coeffs, 16);           // Número de coeficientes
    bs.write_n_bits(m_quantization_factor, 16);  // Fator de quantização
    bs.write_n_bits(header.sample_rate, 32);     // Sample rate
    bs.write_n_bits(samples.size(), 32);         // Número total de amostras
    
    // Processar blocos
    int num_blocks = (samples.size() + m_block_size - 1) / m_block_size;
    int total_bits = 0;
    
    for (int b = 0; b < num_blocks; b++) {
        // Extrair bloco (padding com zeros se necessário)
        vector<short> block(m_block_size, 0);
        int start = b * m_block_size;
        int end = min(start + m_block_size, static_cast<int>(samples.size()));
        
        for (int i = start; i < end; i++) {
            block[i - start] = samples[i];
        }
        
        // Calcular e remover componente DC (média do bloco)
        double dc_mean = 0.0;
        int actual_samples = end - start;
        for (int i = 0; i < actual_samples; i++) {
            dc_mean += block[i];
        }
        dc_mean /= actual_samples;
        
        // Remover DC para melhorar a eficiência da DCT
        vector<short> zero_mean_block(m_block_size, 0);
        for (int i = 0; i < actual_samples; i++) {
            zero_mean_block[i] = block[i] - static_cast<short>(round(dc_mean));
        }
        
        // Guardar DC offset (16 bits com sinal)
        int dc_offset = static_cast<int>(round(dc_mean));
        int dc_sign = (dc_offset < 0) ? 1 : 0;
        int dc_magnitude = abs(dc_offset);
        bs.write_bit(dc_sign);
        bs.write_n_bits(dc_magnitude, 15);
        total_bits += 16;
        
        // Aplicar DCT no sinal com média zero
        vector<double> dct_coeffs = apply_dct(zero_mean_block);
        
        // Quantizar e escrever os primeiros k coeficientes
        for (int k = 0; k < m_num_coeffs; k++) {
            int quantized = quantize(dct_coeffs[k]);
            
            // Escrever sinal (1 bit) e magnitude (15 bits)
            // Isto permite valores de -32768 a 32767
            int sign = (quantized < 0) ? 1 : 0;
            int magnitude = abs(quantized);
            
            bs.write_bit(sign);
            bs.write_n_bits(magnitude, 15);
            total_bits += 16;
        }
    }
    
    bs.close();
    output.close();
    
    cout << "  Bits escritos: " << total_bits << endl;
    cout << "Ficheiro codificado: " << output_file << endl;
    
    return true;
}

//-------------------------------------------------------------------------------------------
// DECODER - Descodifica ficheiro .dct para WAV
//-------------------------------------------------------------------------------------------
bool DCTCodec::decode(const string& input_file, const string& output_file) {
    // Abrir ficheiro de entrada com BitStream
    fstream input(input_file, ios::in | ios::binary);
    if (!input.is_open()) {
        cerr << "Erro: não foi possível abrir " << input_file << endl;
        return false;
    }
    
    BitStream bs(input, STREAM_READ);
    
    // Ler cabeçalho
    int block_size = bs.read_n_bits(16);
    int num_coeffs = bs.read_n_bits(16);
    int quant_factor = bs.read_n_bits(16);
    int sample_rate = bs.read_n_bits(32);
    int total_samples = bs.read_n_bits(32);
    
    cout << "Descodificando: " << input_file << endl;
    cout << "  Sample rate: " << sample_rate << " Hz" << endl;
    cout << "  Amostras: " << total_samples << endl;
    cout << "  Tamanho do bloco: " << block_size << endl;
    cout << "  Coeficientes: " << num_coeffs << endl;
    cout << "  Fator de quantização: " << quant_factor << endl;
    
    // Atualizar parâmetros do codec
    m_block_size = block_size;
    m_num_coeffs = num_coeffs;
    m_quantization_factor = quant_factor;
    
    // Descodificar blocos
    vector<short> decoded_samples;
    int num_blocks = (total_samples + m_block_size - 1) / m_block_size;
    
    for (int b = 0; b < num_blocks; b++) {
        // Ler DC offset (16 bits: 1 bit sinal + 15 bits magnitude)
        int dc_sign = bs.read_bit();
        int dc_magnitude = bs.read_n_bits(15);
        int dc_offset = (dc_sign == 1) ? -dc_magnitude : dc_magnitude;
        
        // Ler coeficientes quantizados
        vector<double> coeffs(m_block_size, 0.0);
        
        for (int k = 0; k < m_num_coeffs; k++) {
            int sign = bs.read_bit();
            int magnitude = bs.read_n_bits(15);
            int quantized = (sign == 1) ? -magnitude : magnitude;
            
            // Dequantizar
            coeffs[k] = dequantize(quantized);
        }
        
        // Os restantes coeficientes ficam a zero (perda de informação)
        
        // Aplicar IDCT
        vector<short> block = apply_idct(coeffs);
        
        // Restaurar componente DC
        for (int i = 0; i < m_block_size; i++) {
            int restored_value = block[i] + dc_offset;
            
            // Limitar ao range de 16-bit signed
            if (restored_value > 32767) restored_value = 32767;
            if (restored_value < -32768) restored_value = -32768;
            
            block[i] = static_cast<short>(restored_value);
        }
        
        // Adicionar ao vetor de saída
        for (int i = 0; i < m_block_size && decoded_samples.size() < static_cast<size_t>(total_samples); i++) {
            decoded_samples.push_back(block[i]);
        }
    }
    
    bs.close();
    input.close();
    
    // Criar cabeçalho WAV
    WAVHeader header;
    memcpy(header.riff, "RIFF", 4);
    memcpy(header.wave, "WAVE", 4);
    memcpy(header.fmt, "fmt ", 4);
    memcpy(header.data, "data", 4);
    
    header.fmt_size = 16;
    header.audio_format = 1;  // PCM
    header.num_channels = 1;  // Mono
    header.sample_rate = sample_rate;
    header.bits_per_sample = 16;
    header.byte_rate = sample_rate * header.num_channels * header.bits_per_sample / 8;
    header.block_align = header.num_channels * header.bits_per_sample / 8;
    header.data_size = decoded_samples.size() * sizeof(short);
    header.file_size = 36 + header.data_size;
    
    // Escrever ficheiro WAV
    if (!write_wav_file(output_file, header, decoded_samples)) {
        return false;
    }
    
    cout << "Ficheiro descodificado: " << output_file << endl;
    
    return true;
}
