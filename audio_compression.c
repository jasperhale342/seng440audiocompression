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
typedef struct wav_header_info {
    unsigned long samples;
    unsigned int size_of_samples;

    unsigned long  RIFF; // big endian
    unsigned long file_size;// little endian
    unsigned long file_type_header;// big endian
    unsigned long format_chunk_marker;// big endian
    __uint32_t format_data_length;// little endian

    __uint16_t format_type;// little endian
    __uint16_t num_channel;// little endian

    __uint32_t sample_rate;// little endian
    __uint32_t sr_btsps_channel; // little endian, (Sample Rate * BitsPerSample * Channels) / 8
    __uint32_t bits_per_sample_channel; // little endian, (BitsPerSample * Channels) / 8.1 - 8 bit mono2 - 8 bit stereo/16 bit mono4 - 16 bit stereo

    __uint16_t bits_per_sample; // little endian

    __uint32_t data_header; // big endian, Marks the beginning of the data section.
    __uint32_t sizeof_data_section;// little endian


}  wav_header;


void print_wave_header(wav_header * wav_info);
void initialize_struct(wav_header * wav_struct);
void get_wave_header_info(FILE * wav, wav_header* wav_info);
void write_header_to_new_file(FILE *input, FILE * output);
unsigned char codeword_compression ( unsigned short sample_magnitude , short sign );
short get_sign(unsigned short buffer);
char get_magnitude(unsigned short sample);
unsigned short get_magnitude_from_codeword(unsigned char codeword);
void read_data(FILE *wav_file, FILE *output, wav_header* wav_struct);
int main(int argc, char **argv)
{
    FILE * wav = fopen(argv[1], "rb");
    FILE * compressed_wave = fopen("new_wav.wav", "wb");

    if (wav == NULL)
    {
        printf("no file to compress\n"); 
        exit(1);
    } else {
        printf("File recieved\n");
    }
    
    printf("making wav_info structure\n");
    wav_header* wav_info = malloc(sizeof(wav_header)); 
   
    initialize_struct(wav_info);

    get_wave_header_info(wav, wav_info);
    

    write_header_to_new_file(wav, compressed_wave);
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
    unsigned short buffer;
    short sign;
    short magnitude;
    unsigned char * data_of_file = malloc(sizeof(char) * wav_struct->file_size-36);
    unsigned char * compressed_codeword = malloc(sizeof(char) * wav_struct->file_size-36);
    //unsigned char * file_data_compressed_codeword = malloc(sizeof(char) * wav_header->file_size-36);

    time_t  start = clock();
    fread(data_of_file, wav_struct->file_size-36, 1, wav_file);
    unsigned long size = wav_struct->file_size; 
    int i;
    for ( i= 0; i < size-36; i++)
    {
        buffer = data_of_file[i] | data_of_file[i+1] << 8;
        sign = get_sign(buffer);
        magnitude = get_magnitude(buffer);
        compressed_codeword[i] = codeword_compression(magnitude, sign);

    }
    time_t  stop = clock();
    double compression_time = (double) (stop - start) / CLOCKS_PER_SEC;
    
    printf("FILE COMPRESSED. time: %d\n", compression_time);

    printf("DECOMPRESSING\n");
    char buffer_2[2];
    char sample;
  
    for (i= 0; i < size; i++)
    {
         
        sign = (compressed_codeword[i] & 0x80) >> 7;
        magnitude  = get_magnitude_from_codeword(compressed_codeword[i]);
        if(sign){
            sample = magnitude;
        } else {
            sample = -magnitude;
        }
        buffer_2[0] =  (char)magnitude & 0x000000FF;
        buffer_2[1] = (char)(magnitude & 0x0000FF00) >> 8;
        fputs( buffer_2, output );
    }

    free(data_of_file);
    free(compressed_codeword);
}
void write_header_to_new_file(FILE *input, FILE * output)
{
    unsigned char buffer[44];
    fread(buffer, sizeof(char), 44, input);
    fputs(buffer, output);
}

