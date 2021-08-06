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


	unsigned char riff[4];						// RIFF string
	uint32_t file_size;				// overall size of file in bytes
	unsigned char file_type_header[4];						// WAVE string
	unsigned char format_chunk_marker[4];			// fmt string with trailing null char
	uint32_t format_data_length;					// length of the format data
	uint16_t format_type;					// format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
	uint16_t num_channel;						// no.of channels
	uint32_t sample_rate;					// sampling rate (blocks per second)
	uint32_t byterate;						// SampleRate * NumChannels * BitsPerSample/8
	uint32_t block_align;					// NumChannels * BitsPerSample/8
	uint16_t bits_per_sample;				// bits per sample, 8- 8bits, 16- 16 bits etc
	unsigned char data_header [4];		// DATA string or FLLR string
	uint32_t data_size;						// NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read


}  wav_header;


void print_wave_header(wav_header * wav_info);
void initialize_struct(wav_header * wav_struct);
void get_wave_header_info(FILE * wav, wav_header* wav_info);
void write_header_to_new_file(FILE *input, FILE * output);
unsigned char codeword_compression ( unsigned short sample_magnitude , short sign );
short get_sign(short buffer);
char get_magnitude(short sample);
unsigned short get_magnitude_from_codeword(unsigned char codeword);
void read_data(FILE *wav_file, FILE *output, wav_header* wav_struct);
int main(int argc, char **argv)
{
    //FILE * wav = fopen(argv[1], "rb");
    FILE * wav = fopen("haazah.wav", "rb");

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
    short buffer;
    short sign;
    unsigned short magnitude;
    int size = (int)(wav_struct->file_size+8-44); 
    unsigned char * data_of_file = malloc(sizeof(unsigned char) *size);
    unsigned char * compressed_codeword = malloc(sizeof(unsigned char) * size);
    //unsigned char * file_data_compressed_codeword = malloc(sizeof(char) * wav_header->file_size-36);

    fseek(wav_file,44,SEEK_SET);

    time_t  start = clock();
    fread(data_of_file, size, 1, wav_file);
   
    int i;
    int count = 0;

    //printf("SIZE:  %d\n",size);

    for ( i= 0; i < size-1; i+=2)
    {
        buffer = (data_of_file[i] | data_of_file[i+1] << 8) >> 3;
        sign = get_sign(buffer);
        magnitude = get_magnitude(buffer);
        compressed_codeword[i] = codeword_compression(magnitude, sign);
        count++;

    }
    time_t  stop = clock();
    double compression_time = (double) (stop - start) / CLOCKS_PER_SEC;
    
    printf("FILE COMPRESSED. time: %f\n", compression_time);

    printf("DECOMPRESSING\n");
    char buffer_2[2];
    char sample;
  
  
    for (i= 0; i < count; i++)
    {
         
        sign = (compressed_codeword[i] & 0x80) >> 7;
        magnitude  = get_magnitude_from_codeword(compressed_codeword[i]);
        magnitude = magnitude;
        if(sign){
            sample = magnitude;
        } else {
            sample = -magnitude;
        }
        sample = sample << 3;
        buffer_2[0] =  (char)sample & 0x000000FF;
        buffer_2[1] = (char)(sample & 0x0000FF00) >> 8;
        fwrite( buffer_2,sizeof(buffer_2),1, output );
    }

    free(data_of_file);
    free(compressed_codeword);
}

