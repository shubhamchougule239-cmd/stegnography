// Header file
#ifndef DECODE_H
#define DECODE_H
#include<stdio.h>
#include "types.h"


typedef struct decode
{
    /* Source Image info */
    char *src_decode;   // To store the src image name
    FILE *fptr_src;     // To store the address of the src image

    /* Extation size and scerete size info */
    int ext_size;      // To store the extation size
    char ext_file[6];  // To store the extation name
    int secert_size;   // To store the scerete size

   /* Destination File Info */
    char dest_text_name[50];    // To store the dest file name
    FILE *fptr_dest;            // To store the address of scerete file
}Decode;    //Structure name 

/* decoding function prototype */

/* Perform the decoding */
Status do_decoding(Decode *de_code,char *argv[]);

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], Decode *de_code);

/* Get File pointers for i/p  files */
Status source_open_files(Decode *de_code);

/* Get File pointers for  o/p files */
Status desti_open_files(Decode *de_code);

/* Decode Magic String */
Status decode_magic_string(const char *magic_string, Decode *de_code);

/*Decode extension size*/
Status decode_secret_file_extn_size( Decode *de_code);

/* Decode secret file extenstion */
Status decode_secret_file_extn(Decode *de_code);

/* Decode secret file size */
Status decode_secret_file_size( Decode *de_code);

/* Decode secret file data*/
Status decode_secret_file_data(Decode *de_code);

/* Decode a byte into LSB of character data  */
unsigned char decode_byte_to_lsb(char *buffer_char);

// Decode a size to lsb
int decode_size_to_lsb( char *imageBuffer);

#endif