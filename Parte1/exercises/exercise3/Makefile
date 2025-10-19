# Makefile for Exercise 3 - WAV Comparator
# Author: Xavier
# Date: October 2025

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -O3
LIBS = -lsndfile

# Directories
SRC_DIR = src
BIN_DIR = bin
RESULTS_DIR = results

# Source files
SOURCES = $(SRC_DIR)/wav_cmp.cpp

# Target executables
TARGETS = $(BIN_DIR)/wav_cmp

# Default target
all: $(TARGETS)

# Create directories if they don't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(RESULTS_DIR):
	mkdir -p $(RESULTS_DIR)

# Main program
$(BIN_DIR)/wav_cmp: $(SRC_DIR)/wav_cmp.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

# Test targets
test: $(TARGETS) | $(RESULTS_DIR)
	@echo "Testing WAV Comparator with quantized files from exercise2..."
	@if [ -f "../exercise2/results/sample.wav" ] && [ -f "../exercise2/results/sample_8bit.wav" ]; then \
		echo "Copying test files from exercise2..."; \
		cp ../exercise2/results/sample*.wav $(RESULTS_DIR)/; \
		echo ""; \
		echo "=== Comparing Original vs 8-bit Quantized ==="; \
		cd $(RESULTS_DIR) && ../bin/wav_cmp sample.wav sample_8bit.wav -v -s; \
		echo ""; \
		echo "=== Comparing Original vs 4-bit Quantized ==="; \
		cd $(RESULTS_DIR) && ../bin/wav_cmp sample.wav sample_4bit.wav -v -s; \
		echo ""; \
		echo "=== Comparing Original vs 2-bit Quantized ==="; \
		cd $(RESULTS_DIR) && ../bin/wav_cmp sample.wav sample_2bit.wav -v -s; \
		echo ""; \
		echo "=== Comparing Original vs 1-bit Quantized ==="; \
		cd $(RESULTS_DIR) && ../bin/wav_cmp sample.wav sample_1bit.wav -v -s; \
	else \
		echo "Test files not found in exercise2/results. Please run exercise2 first."; \
	fi

# Quick test with just one comparison
quick-test: $(TARGETS) | $(RESULTS_DIR)
	@echo "Quick test of WAV Comparator..."
	@if [ -f "../exercise2/results/sample.wav" ] && [ -f "../exercise2/results/sample_8bit.wav" ]; then \
		cp ../exercise2/results/sample.wav ../exercise2/results/sample_8bit.wav $(RESULTS_DIR)/; \
		cd $(RESULTS_DIR) && ../bin/wav_cmp sample.wav sample_8bit.wav -v; \
	else \
		echo "Test files not found. Please run exercise2 first."; \
	fi

# Clean compiled files
clean:
	rm -f $(BIN_DIR)/*
	rm -f $(RESULTS_DIR)/*.wav
	rm -f $(RESULTS_DIR)/*.md
	rm -f $(RESULTS_DIR)/*.txt
	rm -f $(RESULTS_DIR)/*.png

# Clean all generated files
distclean: clean
	rm -rf $(BIN_DIR)
	rm -rf $(RESULTS_DIR)

# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Build wav_cmp executable"
	@echo "  test       - Build and run comprehensive tests with exercise2 files"
	@echo "  quick-test - Build and run quick test with 8-bit comparison"
	@echo "  clean      - Remove compiled files and results"
	@echo "  distclean  - Remove all generated directories"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Usage examples:"
	@echo "  make"
	@echo "  make test"
	@echo "  ./bin/wav_cmp original.wav compared.wav -v -s"

.PHONY: all test quick-test clean distclean help