unsigned short get_magnitude_from_codeword(unsigned char codeword)
{
    unsigned char chord = (codeword & 0x70) >> 4;;
    unsigned char step = codeword & 0x0F;
    int magnitude;
     
    if (chord == 0x7) {
        magnitude = (1 << 7) | (step << 8) | (1 << 12);
    }
    else if (chord == 0x6) {
        magnitude = (1 << 6) | (step << 7) | (1 << 11);
    }
    else if (chord == 0x5) {
        magnitude = (1 << 5) | (step << 6) | (1 << 10);
    }
    else if (chord == 0x4) {
        magnitude = (1 << 4) | (step << 5) | (1 << 9);
    }
    else if (chord == 0x3) {
        magnitude = (1 << 3) | (step << 4) | (1 << 8);
    }
    else if (chord == 0x2) {
        magnitude = (1 << 2) | (step << 3) | (1 << 7);
    }
    else if (chord == 0x1) {
        magnitude = (1 << 1) | (step << 2) | (1 << 6);
    }
    else if (chord == 0x0) {
        magnitude = 1 | (step << 1) | (1 << 5);
    }

    return (unsigned short) magnitude;
}

void get_wave_header_info(FILE * wav, wav_header* wav_info)
{
    printf("START OF GETTING WAVE HEADER INFO\n");
    unsigned char  buffer_32[4];
   
    //RIFF big endian
    printf("GETTING RIFF\n");
    fread(buffer_32, sizeof(char), 4, wav);
    printf("here %x \n", buffer_32);
    (wav_info)->RIFF = ((buffer_32[0]<<24) | (buffer_32[1])<<16 | (buffer_32[2])<<8 | (buffer_32[3]));
    memset(buffer_32, 0, 4);
   // printf("wave riff: %x\n", (*wav_info)->RIFF);
   
   
    //file size little endian
    printf("GETTING FILE SIZE\n");
    fread(buffer_32, sizeof(char), 4, wav);
    (wav_info)->file_size = (buffer_32[0]) | (buffer_32[1] << 8) | (buffer_32[2] << 16) | (buffer_32[3] << 24);
    // printf("FILE SIZE IS is %d\n", (*wav_info)->file_size);
    memset(buffer_32, 0, 4);

    // file_type_header big endian
    printf("GETTING FILE TYPE HEADER\n");
    fread(buffer_32, sizeof(char), 4, wav);
    (wav_info)->file_type_header = ((buffer_32[0]<<24) | (buffer_32[1])<<16 | (buffer_32[2])<<8 | (buffer_32[3]));
    memset(buffer_32, 0, 4);
    

    // //format_chunk_marker big endian
    printf("GETTING CHUNK MARKER\n");
    fread(buffer_32, sizeof(char), 4, wav);
    (wav_info)->format_chunk_marker = ((buffer_32[0]<<24) | (buffer_32[1])<<16 | (buffer_32[2])<<8 | (buffer_32[3]));
    memset(buffer_32, 0, 4);
    printf("file type header: %ld\n",(wav_info)->format_chunk_marker );
    
    

    // //format_data_length little endian
    printf("GETTING FORMAT DATA LENGTH\n");
    fread(buffer_32, sizeof(char), 4, wav);
    (wav_info)->format_data_length = (buffer_32[0]) | (buffer_32[1] << 8) | (buffer_32[2] << 16) | (buffer_32[3] << 24);
    memset(buffer_32, 0, 4);

    //format_type little endian
    printf("GETTING FORMAT TYPE\n");
    fread(buffer_32, sizeof(char), 4, wav);
    (wav_info)->format_type = (buffer_32[0] | (buffer_32[1] << 8));
    memset(buffer_32, 0, 4);

    //num_channel little endian
    printf("GETTING NUMBER OF CHANNELS\n");
    fread(buffer_32, sizeof(char), 4, wav);
    (wav_info)->num_channel = (buffer_32[0] | (buffer_32[1] << 8));
    memset(buffer_32, 0, 4);

    //sample_rate little endian
    printf("GETTING SAMPLE RATE\n");
    fread(buffer_32, sizeof(char), 4, wav);
    (wav_info)->sample_rate = (buffer_32[0]) | (buffer_32[1] << 8) | (buffer_32[2] << 16) | (buffer_32[3] << 24);
    memset(buffer_32, 0, 4);

    //sr_btsps_channel little endian
    printf("GETTING SR_BTSPS_CHANNEL\n");
    fread(buffer_32, sizeof(char), 4, wav);
    (wav_info)->sr_btsps_channel = (buffer_32[0]) | (buffer_32[1] << 8) | (buffer_32[2] << 16) | (buffer_32[3] << 24);
    memset(buffer_32, 0, 4);

    //bits_per_sample_channel little endian
    printf("GETTING BITS PER SAMPLE CHANNEL\n");
    fread(buffer_32, sizeof(char), 4, wav);
    (wav_info)->bits_per_sample_channel = (buffer_32[0]) | (buffer_32[1] << 8) | (buffer_32[2] << 16) | (buffer_32[3] << 24);
    memset(buffer_32, 0, 4);

    //bits_per_sample little endian
    printf("GETTING BITS PER SAMPLE\n");
    fread(buffer_32, sizeof(char), 4, wav);
    (wav_info)->bits_per_sample = (buffer_32[0] | (buffer_32[1] << 8));
    memset(buffer_32, 0, 4);

    //data_header big endian
    printf("GETTING DATA HEADER\n");
    fread(buffer_32, sizeof(char), 4, wav);
    (wav_info)->data_header = ((buffer_32[0]<<24) | (buffer_32[1])<<16 | (buffer_32[2])<<8 | (buffer_32[3]));
    memset(buffer_32, 0, 4);

    //sizeof_data_section
    printf("GETTING SIZE OF DATA SECTION\n");
    fread(buffer_32, sizeof(char), 4, wav);
    (wav_info)->sizeof_data_section = ((buffer_32[0]) | (buffer_32[1] << 8) | (buffer_32[2] << 16) | (buffer_32[3] << 24));
    memset(buffer_32, 0, 4);
    printf("FINISHED PRINTING WAV HEADER INFO\n");

}


