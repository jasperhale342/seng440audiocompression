#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <memory.h> 
#include <time.h>

typedef struct WAV_HEADER {
    uint32_t samples;
    uint32_t size_of_samples;


	uint8_t riff[4];						// RIFF string
	uint32_t file_size;				// overall size of file in bytes
	uint8_t file_type_header[4];						// WAVE string
	uint8_t format_chunk_marker[4];			// fmt string with trailing null char
	uint32_t format_data_length;					// length of the format data
	uint16_t format_type;					// format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
	uint16_t num_channel;						// no.of channels
	uint32_t sample_rate;					// sampling rate (blocks per second)
	uint32_t byterate;						// SampleRate * NumChannels * BitsPerSample/8
	uint32_t block_align;					// NumChannels * BitsPerSample/8
	uint16_t bits_per_sample;				// bits per sample, 8- 8bits, 16- 16 bits etc
	uint8_t data_header [4];		// DATA string or FLLR string
	uint32_t data_size;						// NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read


}  WAV_HEADER;

void get_wave_header_info(FILE * wav, WAV_HEADER* wav_info);
void write_header_encoded_file(WAV_HEADER * wav_info, FILE * output);
void write_header_decoded_file(FILE *input, FILE * output);
uint8_t get_sign(short buffer);
uint16_t get_magnitude(short sample);
uint8_t codeword_compression ( uint16_t sample_magnitude , uint8_t sign );
int16_t codeword_decompression(uint8_t codeword);
void encode_data(uint8_t * data_of_file , uint8_t * compressed_codeword, int arr_size);
void decode_data(uint8_t* compressed_codeword , int16_t* decompressed_codeword, int arr_size);
void write_encoded_data(uint8_t* compressed_codeword, WAV_HEADER* header, int arr_size);
void write_decoded_data(int16_t* decompressed_codeword, FILE* wav_file, int arr_size);
void read_data(FILE *wav_file, WAV_HEADER* wav_struct);