void write_header_to_new_file(FILE *input, FILE * output)
{
    unsigned char buffer[44];
    fseek(input,0,SEEK_SET);
    fread(buffer, sizeof(unsigned char), 44, input);
    fwrite(buffer, sizeof(buffer),1,output);
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

void get_wave_header_info(FILE* wav, wav_header* wav_info)
{
    printf("START OF GETTING WAVE HEADER INFO\n");
    unsigned char buffer_4[4];
    unsigned char buffer_2[2];
    //unsigned char* buffer_4 = (unsigned char*)calloc(4,sizeof(unsigned char));
   
    //RIFF big endian
    printf("GETTING RIFF\n");
    fread((wav_info)->riff, sizeof((wav_info)->riff), 1, wav);
    

    //printf("here %x \n", buffer_4);
    //(wav_info)->riff = ((buffer_4[0]<<24) | (buffer_4[1])<<16 | (buffer_4[2])<<8 | (buffer_4[3]));
    //memset(buffer_4, 0, 4*sizeof(unsigned char));
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
    //memset(buffer_4, 0, 4*sizeof(unsigned char));
   
 
    // //format_chunk_marker big endian
    printf("GETTING CHUNK MARKER\n");
    fread((wav_info)->format_chunk_marker, sizeof((wav_info)->format_chunk_marker), 1, wav);
    //(wav_info)->format_chunk_marker = ((buffer_4[0]<<24) | (buffer_4[1])<<16 | (buffer_4[2])<<8 | (buffer_4[3]));
    //memset(buffer_4, 0, 4*sizeof(unsigned char));
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
   // memset(buffer_4, 0, 4*sizeof(unsigned char));

    //sizeof_data_section
    printf("GETTING SIZE OF DATA SECTION\n");
    fread(buffer_4, sizeof(buffer_4), 1, wav);
    (wav_info)->data_size = ((buffer_4[0]) | (buffer_4[1] << 8) | (buffer_4[2] << 16) | (buffer_4[3] << 24));
    memset(buffer_4, 0, sizeof(buffer_4));
    printf("Data size: %d\n",(int)(wav_info)->data_size);

    printf("FINISHED PRINTING WAV HEADER INFO\n");

    (wav_info)->samples = (8 * (wav_info)->data_size) / ((wav_info)->num_channel * (wav_info)->bits_per_sample);
    (wav_info)->size_of_samples = ((wav_info)->num_channel * (wav_info)->bits_per_sample) / 8;


}

/*
void initialize_struct(wav_header * wav_struct)
{
  wav_header *new_struct = (wav_header*)malloc(sizeof(new_struct));
    new_struct->samples = 0;
    new_struct->size_of_samples= 0;
   
    new_struct->RIFF=0;
    //new_struct->RIFF= (unsigned char*)malloc(4*sizeof(unsigned char)); 
   
    new_struct->overall_size= 0;
    new_struct->wave= 0;
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
*/
char get_magnitude(short sample)
{
    return (unsigned short) (sample < 0 ? -sample : sample);
}
short get_sign(short buffer)
{
    if(buffer >= 0)
    {
        return 1; //positive
    }
    return 0; //negative
}
unsigned char codeword_compression ( unsigned short sample_magnitude , short sign ) 
{
    int chord , step ;
    int codeword_tmp ;
    if( sample_magnitude & (1 << 11)) {
        chord = 0x7 ;
        step = ( sample_magnitude >> 7) & 0xF ;
        codeword_tmp = ( sign << 7) | ( chord << 4) | step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 10)) {
        chord = 0x6 ;
        step = ( sample_magnitude >> 6) & 0xF ;
        codeword_tmp = ( sign << 7) | ( chord << 4) | step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 9)) {
        chord = 0x5 ;
        step = ( sample_magnitude >> 5) & 0xF ;
        codeword_tmp = ( sign << 7) | ( chord << 4) | step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 8)) {
        chord = 0x4 ;
        step = ( sample_magnitude >> 4) & 0xF ;
        codeword_tmp = ( sign << 7) | ( chord << 4) | step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 7)) {
        chord = 0x3 ;
        step = ( sample_magnitude >> 3) & 0xF ;
        codeword_tmp = ( sign << 7) | ( chord << 4) | step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 6)) {
        chord = 0x2 ;
        step = ( sample_magnitude >> 2) & 0xF ;
        codeword_tmp = ( sign << 7) | ( chord << 4) | step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 5)) {
        chord = 0x1 ;
        step = ( sample_magnitude >> 1) & 0xF ;
        codeword_tmp = ( sign << 7) | ( chord << 4) | step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 4)) {
        chord = 0x0 ;
        step = ( sample_magnitude ) & 0xF ;
        codeword_tmp = ( sign << 7) | ( chord << 4) | step ;
        return ( (char) codeword_tmp );
    }
}


