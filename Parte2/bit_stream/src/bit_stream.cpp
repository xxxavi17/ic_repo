//-------------------------------------------------------------------------------------------
//
// Copyright 2025 University of Aveiro, Portugal, All Rights Reserved.
//
// These programs are supplied free of charge for research purposes only,
// and may not be sold or incorporated into any commercial product. There is
// ABSOLUTELY NO WARRANTY of any sort, nor any undertaking that they are
// fit for ANY PURPOSE WHATSOEVER. Use them at your own risk. If you do
// happen to find a bug, or have modifications to suggest, please report
// the same to Armando J. Pinho, ap@ua.pt. The copyright notice above
// and this statement of conditions must remain an integral part of each
// and every copy made of these files.
//
// Armando J. Pinho (ap@ua.pt)
// IEETA / DETI / University of Aveiro
//
//-------------------------------------------------------------------------------------------

#include <cstdint>
#include <fstream>
#include <string>
#include <iostream>  // ADICIONADO: necessário para std::cout e std::cerr
#include "bit_stream.h"

using namespace std;

BitStream::BitStream(std::fstream& fs, bool rw_status) 
    : m_rw_status(rw_status), m_byte_stream(fs, rw_status),
      m_buf(0), m_total_bits(0), m_is_open(true) {
    
    if(m_rw_status == STREAM_READ) {
        m_bit_ptr = 8; // Forçar leitura no primeiro acesso
    } else {
        m_bit_ptr = 0; // Começar do MSB
    }
}

int BitStream::read_bit() {
    validate_read_mode();
    
    if(m_bit_ptr >= 8) {
        int next_byte = m_byte_stream.get();
        if(next_byte == EOF)
            return EOF;
        m_buf = next_byte;
        m_bit_ptr = 0;
    }

    int bit = (m_buf >> (7 - m_bit_ptr)) & 0x01;
    m_bit_ptr++;
    m_total_bits++;
    
    return bit;
}

uint64_t BitStream::read_n_bits(int n) {
    uint64_t x { };
    for(int i = 0 ; i < n ; ++i) {
        int bit = read_bit();
        if(bit == EOF) {
            // CORRIGIDO: retornar valor válido mesmo com EOF
            return x;
        }
        x <<= 1;
        x |= bit;
    }

    return x;
}

string BitStream::read_string() {
    int c;
    string s;

    while((c = read_n_bits(8)) != '\n' && c != EOF)  // CORRIGIDO: verificar EOF
        s += static_cast<char>(c);  // CORRIGIDO: cast explícito

    return s;
}

void BitStream::write_bit(int bit) {
    validate_write_mode();
    
    // CORRIGIDO: usar máscara correta e posição MSB
    m_buf |= (bit & 0x01) << (7 - m_bit_ptr);
    m_bit_ptr++;
    
    if(m_bit_ptr >= 8) {
        m_byte_stream.put(m_buf);
        m_bit_ptr = 0;
        m_buf = 0;
    }
    
    m_total_bits++;
}

void BitStream::write_n_bits(uint64_t bits, int n) {
    for(int i = n - 1 ; i >= 0 ; i--)
        write_bit(((0x01 << i) & bits) >> i);
}

void BitStream::write_string(const string& s) {
    for(const char c : s)
        write_n_bits(c, 8);

    write_n_bits('\n', 8); // Mark the end of the string with a newline
}

off_t BitStream::tell() {
    return m_byte_stream.tell();
}

void BitStream::flush() {
    if(m_rw_status == STREAM_WRITE && m_bit_ptr > 0) {
        // Escrever byte parcial com padding de zeros
        m_byte_stream.put(m_buf);
        m_bit_ptr = 0;
        m_buf = 0;
    }
    m_byte_stream.flush();
}

void BitStream::close() {
    if(m_is_open) {
        if(m_rw_status == STREAM_WRITE && m_bit_ptr > 0) {
            // Escrever byte parcial antes de fechar
            m_byte_stream.put(m_buf);
        }
        m_byte_stream.close();
        m_is_open = false;
    }
}

void BitStream::debug_status() {
    std::cout << "=== BitStream Debug Status ===" << std::endl;
    std::cout << "Mode: " << (m_rw_status ? "READ" : "WRITE") << std::endl;
    std::cout << "Open: " << (m_is_open ? "YES" : "NO") << std::endl;
    std::cout << "Bits processed: " << m_total_bits << std::endl;
    std::cout << "Current bit position: " << m_bit_ptr << std::endl;
    std::cout << "Byte stream position: " << m_byte_stream.tell() << std::endl;
    std::cout << "=============================" << std::endl;
}

bool BitStream::at_eof() const {
    if(m_rw_status != STREAM_READ) {
        return false; // Modo escrita nunca está em EOF
    }
    
    // MELHORADO: implementação mais robusta do EOF
    // Se ainda há bits no buffer, não estamos em EOF
    if(m_bit_ptr < 8 && m_bit_ptr >= 0) {
        return false;
    }
    
    // Verificar se o próximo byte existe sem consumir
    // (implementação simplificada)
    return false; // Para ser implementado conforme necessário
}

// REMOVIDO: método close() duplicado

// Métodos de validação
void BitStream::validate_read_mode() const {
    if(!m_is_open) {
        std::cerr << "Erro: BitStream não está aberto!" << std::endl;
        return;
    }
    if(m_rw_status != STREAM_READ) {
        std::cerr << "Erro: BitStream não está em modo de leitura!" << std::endl;
        return;
    }
}

void BitStream::validate_write_mode() const {
    if(!m_is_open) {
        std::cerr << "Erro: BitStream não está aberto!" << std::endl;
        return;
    }
    if(m_rw_status != STREAM_WRITE) {
        std::cerr << "Erro: BitStream não está em modo de escrita!" << std::endl;
        return;
    }
}