void encode_data(uint8_t * data_of_file , uint8_t * compressed_codeword, int arr_size){  
    short buffer;
    uint8_t sign;
    uint16_t magnitude;
    time_t  start = clock();
   
    int i; // sample array iterator
    int index;

    for ( i= 0; i < arr_size-1; i+=2)
    {
        index = i >> 1;
        buffer = (data_of_file[i] | (data_of_file[i+1] << 8));
        sign = get_sign(buffer);
        magnitude = get_magnitude(buffer);
        compressed_codeword[index] = codeword_compression(magnitude, sign);
        
    }

    time_t  stop = clock();
    double compression_time = (double) (stop - start) / CLOCKS_PER_SEC;
    
    printf("FILE COMPRESSED. time: %f\n", compression_time);
}
void decode_data(uint8_t* compressed_codeword , int16_t* decompressed_codeword, int arr_size){
    int16_t out_sample;
    uint8_t codeword;

    for(int i=0;i<arr_size;i++){
        codeword = (compressed_codeword[i] ^ 0x55);
        out_sample = codeword_decompression(codeword);
        decompressed_codeword[i] = out_sample;
    }
}
void write_encoded_data(uint8_t* compressed_codeword, WAV_HEADER* header, int arr_size){
    FILE * output = fopen("input_compressed.wav", "wb");
    write_header_encoded_file(header,output);

    for(int i=0;i<arr_size;i++){
        fwrite(&compressed_codeword[i],sizeof(uint8_t),1,output);
    }

    fclose(output);
}
void write_decoded_data(int16_t* decompressed_codeword, FILE* wav_file, int arr_size){
    FILE * output = fopen("input_decompressed.wav", "wb");
    write_header_decoded_file(wav_file,output);

    for(int i=0;i<arr_size;i++){
        fwrite(&decompressed_codeword[i],sizeof(int16_t),1,output);
    }

    fclose(output);
}
void read_data(FILE *wav_file, WAV_HEADER* wav_struct){
    int size = ((int)wav_struct->file_size+8-44); 
    int dec_size = (size>>1);
    uint8_t* data_of_file = (uint8_t*)malloc(sizeof(uint8_t) *size);
    fread(data_of_file, size, 1, wav_file); // read input data
    uint8_t* compressed_codeword = (uint8_t*)malloc(sizeof(uint8_t) * size);
    int16_t* decompressed_codeword = (int16_t*)malloc(sizeof(int16_t) * (dec_size));

    encode_data(data_of_file,compressed_codeword,size); //where compression is being timed
    write_encoded_data(compressed_codeword,wav_struct,size);
   
    decode_data(compressed_codeword , decompressed_codeword, dec_size);
    write_decoded_data(decompressed_codeword, wav_file, dec_size);
    
    free(data_of_file);
    free(compressed_codeword);
    free(decompressed_codeword);
    
}
void write_header_decoded_file(FILE *input, FILE * output){
    uint8_t buffer[44];
    fseek(input,0,SEEK_SET);
    fread(buffer, sizeof(buffer) ,1, input);
    fwrite(buffer, sizeof(buffer),1,output);

}
void write_header_encoded_file(WAV_HEADER* wav_info, FILE * output){   
    uint8_t buffer_2[2];
    uint8_t buffer_4[4];
    uint32_t new_file_size;
    uint32_t new_data_size;

    fseek(output,0,SEEK_SET);
    fwrite(&wav_info->riff, sizeof(wav_info->riff),1,output);
    
    //file size
    new_file_size = ((wav_info->file_size - 36) / 2) + 36;
    buffer_4[3] = new_file_size & 0xFF;
    buffer_4[2] = (new_file_size >> 8) & 0xFF;
    buffer_4[1] = (new_file_size >> 16) & 0xFF;
    buffer_4[0] = (new_file_size >> 24) & 0xFF;
    fwrite(&buffer_4[3], 1, 1, output);
    fwrite(&buffer_4[2], 1, 1, output);
    fwrite(&buffer_4[1], 1, 1, output);
    fwrite(&buffer_4[0], 1, 1, output); 
    memset(buffer_4, 0, sizeof(buffer_4));
    
    fwrite(&wav_info->file_type_header, sizeof(wav_info->file_type_header),1,output);
    fwrite(&wav_info->format_chunk_marker, sizeof(wav_info->format_chunk_marker),1,output);
    fwrite(&wav_info->format_data_length, sizeof(uint32_t),1,output);
    
    buffer_2[0] = 6; //a-law fmt
    buffer_2[1] = '\0';
    fwrite(&buffer_2[0], 1, 1, output);
    fwrite(&buffer_2[1], 1, 1, output);
    memset(buffer_2, 0, sizeof(buffer_2));


    fwrite(&wav_info->num_channel, sizeof(uint16_t),1,output);
    fwrite(&wav_info->sample_rate, sizeof(uint32_t),1,output);
    
    // byte rate
    buffer_4[3] = wav_info->byterate & 0xFF;
    buffer_4[2] = (wav_info->byterate >> 8) & 0xFF;
    buffer_4[1] = (wav_info->byterate >> 16) & 0xFF;
    buffer_4[0] = (wav_info->byterate >> 24) & 0xFF;
    fwrite(&buffer_4[3], 1, 1, output);
    fwrite(&buffer_4[2], 1, 1, output);
    fwrite(&buffer_4[1], 1, 1, output);
    fwrite(&buffer_4[0], 1, 1, output); 
    memset(buffer_4, 0, sizeof(buffer_4));
    
    //block align
    buffer_2[1] = wav_info->block_align & 0xFF;
    buffer_2[0] = (wav_info->block_align >> 8) & 0xFF;
    fwrite(&buffer_2[1], 1, 1, output);
    fwrite(&buffer_2[0], 1, 1, output);
    memset(buffer_2, 0, sizeof(buffer_2));
    //fwrite(&wav_info->block_align, sizeof(uint32_t),1,output);

    //bits per sample
    buffer_2[0] = wav_info->bits_per_sample;
    buffer_2[1] = '\0';
    fwrite(&buffer_2[0], 1, 1, output);
    fwrite(&buffer_2[1], 1, 1, output);
    memset(buffer_2, 0, sizeof(buffer_2));
    //fwrite(&wav_info->bits_per_sample, sizeof(uint16_t),1,output);

    fwrite(&wav_info->data_header, sizeof(wav_info->data_header),1,output);
    
    //data size
    new_data_size = new_file_size - 36;
    buffer_4[3] = new_data_size & 0xFF;
    buffer_4[2] = (new_data_size >> 8) & 0xFF;
    buffer_4[1] = (new_data_size >> 16) & 0xFF;
    buffer_4[0] = (new_data_size >> 24) & 0xFF;
    fwrite(&buffer_4[3], 1, 1, output);
    fwrite(&buffer_4[2], 1, 1, output);
    fwrite(&buffer_4[1], 1, 1, output);
    fwrite(&buffer_4[0], 1, 1, output); 
    memset(buffer_4, 0, sizeof(buffer_4));
    //fwrite(&wav_info->data_size, sizeof(uint32_t),1,output);


}
int16_t codeword_decompression(uint8_t codeword){
    int chord = ((codeword & 0x70) >> 4);
    int step = (codeword & 0x0F);
    int sign = (codeword & 0x80) >> 7;

    int magnitude;
     
    if (chord == 0x7) {
        magnitude = (1 << 6) | (step << 7) | (1 << 11);
    }
    else if (chord == 0x6) {
        magnitude = (1 << 5) | (step << 6) | (1 << 10);
    }
    else if (chord == 0x5) {
        magnitude = (1 << 4) | (step << 5) | (1 << 9);
    }
    else if (chord == 0x4) {
        magnitude = (1 << 3) | (step << 4) | (1 << 8);
    }
    else if (chord == 0x3) {
        magnitude = (1 << 2) | (step << 3) | (1 << 7);
    }
    else if (chord == 0x2) {
        magnitude = (1 << 1) | (step << 2) | (1 << 6);
    }
    else if (chord == 0x1) {
        magnitude = (1) | (step << 1) | (1 << 5);
    }
    else {
        magnitude = (1) | (step << 1) ;
        
    }

        if(!sign){
            magnitude = (-magnitude);
        }

    return (int16_t) (magnitude);
}
void get_wave_header_info(FILE* wav, WAV_HEADER* wav_info){
    printf("START OF GETTING WAVE HEADER INFO\n");
    uint8_t buffer_4[4];
    uint8_t buffer_2[2];

    //RIFF big endian
    printf("GETTING RIFF\n");
    fread((wav_info)->riff, sizeof((wav_info)->riff), 1, wav);
  
    //file size little endian
    printf("GETTING FILE SIZE\n");
    fread(buffer_4, sizeof(buffer_4), 1, wav);
    (wav_info)->file_size = (buffer_4[0]) | (buffer_4[1] << 8) | (buffer_4[2] << 16) | (buffer_4[3] << 24);

    memset(buffer_4, 0, sizeof(buffer_4));
    printf("File size: %d\n",(int)(wav_info)->file_size);

    // file_type_header big endian
    printf("GETTING FILE TYPE HEADER\n");
    fread((wav_info)->file_type_header, sizeof((wav_info)->file_type_header), 1, wav);

    // //format_chunk_marker big endian
    printf("GETTING CHUNK MARKER\n");
    fread((wav_info)->format_chunk_marker, sizeof((wav_info)->format_chunk_marker), 1, wav);

    // //format_data_length little endian
    printf("GETTING FORMAT DATA LENGTH\n");
    fread(buffer_4, sizeof(buffer_4), 1, wav);
    (wav_info)->format_data_length = (buffer_4[0]) | (buffer_4[1] << 8) | (buffer_4[2] << 16) | (buffer_4[3] << 24);
    memset(buffer_4, 0, sizeof(buffer_4));

    //format_type little endian
    printf("GETTING FORMAT TYPE\n");
    fread(buffer_2, sizeof(buffer_2), 1, wav);
 
    (wav_info)->format_type = (buffer_2[0] | (buffer_2[1] << 8));
    
    printf("Format Type: %d\n",(wav_info)->format_type);

    memset(buffer_2, 0, sizeof(buffer_2));


    //num_channel little endian
    printf("GETTING NUMBER OF CHANNELS\n");
    fread(buffer_2, sizeof(buffer_2), 1, wav);
    (wav_info)->num_channel = (buffer_2[0] | (buffer_2[1] << 8));
    memset(buffer_2, 0, sizeof(buffer_2));

    //sample_rate little endian
    printf("GETTING SAMPLE RATE\n");
    fread(buffer_4, sizeof(buffer_4), 1, wav);
    (wav_info)->sample_rate = (buffer_4[0]) | (buffer_4[1] << 8) | (buffer_4[2] << 16) | (buffer_4[3] << 24);
    memset(buffer_4, 0, sizeof(buffer_4));

    printf("Sample rate : %d\n",(wav_info)->sample_rate);

    //sr_btsps_channel little endian
    printf("GETTING BYTERATE\n");
    fread(buffer_4, sizeof(buffer_4), 1, wav);
    (wav_info)->byterate = (buffer_4[0]) | (buffer_4[1] << 8) | (buffer_4[2] << 16) | (buffer_4[3] << 24);
    memset(buffer_4, 0, sizeof(buffer_4));

    //bits_per_sample_channel little endian
    printf("GETTING block align\n");
    fread(buffer_2, sizeof(buffer_2), 1, wav);
    (wav_info)->block_align = (buffer_2[0]) | (buffer_2[1] << 8) ;
    memset(buffer_2, 0, sizeof(buffer_2));

    //bits_per_sample little endian
    printf("GETTING BITS PER SAMPLE\n");
    fread(buffer_2, sizeof(buffer_2), 1, wav);
    (wav_info)->bits_per_sample = (buffer_2[0] | (buffer_2[1] << 8));
    memset(buffer_2, 0, sizeof(buffer_2));

    //data_header big endian
    printf("GETTING DATA HEADER\n");
    fread((wav_info)->data_header, sizeof((wav_info)->data_header), 1, wav);

    //sizeof_data_section
    printf("GETTING SIZE OF DATA SECTION\n");
    fread(buffer_4, sizeof(buffer_4), 1, wav);
    (wav_info)->data_size = ((buffer_4[0]) | (buffer_4[1] << 8) | (buffer_4[2] << 16) | (buffer_4[3] << 24));
    memset(buffer_4, 0, sizeof(buffer_4));
    //printf("Data size: %d\n",(int)(wav_info)->data_size);

    printf("FINISHED GETTING WAV HEADER INFO\n");

    (wav_info)->samples = (8 * (wav_info)->data_size) / ((wav_info)->num_channel * (wav_info)->bits_per_sample);
    (wav_info)->size_of_samples = ((wav_info)->num_channel * (wav_info)->bits_per_sample) / 8;


}
uint16_t get_magnitude(short sample){
    if (sample < 0){sample = -sample;}
    return (uint16_t) sample;
}
uint8_t get_sign(short buffer){
    if(buffer >= 0){return 1;} //positive
    return 0; //negative   
}
uint8_t codeword_compression ( uint16_t sample_magnitude , uint8_t sign ) {
    uint8_t chord , step ;
    uint8_t codeword_tmp ;

    if( sample_magnitude & (1 << 11)) {
        chord = 0x7 ;
        step = ( sample_magnitude >> 7) & 0xF ;
    }
    else if( sample_magnitude & (1 << 10)) {
        chord = 0x6 ;
        step = ( sample_magnitude >> 6) & 0xF ;
    }
    else if( sample_magnitude & (1 << 9)) {
        chord = 0x5 ;
        step = ( sample_magnitude >> 5) & 0xF ;
    }
    else if( sample_magnitude & (1 << 8)) {
        chord = 0x4 ;
        step = ( sample_magnitude >> 4) & 0xF ;
    }
    else if( sample_magnitude & (1 << 7)) {
        chord = 0x3 ;
        step = ( sample_magnitude >> 3) & 0xF ;
    }
    else if( sample_magnitude & (1 << 6)) {
        chord = 0x2 ;
        step = ( sample_magnitude >> 2) & 0xF ;
    }
    else if( sample_magnitude & (1 << 5)) {
        chord = 0x1 ;
        step = ( sample_magnitude >> 1) & 0xF ;
    }
    else {
        chord = 0x0 ;
        step = ( sample_magnitude >> 1) & 0xF ;
    }
        codeword_tmp = ( sign << 7) | ( chord << 4) | step ;
        return (codeword_tmp ^ 0x55);

}
int main(int argc, char **argv){
    FILE * wav = fopen(argv[1], "rb");

    if (wav == NULL)
    {
        printf("no file to compress\n"); 
        exit(1);
    } else {
        printf("File recieved\n");
    }
    
    printf("making wav_info structure\n");
    WAV_HEADER* wav_info = (WAV_HEADER*)malloc(sizeof(WAV_HEADER)); 

    get_wave_header_info(wav, wav_info);

    printf("finished writing header to ouput file\n");
    read_data(wav, wav_info);

    free(wav_info);
    fclose(wav);

  
}