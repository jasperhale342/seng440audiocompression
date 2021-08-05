output: audio_compression.c 
	gcc audio_compression.c -o audio_compression
run: audio_compression haazah.wav
	./audio_compression haazah.wav
clean: 
	rm audio_compression
