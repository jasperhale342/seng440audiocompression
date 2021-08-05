#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <memory.h> 
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

int main(int argc, char **argv)
{
    FILE * wav = fopen(argv[1], "rb");
    // FILE * compressed_wave = fopen("compressed_wav.md", "wb");

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
    
   
    printf("passing wav_info to get_wave_header function\n");
    get_wave_header_info(wav, wav_info);
    
    // other_func(wav);
    // printf("Printing wave info\n");
    // print_wave_header(&wav_info);
    fclose(wav);
  
}
//data is stored in little endian 
void read_data(FILE *wav_file,  FILE* output_file, wav_header* wav_struct)
{
    unsigned char buffer[2];

    while (wav_file != NULL)
    {
        fread(buffer, sizeof(char), 2, wav);

    }
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
   
    printf("here %x \n", buffer_32);
    memset(buffer_32, 0, 4);

    // file_type_header big endian
    printf("GETTING FILE TYPE HEADER\n");
    
   
    fread(buffer_32, sizeof(char), 4, wav);
    printf("here %x \n", buffer_32);
    (wav_info)->file_type_header = ((buffer_32[0]<<24) | (buffer_32[1])<<16 | (buffer_32[2])<<8 | (buffer_32[3]));
    memset(buffer_32, 0, 4);
    printf("here %ld \n", (wav_info)->file_type_header);
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

// void print_wave_header(wav_header **wav_info)
// {
//     printf("START OF PRINTING WAV HEADER INFO\n");
//     printf("RIFF: " "%" PRIu32"\n", (wav_info)->RIFF);
//     printf("file_size: " "%" PRIu32 " \n" , (wav_info)->file_size);
//     printf("file_type_header: " "%" PRIu32 " \n" ,(wav_info)->file_type_header);
//     printf("chunk_marker: " "%" PRIu32 " \n" ,(wav_info)->format_chunk_marker);
//     printf("format_data_length: " "%" PRIu32 " \n" ,(wav_info)->format_data_length);

//     printf("format_type: " "%" PRIu16 " \n" ,(wav_info)->format_type);
//     printf("num_channel: " "%" PRIu16 " \n" ,(wav_info)->num_channel);

//     printf("sample_rate: " "%" PRIu32 " \n" ,(wav_info)->sample_rate);
//     printf("sr_btsps_channel: " "%" PRIu32 " \n" ,(wav_info)->sr_btsps_channel);
//     printf("bits_per_sample_channel: " "%" PRIu32 " \n" ,(wav_info)->bits_per_sample_channel);

//     printf("bits_per_sample: " "%" PRIu16 " \n" ,(wav_info)->bits_per_sample);

//     printf("data_header: " "%" PRIu32 " \n" ,(wav_info)->data_header);
//     printf("sizeof_data_section: " "%" PRIu32 " \n" ,(wav_info)->sizeof_data_section);
// }
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


void other_func(FILE * wav, wav_header **wav_info)
{
     unsigned char  buffer_32[4];
    unsigned char buffer_other[4];

    
    
    //RIFF big endian
    printf("===OTHER===: GETTING RIFF\n");
    fread(buffer_32, sizeof(char), 4, wav);
    (*wav_info)->RIFF = ((buffer_32[0]<<24) | (buffer_32[1])<<16 | (buffer_32[2])<<8 | (buffer_32[3]));
    // memset(buffer_32, 0, 4);
    printf("===OTHER===:wave riff: %x\n", (*wav_info)->RIFF);
   
    //file size little endian
    printf("===OTHER===:GETTING FILE SIZE\n");
    fread(buffer_32, sizeof(char), 4, wav);
    (*wav_info)->file_size = (buffer_32[0]) | (buffer_32[1] << 8) | (buffer_32[2] << 16) | (buffer_32[3] << 24);
    // memset(buffer_32, 0, 4);
    printf("===OTHER===:FILE SIZE IS is %d\n", (*wav_info)->file_size);

    // file_type_header big endian
    printf("===OTHER===:GETTING FILE TYPE HEADER\n");
}


char get_magnitude(){
    return 'a';
}
char get_sign(unsigned char buffer[]){
    if(buffer[0]<<8 | buffer[1] & 0x8000)
    {
        return 1; //positive
    }
    return 0; //negative
}
char codeword_compression ( unsigned int sample_magnitude , int sign ) {
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


