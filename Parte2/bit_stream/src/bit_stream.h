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

#ifndef BIT_STREAM_H
#define BIT_STREAM_H

#include <string>
#include <fstream>
#include "byte_stream.h"

class BitStream {
  private:
	bool		m_rw_status { STREAM_READ };		//indica se o stream esta em modo leitura(true) ou escrita(false)
	int			m_buf;								//buffer interno para armazenar bits temporariamente
	int			m_bit_ptr;							//ponteiro que indica a posição atual do bit dentro do buffer
	ByteStream	m_byte_stream;						// instancia de ByteStream que gere o acesso aos bytes subjacentes

	//contador de bits processados
	uint64_t	m_total_bits;
	//flag para verificar se o stream esta aberto
	bool		m_is_open;

  public:
	BitStream(std::fstream& fs, bool rw_status);	//inputs: filestream e modo de operaçao

	BitStream() = delete;
	BitStream(const BitStream&) = delete;
	BitStream(BitStream&&) = delete;
	BitStream& operator=(BitStream&&) = delete;
	BitStream& operator=(const BitStream&) = delete;

	int read_bit();					//le um unico bit
	uint64_t read_n_bits(int n);	//le 'n' bits e retorna como uint64_t
	std::string read_string();		//le uma string codificada bit a bit
	void write_bit(int bit);		//escreve um unico bit
	void write_n_bits(uint64_t bits, int n);		//escreve 'n' bits de um valor
	void write_string(const std::string& s);		//escreve uma string
	off_t tell();									//retorna a posiçao atual no stream
	void close();									//fecha o stream

	bool is_open() const {return m_is_open; }		//metodo para verificar se o stream esta aberto
	uint64_t get_total_bits() const {return m_total_bits; }		//metodo para obter as estatisticas de uso

	//metodo para forçar o flush de dados pendentes
	void flush();

	//metodo para escrever um byte completo
	void write_byte(uint8_t byte) {write_n_bits(byte, 8); }

	//metodo para ler um byte completo
	uint8_t read_byte() { return static_cast<uint8_t>(read_n_bits(8)); }

	//metodo para dar rest as estatisticas
	void reset_stats() {m_total_bits = 0; }

	//metodo de debug- mostra estado atual
	void debug_status();

	//metodo para verificar se chegou ao fim do ficheiro
	bool at_eof() const;

	private:
    
    // Atualizar o contador de bits processados
    void update_bit_counter(int bits_count) { m_total_bits += bits_count; }
    
    // Validar se o stream está em modo correto
    void validate_read_mode() const;
    void validate_write_mode() const;
};

#endif

