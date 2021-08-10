output: audio_compression.c 
	gcc audio_compression.c -o audio_compression.exe
	gcc audio_compression_optimized_c_code.c -o audio_compression_optimized_c_code.exe
run: audio_compression.exe input.wav
	./audio_compression.exe input.wav
	./audio_compression_optimized_c_code.exe input.wav
	hexdump -C input.wav > inputDump.txt
	hexdump -C input_compressed.wav > compressedDump.txt
	hexdump -C input_decompressed.wav > decompressedDump.txt
clean: 
	rm audio_compression.exe
	rm audio_compression_optimized_c_code.exe
	rm input_compressed.wav
	rm input_decompressed.wav
	rm inputDump.txt
	rm compressedDump.txt
	rm decompressedDump.txt
