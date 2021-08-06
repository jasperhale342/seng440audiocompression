output: audio_compression.c 
	gcc audio_compression.c -o audio_compression
run: audio_compression input.wav
	./audio_compression input.wav
	hexdump -C input.wav > inDump.txt
	hexdump -C new_wav.wav > outDump.txt
clean: 
	rm audio_compression
	rm new_wav.wav
	rm inDump.txt
	rm outDump.txt
