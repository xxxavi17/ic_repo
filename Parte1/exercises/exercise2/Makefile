# Makefile for Exercise 2 - WAV Quantizer
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
SOURCES = $(SRC_DIR)/wav_quant.cpp $(SRC_DIR)/audio_analyzer.cpp

# Target executables
TARGETS = $(BIN_DIR)/wav_quant $(BIN_DIR)/audio_analyzer

# Default target
all: $(TARGETS)

# Create directories if they don't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(RESULTS_DIR):
	mkdir -p $(RESULTS_DIR)

# Main program
$(BIN_DIR)/wav_quant: $(SRC_DIR)/wav_quant.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

# Audio analyzer
$(BIN_DIR)/audio_analyzer: $(SRC_DIR)/audio_analyzer.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

# Test targets
test: $(TARGETS) | $(RESULTS_DIR)
	@echo "Testing WAV Quantizer with different bit depths..."
	@if [ -f "../exercise1/results/sample.wav" ]; then \
		echo "Using sample.wav from exercise1..."; \
		cp ../exercise1/results/sample.wav $(RESULTS_DIR)/; \
		cd $(RESULTS_DIR) && ../bin/wav_quant sample.wav sample_8bit.wav 8 -v; \
		cd $(RESULTS_DIR) && ../bin/wav_quant sample.wav sample_4bit.wav 4 -v; \
		cd $(RESULTS_DIR) && ../bin/wav_quant sample.wav sample_2bit.wav 2 -v; \
		cd $(RESULTS_DIR) && ../bin/wav_quant sample.wav sample_1bit.wav 1 -v; \
	else \
		echo "sample.wav not found in exercise1. Please provide a test WAV file."; \
	fi

# Clean compiled files
clean:
	rm -f $(BIN_DIR)/*
	rm -f $(RESULTS_DIR)/*.wav
	rm -f $(RESULTS_DIR)/*.txt
	rm -f $(RESULTS_DIR)/*.png

# Clean all generated files
distclean: clean
	rm -rf $(BIN_DIR)
	rm -rf $(RESULTS_DIR)

# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Build all executables"
	@echo "  test       - Build and run tests with sample audio"
	@echo "  clean      - Remove compiled files and results"
	@echo "  distclean  - Remove all generated directories"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Usage examples:"
	@echo "  make"
	@echo "  make test"
	@echo "  ./bin/wav_quant input.wav output.wav 8 -v"

.PHONY: all test clean distclean help