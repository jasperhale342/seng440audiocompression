output: audio_compression.c audio_compression_optimized_c_code.c
	gcc audio_compression.c -o audio_compression
	gcc audio_compression_optimized_c_code.c -o audio_compression_optimized_c_code
run: audio_compression audio_compression_optimized_c_code input.wav
	./audio_compression input.wav
	./audio_compression_optimized_c_code input.wav
	hexdump -C input.wav > inputDump.txt
	hexdump -C input_compressed.wav > compressedDump.txt
	hexdump -C input_decompressed.wav > decompressedDump.txt
clean: 
	rm audio_compression
	rm audio_compression_optimized_c_code
	rm input_compressed.wav
	rm input_decompressed.wav
	rm inputDump.txt
	rm compressedDump.txt
	rm decompressedDump.txt