void initialize_struct(wav_header * wav_struct)
{
  wav_header *new_struct = malloc(sizeof(new_struct));
    new_struct->samples = 0;
    new_struct->size_of_samples= 0;
    new_struct->RIFF= 0; 
    new_struct->file_size= 0;
    new_struct->file_type_header= 0;
    new_struct->format_chunk_marker= 0;
    new_struct->format_data_length= 0;
    new_struct->format_type= 0;
    new_struct->num_channel= 0;
    new_struct->sample_rate= 0;
    new_struct->sr_btsps_channel= 0; 
    new_struct->bits_per_sample_channel= 0; 
    new_struct->bits_per_sample= 0; 
    new_struct->data_header= 0; 
    new_struct->sizeof_data_section= 0;
    wav_struct = new_struct;
}





char get_magnitude(unsigned short sample)
{
    return (unsigned short) (sample < 0 ? -sample : sample);
}
short get_sign(unsigned short buffer)
{
    if(buffer & 0x8000)
    {
        return 1; //positive
    }
    return 0; //negative
}
unsigned char codeword_compression ( unsigned short sample_magnitude , short sign ) 
{
    int chord , step ;
    int codeword_tmp ;
    if( sample_magnitude & (1 << 12)) {
        chord = 0x7 ;
        step = ( sample_magnitude >> 8) & 0xF ;
        codeword_tmp = ( sign << 7) & ( chord << 4) & step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 11)) {
        chord = 0x6 ;
        step = ( sample_magnitude >> 7) & 0xF ;
        codeword_tmp = ( sign << 7) & ( chord << 4) & step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 10)) {
        chord = 0x5 ;
        step = ( sample_magnitude >> 6) & 0xF ;
        codeword_tmp = ( sign << 7) & ( chord << 4) & step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 9)) {
        chord = 0x4 ;
        step = ( sample_magnitude >> 5) & 0xF ;
        codeword_tmp = ( sign << 7) & ( chord << 4) & step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 8)) {
        chord = 0x3 ;
        step = ( sample_magnitude >> 4) & 0xF ;
        codeword_tmp = ( sign << 7) & ( chord << 4) & step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 7)) {
        chord = 0x2 ;
        step = ( sample_magnitude >> 3) & 0xF ;
        codeword_tmp = ( sign << 7) & ( chord << 4) & step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 6)) {
        chord = 0x1 ;
        step = ( sample_magnitude >> 2) & 0xF ;
        codeword_tmp = ( sign << 7) & ( chord << 4) & step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 5)) {
        chord = 0x0 ;
        step = ( sample_magnitude >> 1) & 0xF ;
        codeword_tmp = ( sign << 7) & ( chord << 4) & step ;
        return ( (char) codeword_tmp );
    }
}


