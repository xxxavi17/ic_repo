all:
	@(if [ ! -d build ]; then mkdir build; fi; cd build; cmake ..; make)

clean:
	rm -rf build ../bin

test: all
	@echo "Testing with sample audio file..."
	@cd ../results && ../../sndfile-example/bin/wav_cp ../../sndfile-example/test/sample.wav test_sample.wav
	@cd ../results && ../bin/wav_hist_extended test_sample.wav 1 -v -save -plot

.PHONY: all clean test