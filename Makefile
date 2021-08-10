output: audio_compression.c 
	gcc audio_compression.c -o audio_compression
run: audio_compression input.wav
	./audio_compression input.wav
	hexdump -C input.wav > inputDump.txt
	hexdump -C input_compressed.wav > compressedDump.txt
	hexdump -C input_decompressed.wav > decompressedDump.txt
clean: 
	rm audio_compression
	rm input_compressed.wav
	rm input_decompressed.wav
	rm inputDump.txt
	rm compressedDump.txt
	rm decompressedDump.txt
