#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <memory.h> 
#include <time.h>
int8_t a_law(int16_t sample){
    return 0;

}
/*
    1. Read in file
    2. compression or decompression
    compression:
        1. create extension
        2. read data from file header to find out important info
        3. input signal is converted to is converted to sign-magnitude representation 
        4. add 33 for bias
        5. 
*/
typedef struct wav_header {
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


}  wav_header;


void print_wave_header(wav_header * wav_info);
void initialize_struct(wav_header * wav_struct);
void get_wave_header_info(FILE * wav, wav_header* wav_info);
//void write_header_to_new_file(FILE *input, FILE * output);
void write_header_to_new_file(wav_header * wav_info, FILE * output);
uint8_t codeword_compression ( uint16_t sample_magnitude , uint8_t sign );
uint8_t get_sign(short buffer);
uint16_t get_magnitude(short sample);
uint16_t get_magnitude_from_codeword1(uint8_t codeword);
void read_data(FILE *wav_file, FILE *output, wav_header* wav_struct);
void write_input_header(FILE *input, FILE * output);
//unsigned short get_magnitude_from_codeword(uint8_t codeword);
int main(int argc, char **argv)
{
    FILE * wav = fopen(argv[1], "rb");
    //FILE * wav = fopen("input.wav", "rb");

    FILE * compressed_wave = fopen("new_wav.wav", "wb");

    if (wav == NULL)
    {
        printf("no file to compress\n"); 
        exit(1);
    } else {
        printf("File recieved\n");
    }
    
    printf("making wav_info structure\n");
    wav_header* wav_info = (wav_header*)malloc(sizeof(wav_header)); 
   
    //initialize_struct(wav_info);

    get_wave_header_info(wav, wav_info);
    


    //write_header_to_new_file(wav, compressed_wave);
    write_header_to_new_file(wav_info, compressed_wave);
    

    printf("finished writing header to ouput file\n");
    read_data(wav, compressed_wave, wav_info);

    
    // other_func(wav);
    // printf("Printing wave info\n");
    // print_wave_header(&wav_info);
    free(wav_info);
    fclose(wav);
    fclose(compressed_wave);
  
}
//data is stored in little endian 


void read_data(FILE *wav_file, FILE *output, wav_header* wav_struct)
{
    short buffer;
    short in_sample;
    uint8_t sign;
    uint16_t magnitude;
    int size = ((int)wav_struct->file_size+8-44); 
    uint8_t * data_of_file = (uint8_t*)malloc(sizeof(uint8_t) *size);
    uint8_t * compressed_codeword = (uint8_t*)malloc(sizeof(uint8_t) * size);
    //uint8_t * file_data_compressed_codeword = malloc(sizeof(char) * wav_header->file_size-36);

    //fseek(wav_file,44,SEEK_SET);

    time_t  start = clock();
    fread(data_of_file, size, 1, wav_file);
   
    int i;
    int count = 0;

    //printf("SIZE:  %d\n",size);

    for ( i= 0; i < size-1; i+=2)
    {
        buffer = (data_of_file[i] | (data_of_file[i+1] << 8));
        sign = get_sign(buffer);
        magnitude = get_magnitude(buffer);
        compressed_codeword[i] = codeword_compression(magnitude, sign);
        count++;
        fwrite( &compressed_codeword[i],sizeof(compressed_codeword[i]),1, output );
        //printf("codeword: %d\n",compressed_codeword[i]);
    }
    time_t  stop = clock();
    double compression_time = (double) (stop - start) / CLOCKS_PER_SEC;
    
    printf("FILE COMPRESSED. time: %f\n", compression_time);
    printf("Iteration in compression loop: %d\n", count);
   
    int16_t out_sample;
    short out_sample_1;
    uint8_t codeword;
    uint8_t out_step;
    uint8_t out_chord;
    uint8_t out_sign;
    uint8_t buf[2];
    int counter2 = 0;
    uint16_t temp;


    short first_un = data_of_file[0] | (data_of_file[1] << 8);
    short first_de;

    FILE* dec = fopen("dec.wav","wb");
    
    write_input_header(wav_file,dec);

    for(int i=0;i<size-1;i+=2){
        codeword = (compressed_codeword[i] ^ 0x55);
        out_sign = (codeword & 0x80) >> 7;
        out_sample = get_magnitude_from_codeword1(codeword);
        if(out_sign == 0){
            out_sample = (-out_sample);
        }

        if(i ==0){
            first_de = out_sample_1;
        }

        counter2++;
        //out_sample_1 = out_sample_1 << 3;
        fwrite(&out_sample,sizeof(int16_t),1, dec );
        //fwrite(&temp,sizeof(temp),1, dec );
    }


        printf("un = %u , de = %u\n",first_un, first_de);



    fclose(dec);
    //free(data_of_file);
    free(compressed_codeword);
}

