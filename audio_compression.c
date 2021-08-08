#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <memory.h> 
#include <time.h>
#include <stdbool.h> 

int chord1;
int chord2;
int chord3;
int chord4;
int chord5;
int chord6;
int chord7;
int chord8;

int decomp_chord1;
int decomp_chord2;
int decomp_chord3;
int decomp_chord4;
int decomp_chord5;
int decomp_chord6;
int decomp_chord7;
int decomp_chord8;


int oustide_counter;
typedef struct wav_header {
    uint32_t samples;
    uint32_t size_of_samples;

    //ChunkID
	uint8_t riff[4];

    //ChunkSize						
	uint32_t file_size;				// overall size of file in bytes
	
    //Format
    uint8_t file_type_header[4];						// WAVE string
	
    //SubChunkID
    uint8_t format_chunk_marker[4];			// fmt string with trailing null char

    //SubChunk1Size
	uint32_t format_data_length;					// length of the format data
	
    //AudioFormat
    uint16_t format_type;					// format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
	
    //NumOfChannels
    uint16_t num_channel;						// no.of channels
	
    //SampleRate
    uint32_t sample_rate;					// sampling rate (blocks per second)
	
    //ByteRate
    uint32_t byterate;						// SampleRate * NumChannels * BitsPerSample/8
	
    //Block Align
    uint32_t block_align;					// NumChannels * BitsPerSample/8
	
    //BitsPerSample
    uint16_t bits_per_sample;				// bits per sample, 8- 8bits, 16- 16 bits etc
	
    //SubChunk2ID
    uint8_t data_header [4];		// DATA string or FLLR string
	
    //Subchunk2Size
    uint32_t data_size;						// NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read


}  wav_header;

