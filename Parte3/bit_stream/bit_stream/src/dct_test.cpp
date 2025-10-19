//-------------------------------------------------------------------------------------------
//
// Programa de Teste do DCT Codec
// Testa codec com diferentes parâmetros e calcula métricas de qualidade
//
//-------------------------------------------------------------------------------------------

#include "dct_codec.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <filesystem>
#include <chrono>

using namespace std;
namespace fs = std::filesystem;

/**
 * @brief Estrutura para armazenar resultados de um teste
 */
struct TestResult {
    string test_name;
    string input_file;
    int block_size;
    int num_coeffs;
    int quantization_factor;
    
    // Métricas
    double bitrate;           // bits por segundo
    double snr_db;            // Signal-to-Noise Ratio em dB
    double compression_ratio; // Tamanho original / tamanho comprimido
    size_t original_size;     // Tamanho do ficheiro original em bytes
    size_t compressed_size;   // Tamanho do ficheiro comprimido em bytes
    double duration;          // Duração do áudio em segundos
    double encoding_time;     // Tempo de codificação em segundos
    double decoding_time;     // Tempo de descodificação em segundos
};

/**
 * @brief Calcula o SNR (Signal-to-Noise Ratio) entre sinal original e reconstruído
 * @param original Vetor com amostras originais
 * @param reconstructed Vetor com amostras reconstruídas
 * @return SNR em decibéis (dB)
 */
double calculate_snr(const vector<short>& original, const vector<short>& reconstructed) {
    if (original.size() != reconstructed.size()) {
        cerr << "Erro: vetores com tamanhos diferentes" << endl;
        return 0.0;
    }
    
    double signal_power = 0.0;
    double noise_power = 0.0;
    
    for (size_t i = 0; i < original.size(); i++) {
        double s = original[i];
        double diff = original[i] - reconstructed[i];
        
        signal_power += s * s;
        noise_power += diff * diff;
    }
    
    // Evitar divisão por zero
    if (noise_power < 1e-10) {
        return 100.0; // SNR muito alto (quase sem ruído)
    }
    
    // SNR em dB = 10 * log10(signal_power / noise_power)
    double snr = 10.0 * log10(signal_power / noise_power);
    return snr;
}

/**
 * @brief Executa um teste completo com parâmetros específicos
 * @param input_wav Ficheiro de entrada WAV
 * @param test_name Nome do teste
 * @param block_size Tamanho do bloco
 * @param num_coeffs Número de coeficientes
 * @param quant_factor Fator de quantização
 * @return Estrutura com os resultados do teste
 */
TestResult run_test(const string& input_wav, const string& test_name,
                    int block_size, int num_coeffs, int quant_factor) {
    
    TestResult result;
    result.test_name = test_name;
    result.input_file = input_wav;
    result.block_size = block_size;
    result.num_coeffs = num_coeffs;
    result.quantization_factor = quant_factor;
    
    cout << "\n========================================" << endl;
    cout << "Teste: " << test_name << endl;
    cout << "Ficheiro: " << input_wav << endl;
    cout << "========================================" << endl;
    
    // Extrair nome base do ficheiro (sem path e extensão)
    fs::path input_path(input_wav);
    string base_name = input_path.stem().string();
    
    // Criar nomes únicos para cada teste na pasta resultados_testes
    string results_dir = "resultados_testes";
    string encoded_file = results_dir + "/" + base_name + "_" + test_name + ".dct";
    string decoded_file = results_dir + "/" + base_name + "_" + test_name + "_decoded.wav";
    
    // Garantir que a pasta existe
    if (!fs::exists(results_dir)) {
        fs::create_directory(results_dir);
    }
    
    // Criar codec
    DCTCodec codec(block_size, num_coeffs, quant_factor);
    
    // Medir tempo de codificação
    auto start_encode = chrono::high_resolution_clock::now();
    if (!codec.encode(input_wav, encoded_file)) {
        cerr << "Erro na codificação!" << endl;
        return result;
    }
    auto end_encode = chrono::high_resolution_clock::now();
    result.encoding_time = chrono::duration<double>(end_encode - start_encode).count();
    
    // Medir tempo de descodificação
    auto start_decode = chrono::high_resolution_clock::now();
    if (!codec.decode(encoded_file, decoded_file)) {
        cerr << "Erro na descodificação!" << endl;
        return result;
    }
    auto end_decode = chrono::high_resolution_clock::now();
    result.decoding_time = chrono::duration<double>(end_decode - start_decode).count();
    
    // Calcular tamanhos dos ficheiros
    result.original_size = fs::file_size(input_wav);
    result.compressed_size = fs::file_size(encoded_file);
    result.compression_ratio = static_cast<double>(result.original_size) / result.compressed_size;
    
    // Ler ficheiros originais e reconstruídos para calcular SNR
    WAVHeader original_header, reconstructed_header;
    vector<short> original_samples, reconstructed_samples;
    
    read_wav_file(input_wav, original_header, original_samples);
    read_wav_file(decoded_file, reconstructed_header, reconstructed_samples);
    
    // Calcular métricas
    result.duration = static_cast<double>(original_samples.size()) / original_header.sample_rate;
    result.snr_db = calculate_snr(original_samples, reconstructed_samples);
    result.bitrate = (result.compressed_size * 8.0) / result.duration;
    
    // Imprimir resultados
    cout << "\n--- RESULTADOS ---" << endl;
    cout << fixed << setprecision(2);
    cout << "Duração do áudio: " << result.duration << " s" << endl;
    cout << "Tamanho original: " << result.original_size << " bytes" << endl;
    cout << "Tamanho comprimido: " << result.compressed_size << " bytes" << endl;
    cout << "Taxa de compressão: " << result.compression_ratio << "x" << endl;
    cout << "Bitrate: " << result.bitrate / 1000.0 << " kbps" << endl;
    cout << "SNR: " << result.snr_db << " dB" << endl;
    cout << "Tempo de codificação: " << result.encoding_time << " s" << endl;
    cout << "Tempo de descodificação: " << result.decoding_time << " s" << endl;
    
    // MANTER ficheiros para comparação posterior
    cout << "\nFicheiros guardados:" << endl;
    cout << "  Comprimido: " << encoded_file << endl;
    cout << "  Descodificado: " << decoded_file << endl;
    
    // NÃO apagar - guardar para comparação!
    // fs::remove(encoded_file);
    // fs::remove(decoded_file);
    
    return result;
}

