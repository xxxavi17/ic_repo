# WAV Effects Processor Makefile
# Author: Xavier
# Exercise 4 - Audio Effects Implementation

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -O3
DEBUGFLAGS = -g -O0 -DDEBUG

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin
RESULTS_DIR = results

# Libraries
LIBS = -lsndfile -lm

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
TARGET = $(BIN_DIR)/wav_effects

# Test files (will be copied from other exercises)
TEST_INPUT = $(RESULTS_DIR)/sample.wav
SAMPLE_8BIT = $(RESULTS_DIR)/sample_8bit.wav
SAMPLE_4BIT = $(RESULTS_DIR)/sample_4bit.wav

# Effect output files
ECHO_OUTPUT = $(RESULTS_DIR)/sample_echo.wav
MULTIECHO_OUTPUT = $(RESULTS_DIR)/sample_multiecho.wav
AMPLITUDE_OUTPUT = $(RESULTS_DIR)/sample_amplitude.wav
CHORUS_OUTPUT = $(RESULTS_DIR)/sample_chorus.wav
REVERB_OUTPUT = $(RESULTS_DIR)/sample_reverb.wav

# Default target
all: $(TARGET)

# Create directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(RESULTS_DIR):
	mkdir -p $(RESULTS_DIR)

# Compile object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link executable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LIBS)

# Debug build
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(TARGET)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Clean all generated files
cleanall: clean
	rm -rf $(RESULTS_DIR)

# Copy test files from other exercises
copy-test-files: | $(RESULTS_DIR)
	@echo "Copying test files from Exercise 2..."
	@if [ -f "../exercise2/results/sample.wav" ]; then \
		cp ../exercise2/results/sample.wav $(TEST_INPUT); \
		echo "Copied sample.wav"; \
	else \
		echo "Warning: sample.wav not found in Exercise 2"; \
	fi
	@if [ -f "../exercise2/results/sample_8bit.wav" ]; then \
		cp ../exercise2/results/sample_8bit.wav $(SAMPLE_8BIT); \
		echo "Copied sample_8bit.wav"; \
	fi
	@if [ -f "../exercise2/results/sample_4bit.wav" ]; then \
		cp ../exercise2/results/sample_4bit.wav $(SAMPLE_4BIT); \
		echo "Copied sample_4bit.wav"; \
	fi

# Test individual effects
test-echo: $(TARGET) copy-test-files
	@echo "Testing Echo effect..."
	$(TARGET) $(TEST_INPUT) $(ECHO_OUTPUT) echo 250 0.6
	@echo "Echo effect applied successfully!"

test-multiecho: $(TARGET) copy-test-files
	@echo "Testing Multi-Echo effect..."
	$(TARGET) $(TEST_INPUT) $(MULTIECHO_OUTPUT) multiecho 150 4 0.7
	@echo "Multi-Echo effect applied successfully!"

test-amplitude: $(TARGET) copy-test-files
	@echo "Testing Amplitude Modulation effect..."
	$(TARGET) $(TEST_INPUT) $(AMPLITUDE_OUTPUT) amplitude 8 0.8 0
	@echo "Amplitude Modulation effect applied successfully!"

test-chorus: $(TARGET) copy-test-files
	@echo "Testing Chorus effect..."
	$(TARGET) $(TEST_INPUT) $(CHORUS_OUTPUT) chorus 12 1.2 0.6 0.3 0.5
	@echo "Chorus effect applied successfully!"

test-reverb: $(TARGET) copy-test-files
	@echo "Testing Reverb effect..."
	$(TARGET) $(TEST_INPUT) $(REVERB_OUTPUT) reverb 0.7 0.4 0.4
	@echo "Reverb effect applied successfully!"

# Test all effects
test-all: test-echo test-multiecho test-amplitude test-chorus test-reverb
	@echo "All effects tested successfully!"
	@echo ""
	@echo "Generated files:"
	@ls -la $(RESULTS_DIR)/*.wav

# Test with quantized samples
test-quantized: $(TARGET) copy-test-files
	@echo "Testing effects with quantized samples..."
	@if [ -f "$(SAMPLE_8BIT)" ]; then \
		$(TARGET) $(SAMPLE_8BIT) $(RESULTS_DIR)/sample_8bit_reverb.wav reverb 0.5 0.3 0.3; \
		echo "Applied reverb to 8-bit sample"; \
	fi
	@if [ -f "$(SAMPLE_4BIT)" ]; then \
		$(TARGET) $(SAMPLE_4BIT) $(RESULTS_DIR)/sample_4bit_echo.wav echo 200 0.5; \
		echo "Applied echo to 4-bit sample"; \
	fi

# Show effect usage
usage: $(TARGET)
	$(TARGET)

# Performance test
perf-test: $(TARGET) copy-test-files
	@echo "Running performance tests..."
	@echo "Testing Echo effect performance:"
	@time $(TARGET) $(TEST_INPUT) $(RESULTS_DIR)/perf_echo.wav echo 300 0.7
	@echo ""
	@echo "Testing Reverb effect performance:"
	@time $(TARGET) $(TEST_INPUT) $(RESULTS_DIR)/perf_reverb.wav reverb 0.8 0.5 0.5

# Create analysis script
create-analysis: | $(RESULTS_DIR)
	@echo "Creating analysis script..."
	@echo "#!/usr/bin/env python3" > $(RESULTS_DIR)/analyze_effects.py
	@echo "# Audio Effects Analysis Script" >> $(RESULTS_DIR)/analyze_effects.py
	@echo "# This script will be implemented to analyze effect characteristics" >> $(RESULTS_DIR)/analyze_effects.py
	@chmod +x $(RESULTS_DIR)/analyze_effects.py

# Help target
help:
	@echo "WAV Effects Processor - Available targets:"
	@echo ""
	@echo "Build targets:"
	@echo "  all          - Build the wav_effects program"
	@echo "  debug        - Build with debug flags"
	@echo "  clean        - Remove build files"
	@echo "  cleanall     - Remove all generated files"
	@echo ""
	@echo "Test targets:"
	@echo "  copy-test-files  - Copy test files from Exercise 2"
	@echo "  test-echo        - Test echo effect"
	@echo "  test-multiecho   - Test multi-echo effect"
	@echo "  test-amplitude   - Test amplitude modulation effect"
	@echo "  test-chorus      - Test chorus effect"
	@echo "  test-reverb      - Test reverb effect"
	@echo "  test-all         - Test all effects"
	@echo "  test-quantized   - Test effects with quantized samples"
	@echo "  perf-test        - Run performance tests"
	@echo ""
	@echo "Utility targets:"
	@echo "  usage            - Show program usage"
	@echo "  create-analysis  - Create analysis script template"
	@echo "  help             - Show this help message"

# Declare phony targets
.PHONY: all debug clean cleanall copy-test-files test-echo test-multiecho test-amplitude test-chorus test-reverb test-all test-quantized usage perf-test create-analysis help