void write_input_header(FILE *input, FILE * output)
{
    uint8_t buffer[44];
    fseek(input,0,SEEK_SET);
    fread(buffer, sizeof(buffer) ,1, input);
    fwrite(buffer, sizeof(buffer),1,output);

}

void write_header_to_new_file(wav_header* wav_info, FILE * output)
{   
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
    //fwrite(&wav_info->format_type, sizeof(uint16_t),1,output);
    
    buffer_2[0] = 6;
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


uint16_t get_magnitude_from_codeword1(uint8_t codeword)
{
    //codeword = codeword ^ 0x55;
    int chord = ((codeword & 0x70) >> 4);
    int step = (codeword & 0x0F);
    int magnitude;
    int bit = 1;
     
    if (chord == 0x7) {
        magnitude = (bit << 6) | (step << 7) | (bit << 11);
    }
    else if (chord == 0x6) {
        magnitude = (bit << 5) | (step << 6) | (bit << 10);
    }
    else if (chord == 0x5) {
        magnitude = (bit << 4) | (step << 5) | (bit << 9);
    }
    else if (chord == 0x4) {
        magnitude = (bit << 3) | (step << 4) | (bit << 8);
    }
    else if (chord == 0x3) {
        magnitude = (bit << 2) | (step << 3) | (bit << 7);
    }
    else if (chord == 0x2) {
        magnitude = (bit << 1) | (step << 2) | (bit << 6);
    }
    else if (chord == 0x1) {
        magnitude = (bit) | (step << 1) | (bit << 5);
    }
    else {
        magnitude = (bit) | (step << 1) ;
        
    }
    return (uint16_t) (magnitude);
}

void get_wave_header_info(FILE* wav, wav_header* wav_info)
{
    printf("START OF GETTING WAVE HEADER INFO\n");
    uint8_t buffer_4[4];
    uint8_t buffer_2[2];
    //uint8_t* buffer_4 = (uint8_t*)calloc(4,sizeof(uint8_t));
   
    //RIFF big endian
    printf("GETTING RIFF\n");
    fread((wav_info)->riff, sizeof((wav_info)->riff), 1, wav);
    

    //printf("here %x \n", buffer_4);
    //(wav_info)->riff = ((buffer_4[0]<<24) | (buffer_4[1])<<16 | (buffer_4[2])<<8 | (buffer_4[3]));
    //memset(buffer_4, 0, 4*sizeof(uint8_t));
    //fflush(stdout);
  
    //file size little endian
    printf("GETTING FILE SIZE\n");
    fread(buffer_4, sizeof(buffer_4), 1, wav);
    (wav_info)->file_size = (buffer_4[0]) | (buffer_4[1] << 8) | (buffer_4[2] << 16) | (buffer_4[3] << 24);
    // printf("FILE SIZE IS is %d\n", (*wav_info)->file_size);
    memset(buffer_4, 0, sizeof(buffer_4));
    printf("File size: %d\n",(int)(wav_info)->file_size);


    // file_type_header big endian
    printf("GETTING FILE TYPE HEADER\n");
    fread((wav_info)->file_type_header, sizeof((wav_info)->file_type_header), 1, wav);
    //(wav_info)->file_type_header = ((buffer_4[0]<<24) | (buffer_4[1])<<16 | (buffer_4[2])<<8 | (buffer_4[3]));
    //memset(buffer_4, 0, 4*sizeof(uint8_t));
   
 
    // //format_chunk_marker big endian
    printf("GETTING CHUNK MARKER\n");
    fread((wav_info)->format_chunk_marker, sizeof((wav_info)->format_chunk_marker), 1, wav);
    //(wav_info)->format_chunk_marker = ((buffer_4[0]<<24) | (buffer_4[1])<<16 | (buffer_4[2])<<8 | (buffer_4[3]));
    //memset(buffer_4, 0, 4*sizeof(uint8_t));
    //printf("file type header: %ld\n",(wav_info)->format_chunk_marker );
    
    

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
    //(wav_info)->data_header = ((buffer_4[0]<<24) | (buffer_4[1])<<16 | (buffer_4[2])<<8 | (buffer_4[3]));
   // memset(buffer_4, 0, 4*sizeof(uint8_t));
    printf("%c%c%c%c\n",(wav_info)->data_header[0],(wav_info)->data_header[1],(wav_info)->data_header[2],(wav_info)->data_header[3]);
    //sizeof_data_section
    printf("GETTING SIZE OF DATA SECTION\n");
    fread(buffer_4, sizeof(buffer_4), 1, wav);
    (wav_info)->data_size = ((buffer_4[0]) | (buffer_4[1] << 8) | (buffer_4[2] << 16) | (buffer_4[3] << 24));
    memset(buffer_4, 0, sizeof(buffer_4));
    printf("Data size: %d\n",(int)(wav_info)->data_size);

    printf("FINISHED PRINTING WAV HEADER INFO\n");

    (wav_info)->samples = (8 * (wav_info)->data_size) / ((wav_info)->num_channel * (wav_info)->bits_per_sample);
    (wav_info)->size_of_samples = ((wav_info)->num_channel * (wav_info)->bits_per_sample) / 8;

    printf("num sample %d, size sample %d\n",(wav_info)->samples,(wav_info)->size_of_samples);
    printf("BPS %d\n",(wav_info)->bits_per_sample);

}


uint16_t get_magnitude(short sample)
{
    return (uint16_t) (sample < 0 ? -sample : sample);
}
uint8_t get_sign(short buffer)
{
    if(buffer >= 0)
    {
        return 1; //positive
    }
    else 
    {
        return 0; //negative
    }
    
}
uint8_t codeword_compression ( uint16_t sample_magnitude , uint8_t sign ) 
{
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
/*
unsigned short get_magnitude_from_codeword(uint8_t codeword)
{       
        codeword = codeword ^ 0x55;
        int chord = (codeword & 0x70) >> 4;
        int step = codeword & 0x0F;
        int magnitude;
        
        if (chord == 0x7) {
            magnitude = (1 << 6) | (step << 8) | (1 << 11);
        }
        else if (chord == 0x6) {
            magnitude = (1 << 5) | (step << 7) | (1 << 10);
        }
        else if (chord == 0x5) {
            magnitude = (1 << 4) | (step << 6) | (1 << 9);
        }
        else if (chord == 0x4) {
            magnitude = (1 << 3) | (step << 5) | (1 << 8);
        }
        else if (chord == 0x3) {
            magnitude = (1 << 2) | (step << 4) | (1 << 7);
        }
        else if (chord == 0x2) {
            magnitude = (1 << 1) | (step << 3) | (1 << 6);
        }
        else if (chord == 0x1) {
            magnitude = (1) | (step << 2) | (1 << 5);
        }
        else {
            magnitude = 1 | (step << 1) ;
        }

        return  (unsigned short)magnitude;
}
*/