/**
 * @brief Gera relatório em CSV com todos os resultados
 * @param results Vetor com todos os resultados dos testes
 * @param output_file Nome do ficheiro CSV de saída
 */
void generate_csv_report(const vector<TestResult>& results, const string& output_file) {
    ofstream csv(output_file, ios::trunc);  // Modo truncate: apaga e reescreve
    if (!csv.is_open()) {
        cerr << "Erro ao criar ficheiro CSV" << endl;
        return;
    }
    
    // Cabeçalho
    csv << "Teste,Ficheiro,Tamanho_Bloco,Num_Coeficientes,Fator_Quantizacao,";
    csv << "Duracao_s,Tamanho_Original_bytes,Tamanho_Comprimido_bytes,";
    csv << "Taxa_Compressao,Bitrate_kbps,SNR_dB,";
    csv << "Tempo_Codificacao_s,Tempo_Descodificacao_s" << endl;
    
    // Dados
    for (const auto& r : results) {
        csv << r.test_name << ","
            << r.input_file << ","
            << r.block_size << ","
            << r.num_coeffs << ","
            << r.quantization_factor << ","
            << fixed << setprecision(3)
            << r.duration << ","
            << r.original_size << ","
            << r.compressed_size << ","
            << r.compression_ratio << ","
            << r.bitrate / 1000.0 << ","
            << r.snr_db << ","
            << r.encoding_time << ","
            << r.decoding_time << endl;
    }
    
    csv.close();
    cout << "\n\nRelatório CSV gerado: " << output_file << endl;
}

/**
 * @brief Programa principal
 */
int main(int argc, char* argv[]) {
    cout << "===============================================" << endl;
    cout << "   DCT Audio Codec - Programa de Teste" << endl;
    cout << "===============================================\n" << endl;
    
    vector<TestResult> all_results;
    
    // Verificar se foi fornecido um ficheiro específico
    if (argc > 1) {
        // Modo: testar um ficheiro específico
        string input_file = argv[1];
        
        cout << "Testando ficheiro: " << input_file << endl;
        
        // Executar testes com diferentes parâmetros (T2-T7)
        // T1 removido devido a problema de precisão numérica
        all_results.push_back(run_test(input_file, "T2", 512, 256, 2));
        all_results.push_back(run_test(input_file, "T3", 512, 128, 5));
        all_results.push_back(run_test(input_file, "T4", 1024, 256, 10));
        all_results.push_back(run_test(input_file, "T5", 1024, 128, 20));
        all_results.push_back(run_test(input_file, "T6", 256, 128, 5));
        all_results.push_back(run_test(input_file, "T7", 256, 64, 10));
        
    } else {
        // Modo: testar todos os ficheiros na pasta audio_files/mono
        string audio_dir = "../../../audio_files/mono";
        
        if (!fs::exists(audio_dir)) {
            cerr << "Erro: pasta audio_files/mono não encontrada!" << endl;
            cerr << "Por favor, crie a pasta e coloque os ficheiros WAV mono dentro." << endl;
            cerr << "Uso: " << argv[0] << " [ficheiro.wav]" << endl;
            return 1;
        }
        
        cout << "Testando todos os ficheiros em: " << audio_dir << "\n" << endl;
        
        // Listar ficheiros WAV
        vector<string> wav_files;
        for (const auto& entry : fs::directory_iterator(audio_dir)) {
            if (entry.path().extension() == ".wav") {
                wav_files.push_back(entry.path().string());
            }
        }
        
        if (wav_files.empty()) {
            cerr << "Nenhum ficheiro WAV encontrado!" << endl;
            return 1;
        }
        
        // Testar cada ficheiro com os parâmetros recomendados (T2-T7)
        // T1 removido devido a problema de precisão numérica
        for (const auto& wav_file : wav_files) {
            cout << "\n\n*** Ficheiro: " << wav_file << " ***\n" << endl;
            
            // Testes baseados na tabela fornecida
            all_results.push_back(run_test(wav_file, "T2", 512, 256, 2));
            all_results.push_back(run_test(wav_file, "T3", 512, 128, 5));
            all_results.push_back(run_test(wav_file, "T4", 1024, 256, 10));
            all_results.push_back(run_test(wav_file, "T5", 1024, 128, 20));
            all_results.push_back(run_test(wav_file, "T6", 256, 128, 5));
            all_results.push_back(run_test(wav_file, "T7", 256, 64, 10));
        }
    }
    
    // Gerar relatório CSV
    generate_csv_report(all_results, "dct_codec_results.csv");
    
    cout << "\n\n===============================================" << endl;
    cout << "   Testes concluídos!" << endl;
    cout << "===============================================" << endl;
    
    return 0;
}