unsigned short get_magnitude(short sample)
{
    return (unsigned short) (sample < 0 ? -sample : sample);
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

void write_four_bytes(FILE * output, uint32_t data, bool little_endian)
{
    uint8_t buffer_4[4];
    if(little_endian)
    {
        buffer_4[3] = data & 0xFF;
        buffer_4[2] = (data >> 8) & 0xFF;
        buffer_4[1] = (data >> 16) & 0xFF;
        buffer_4[0] = (data >> 24) & 0xFF;
        fwrite(&buffer_4[3], 1, 1, output);
        fwrite(&buffer_4[2], 1, 1, output);
        fwrite(&buffer_4[1], 1, 1, output);
        fwrite(&buffer_4[0], 1, 1, output);  
    } else {
        buffer_4[3] = data & 0xFF;
        buffer_4[2] = (data >> 8) & 0xFF;
        buffer_4[1] = (data >> 16) & 0xFF;
        buffer_4[0] = (data >> 24) & 0xFF;
        fwrite(&buffer_4[0], 1, 1, output);
        fwrite(&buffer_4[1], 1, 1, output);
        fwrite(&buffer_4[2], 1, 1, output);
        fwrite(&buffer_4[3], 1, 1, output); 

    }
}

void write_two_bytes(FILE * output, uint16_t data, bool little_endian)
{
    uint8_t buffer_2[2];
    if(little_endian)
    {
        buffer_2[1] = data & 0xFF;
        buffer_2[0] = (data >> 8) & 0xFF;
        fwrite(&buffer_2[1], 1, 1, output);
        fwrite(&buffer_2[0], 1, 1, output); 
         
    } else {
        buffer_2[1] = data & 0xFF;
        buffer_2[0] = (data >> 8) & 0xFF;
        fwrite(&buffer_2[0], 1, 1, output);
        fwrite(&buffer_2[1], 1, 1, output);

    }
}
void write_two_bytes_other(FILE * output, uint16_t data, bool little_endian)
{
    uint8_t buffer_2[2];
    if(little_endian)
    {
        buffer_2[1] = data & 0x000000FF;
        buffer_2[0] = (data & 0x0000FF00) >>8;
        fwrite(&buffer_2[1], 1, 1, output);
        fwrite(&buffer_2[0], 1, 1, output); 
        oustide_counter++; 
    } else {
        buffer_2[1] = data & 0xFF;
        buffer_2[0] = (data >> 8) & 0xFF;
        fwrite(&buffer_2[0], 1, 1, output);
        fwrite(&buffer_2[1], 1, 1, output);

    }
}



uint8_t codeword_compression ( unsigned short sample_magnitude , short sign ) 
{
    int chord , step ;
    int codeword_tmp ;
    if( sample_magnitude & (1 << 11)) {
        chord = 0x7 ;
        step = ( sample_magnitude >> 7) & 0xF ;
        chord1++;
        
    }
    else if( sample_magnitude & (1 << 10)) {
        chord = 0x6 ;
        step = ( sample_magnitude >> 6) & 0xF ;
        chord2++;

    }
    else if( sample_magnitude & (1 << 9)) {
        chord = 0x5 ;
        step = ( sample_magnitude >> 5) & 0xF ;
        chord3++;
        
    }
    else if( sample_magnitude & (1 << 8)) {
        chord = 0x4 ;
        step = ( sample_magnitude >> 4) & 0xF ;
        chord4++;
    }
    else if( sample_magnitude & (1 << 7)) {
        chord = 0x3 ;
        step = ( sample_magnitude >> 3) & 0xF ;
        chord5++;
    }
    else if( sample_magnitude & (1 << 6)) {
        chord = 0x2 ;
        step = ( sample_magnitude >> 2) & 0xF ;
        chord6++;
       
    }
    else if( sample_magnitude & (1 << 5)) {
        chord = 0x1 ;
        step = ( sample_magnitude >> 1) & 0xF ;
        chord7++;
    }
    else {
        chord = 0x0 ;
        step = ( sample_magnitude >> 1) & 0xF ;
        chord8++;
        
    }
    codeword_tmp = ( sign << 7) | ( chord << 4) | step ;
    return (uint8_t)(codeword_tmp );
}

void set_new_file_size(FILE* output, uint32_t new_file_size)
{
    uint8_t buffer_4[4];
    fseek(output, 54 ,SEEK_SET);
    uint32_t header_size = new_file_size + 58 -8;
    uint32_t sample_length = new_file_size;
    write_four_bytes(output, header_size, true);
    
    fseek(output, 46 ,SEEK_SET);
    write_four_bytes(output, sample_length, true);



}

unsigned short get_magnitude_from_codeword(char codeword)
{
    int chord = (codeword & 0x70) >> 4;;
    int step = codeword & 0x0F;
    int magnitude;
     
    if (chord == 0x7) {
        magnitude = (1 << 6) | (step << 8) | (1 << 11);
        decomp_chord1++;
    }
    else if (chord == 0x6) {
        magnitude = (1 << 5) | (step << 7) | (1 << 10);
        decomp_chord2++;
    }
    else if (chord == 0x5) {
        magnitude = (1 << 4) | (step << 6) | (1 << 9);
        decomp_chord3++;
    }
    else if (chord == 0x4) {
        magnitude = (1 << 3) | (step << 5) | (1 << 8);
        decomp_chord4++;
    }
    else if (chord == 0x3) {
        magnitude = (1 << 2) | (step << 4) | (1 << 7);
        decomp_chord5++;
    }
    else if (chord == 0x2) {
        magnitude = (1 << 1) | (step << 3) | (1 << 6);
        decomp_chord6++;
    }
    else if (chord == 0x1) {
        magnitude = (1) | (step << 2) | (1 << 5);
        decomp_chord7++;
    }
    else {
        magnitude = 1 | (step << 1) ;
        decomp_chord8++;
    }

    return  magnitude;
}

void read_data(FILE *wav_file, FILE *output, wav_header* wav_struct)
{
    short buffer;
    short in_sample;
    short sign;
    unsigned short magnitude;
    int size = ((int)wav_struct->file_size+8-44); 
    uint8_t * data_of_file = malloc(sizeof(uint8_t) *size);
    uint8_t * compressed_codeword = malloc(sizeof(uint8_t) * size);
    //uint8_t * file_data_compressed_codeword = malloc(sizeof(char) * wav_header->file_size-36);

    //fseek(wav_file,44,SEEK_SET);

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
        fwrite( &compressed_codeword[i],sizeof(compressed_codeword[i]),1, output );
        
    }
    time_t  stop = clock();
    double compression_time = (double) (stop - start) / CLOCKS_PER_SEC;
    //set_new_file_size(output, count);
    short out_sample;
    unsigned short out_magnitude;
    short out_sign;
    int another_counter = 0;
    __uint8_t  comp_codeword;
    compressed_codeword[size+3];
     FILE * something = fopen("something.md", "wb");
    for (i=0; i < count; i++)
    {
        comp_codeword = compressed_codeword[i];
        out_sign = (comp_codeword &0x80 ) >> 7;
        out_magnitude = get_magnitude_from_codeword(comp_codeword);
        out_sample =  (short)(out_sign ? out_magnitude : -out_magnitude);
        write_two_bytes_other(output, (uint16_t)(out_sample<<3), true);
        if(ftell(something) < 4)
        {
            write_two_bytes_other(something, (uint16_t)(out_sample<<3), true);
        }
        
        another_counter++;


    }
    fclose(something);


    printf("chord count: count1: %d count2: %d count3: %d count4: %d count5:%d count6:%d count7:%d count8:%d \n", chord1, chord2, chord3, chord4, chord5, chord6, chord7, chord8);
    printf("total: %d\n", chord1+ chord2+ chord3+ chord4+ chord5+ chord6+ chord7+ chord8);
    printf("decomp chord count: count1: %d count2: %d count3: %d count4: %d count5:%d count6:%d count7:%d count8:%d \n", decomp_chord1, decomp_chord2, decomp_chord3, decomp_chord4, decomp_chord5, decomp_chord6, decomp_chord7, decomp_chord8);
    printf("total: %d\n", decomp_chord1+ decomp_chord2+ decomp_chord3+ decomp_chord4+ decomp_chord5+ decomp_chord6+ decomp_chord7+ decomp_chord8);
    printf("FILE COMPRESSED. time: %f\n", compression_time);
    printf("Iteration in compression loop: %d\n", count);
    printf("anoter counter: %d\n", another_counter);
    printf("outside counter: %d\n", oustide_counter);
    printf("size of codeword compressed: %ld\n", strlen(compressed_codeword));
    
    free(data_of_file);
    free(compressed_codeword);
}

