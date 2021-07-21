#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
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
int main(int argc, char **argv){
    FILE * fp = argv[1];
    char 
    if(strcmp(argv[2],"compress"){
        printf("=== COMPRESSING ===\n");
    } else if (strcmp(argv[2], "expand")){
        printf("=== EXPANDING ===\n");
    } else{
        printf("wrong command\n");
        exit(1);
    }
    exit(0);
}
char codeword_compression ( unsigned int sample_magnitude , int sign ) {
    int chord , step ;
    int codeword_tmp ;
    if( sample_magnitude & (1 << 11)) {
        chord = 0 x7 ;
        step = ( sample_magnitude >> 8) & 0 xF ;
        codeword_tmp = ( sign << 7) & ( chord << 4) & step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 11)) {
        chord = 0 x6 ;
        step = ( sample_magnitude >> 7) & 0 xF ;
        codeword_tmp = ( sign << 7) & ( chord << 4) & step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 10)) {
        chord = 0 x5 ;
        step = ( sample_magnitude >> 6) & 0 xF ;
        codeword_tmp = ( sign << 7) & ( chord << 4) & step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 9)) {
        chord = 0 x4 ;
        step = ( sample_magnitude >> 5) & 0 xF ;
        codeword_tmp = ( sign << 7) & ( chord << 4) & step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 8)) {
        chord = 0 x3 ;
        step = ( sample_magnitude >> 4) & 0 xF ;
        codeword_tmp = ( sign << 7) & ( chord << 4) & step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 7)) {
        chord = 0 x2 ;
        step = ( sample_magnitude >> 3) & 0 xF ;
        codeword_tmp = ( sign << 7) & ( chord << 4) & step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 6)) {
        chord = 0 x1 ;
        step = ( sample_magnitude >> 2) & 0 xF ;
        codeword_tmp = ( sign << 7) & ( chord << 4) & step ;
        return ( (char) codeword_tmp );
    }
    if( sample_magnitude & (1 << 5)) {
        chord = 0 x0 ;
        step = ( sample_magnitude >> 1) & 0 xF ;
        codeword_tmp = ( sign << 7) & ( chord << 4) & step ;
        return ( (char) codeword_tmp );
    }
}