void write_header_to_new_file(wav_header* wav_info, FILE * output)
{   
    
    uint32_t new_file_size;
    uint32_t new_data_size;



    fseek(output,0,SEEK_SET);
    //writing chunkID
    
    fwrite(&wav_info->riff, sizeof(wav_info->riff),1,output);
    // memset(buffer_4, 0, sizeof(buffer_4));
  
    
    //file size -44 + 8
    //file size + other chunks
    new_file_size = ((wav_info->file_size - 50) / 2) + 50;
    write_four_bytes(output, new_file_size, true);
   
    

    //file_type_header WAVE
    fwrite(&wav_info->file_type_header, sizeof(wav_info->file_type_header),1,output);

    //format_chunk_marker
    //set fmt
    fwrite(&wav_info->format_chunk_marker, sizeof(wav_info->format_chunk_marker),1,output);
    

    //format_data_length
    uint32_t format_data_length = 0x00000012;
    write_four_bytes(output, format_data_length, true); 
    //fwrite(&wav_info->format_data_length, sizeof(uint32_t),1,output);

    //format_type
    //fwrite(&wav_info->format_type, sizeof(uint16_t),1,output);
    uint16_t format_type = 0x0006;
    write_two_bytes(output, format_type, true);

    //num_channel
    uint16_t num_channel = wav_info->num_channel;
    write_two_bytes(output, num_channel, true);
    //fwrite(&wav_info->num_channel, sizeof(uint16_t),1,output);

    //sample_rate
    uint32_t sample_rate = wav_info->sample_rate;
    write_four_bytes(output, sample_rate, true);
   
    
    // byte rate
    uint32_t byte_rate = wav_info->byterate;
    write_four_bytes(output, byte_rate, true);
   
    
    //block align
    uint16_t block_align = wav_info->block_align;
    write_two_bytes(output, block_align, true);
    //fwrite(&wav_info->block_align, sizeof(uint32_t),1,output);

    //bits per sample
    uint16_t bits_per_sample = 0x0008;
    write_two_bytes(output, bits_per_sample, true);
    // fwrite(&wav_info->bits_per_sample, sizeof(uint16_t),1,output);

    //cbSize
    uint16_t cbSize = 0x0000;
    write_two_bytes(output, cbSize, true);

    

    //fact chunk ID
    uint32_t fact_chunk_ID = 0x66616374;
    write_four_bytes(output, fact_chunk_ID, false);

    //fact chunk size
    uint32_t fact_chunk_size = 0x00000004;
    write_four_bytes(output, fact_chunk_size, true);
   
    new_data_size = new_file_size - 50;
    // sample length
    uint32_t sample_length = new_data_size;
    write_four_bytes(output, sample_length, true);
 
    //data_header
    fwrite(&wav_info->data_header, sizeof(wav_info->data_header),1,output);
    
    
    //data size
    write_four_bytes(output, new_data_size, true);
    //fwrite(&wav_info->data_size, sizeof(uint32_t),1,output);


    

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

void write_uncompressed_header(wav_header* wav_info, FILE * output)
{
    uint32_t new_file_size;
    fseek(output,0,SEEK_SET);
    //RIFF
    fwrite(&wav_info->riff, sizeof(wav_info->riff),1,output);

    //file size and other chunks
    new_file_size = ((wav_info->file_size - 36) / 2) + 36;
    write_four_bytes(output, (wav_info->file_size - 8), true);

    //wave  ID
    fwrite(&wav_info->file_type_header, sizeof(wav_info->file_type_header),1,output);
    
    //fmt chunk ID
    fwrite(&wav_info->format_chunk_marker, sizeof(wav_info->format_chunk_marker),1,output);
    
    //fmt chunk size
    uint32_t format_data_length = 0x00000012;
    write_four_bytes(output, format_data_length, true); 

    //uncompressed format type
    uint16_t format_type = 0x0001;
    write_two_bytes(output, format_type, true);

    //channels
    uint16_t num_channel = wav_info->num_channel;
    write_two_bytes(output, num_channel, true);

    //sample_rate
    uint32_t sample_rate = wav_info->sample_rate;
    write_four_bytes(output, sample_rate, true);
   
    
    // byte rate
    uint32_t byte_rate = wav_info->byterate;
    write_four_bytes(output, byte_rate, true);
   
    
    //block align
    uint16_t block_align = wav_info->block_align;
    write_two_bytes(output, block_align, true);
    //fwrite(&wav_info->block_align, sizeof(uint32_t),1,output);

    //bits per sample
    uint16_t bits_per_sample = 0x0010;
    write_two_bytes(output, bits_per_sample, true);


    //data_header
    uint32_t data_header = 0x64617461;
    write_four_bytes(output, data_header, true);
    // fwrite(&wav_info->data_header, sizeof(wav_info->data_header),1,output);
   
   
    write_four_bytes(output, wav_info->file_size -36, true);
    


}

void copy_header_to_new_file(FILE *input, FILE * output)
{
    uint8_t buffer[44];
    fseek(input,0,SEEK_SET);
    fread(buffer, sizeof(buffer) ,1, input);
    fwrite(buffer, sizeof(buffer),1,output);
}

int main(int argc, char **argv)
{
    //FILE * wav = fopen(argv[1], "rb");
    FILE * wav = fopen("input.wav", "rb");

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
    


    copy_header_to_new_file(wav, compressed_wave);
   // write_header_to_new_file(wav_info, compressed_wave);
//    write_uncompressed_header(wav_info, compressed_wave);
    

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









