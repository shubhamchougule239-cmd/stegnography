// Heading Files
#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

// Text color macros
#define RESET "\033[0m"
#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define CYAN "\033[0;36m"

Status do_decoding(Decode *de_code, char *argv[])
{
    
    /* Function call for Read and validate Decode args from argv */
    if (read_and_validate_decode_args(argv, de_code) == e_failure)
    {
        return e_failure;
    }
    /* Function call for Get File pointers for i/p  files */
    else if (source_open_files(de_code) == e_failure)
    {
        return e_failure;
    }
    /* Function call for Decode Magic String */
    else if (decode_magic_string(MAGIC_STRING, de_code) == e_failure)
    {
        return e_failure;
    }
    /*Function call for Decode extension size*/
    else if (decode_secret_file_extn_size(de_code) == e_failure)
    {
        return e_failure;
    }
    /*Function call for Decode secret file extenstion */
    else if (decode_secret_file_extn(de_code) == e_failure)
    {
        return e_failure;
    }
    /*Function call for Get File pointers for  o/p files */ 
    else if (desti_open_files(de_code) == e_failure)
    {
        return e_failure;
    }
     /*Function call for Decode secret file size */
    else if (decode_secret_file_size(de_code) == e_failure)
    {
        return e_failure;
    }
    /*Function call for Decode secret file data*/
    else if (decode_secret_file_data(de_code) == e_failure)
    {
        return e_failure;
    }
    
    
}

Status read_and_validate_decode_args(char *argv[], Decode *de_code)
{
    char *dot = strrchr(argv[2], '.');
    if (dot == NULL || strcasecmp(dot, ".bmp") != 0)
    {
        printf(RED "❌ Your Source file is invalid format (only .bmp file is supported)\n" RESET);
        return e_failure;
    }
    else
    {
        de_code->src_decode = argv[2];
    }

    if (argv[3] == NULL)
    {
        strcpy(de_code->dest_text_name, "default");
    }
    else
    {
        dot = strrchr(argv[3], '.');
        char temp[50];
        int i = 0;
        while (&argv[3][i] != dot && argv[3][i] != '\0')
        {
            temp[i] = argv[3][i];
            i++;
        }
        temp[i] = '\0';
        // printf("%s", temp);
        strcpy(de_code->dest_text_name, temp);
    }

    // Success message added
    printf(GREEN "✅ Decode arguments validated successfully!\n" RESET);

    return e_success;
}

Status source_open_files(Decode *de_code)
{
    // Src Image file
    de_code->fptr_src = fopen(de_code->src_decode, "r");
    // Do Error handling
    if (de_code->fptr_src == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED "❌ ERROR: Unable to open file %s\n" RESET, de_code->src_decode);
        return e_failure;
    }

    fseek(de_code->fptr_src, 54, SEEK_SET);

    // Success message added
    printf(GREEN "✅ Source image file opened successfully!\n" RESET);

    return e_success;
}

Status desti_open_files(Decode *de_code)
{
    strcat(de_code->dest_text_name, de_code->ext_file);

    // Destination text file
    de_code->fptr_dest = fopen(de_code->dest_text_name, "w");
    // Do Error handling
    if (de_code->fptr_dest == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED "❌ ERROR: Unable to open file %s\n" RESET, de_code->dest_text_name);
        return e_failure;
    }

    // Success message added
    printf(GREEN "✅ Destination text file opened successfully!\n" RESET);

    return e_success;
}

Status decode_magic_string(const char *magic_string, Decode *de_code)
{
    char buffer[8];
    unsigned char ch = 0;

    for (int i = 0; i < 2; i++)
    {
        fread(buffer, 1, 8, de_code->fptr_src);
        ch = decode_byte_to_lsb(buffer);
        // printf("%c", ch);

        if (ch != magic_string[i])
        {
            printf(RED "❌ The magic string does not match\n" RESET);
            return e_failure;
        }
    }

    // Success message added
    printf(GREEN "✅ Magic string decoded successfully!\n" RESET);

    return e_success;
}

Status decode_secret_file_extn_size(Decode *de_code)
{
    char buffer[32];

    fread(buffer, 1, 32, de_code->fptr_src);
    de_code->ext_size = decode_size_to_lsb(buffer);

    if (de_code->ext_size == 0)
    {
        printf(RED "❌ The extension size is 0\n" RESET);
        return e_failure;
    }

    // Success message added
    printf(GREEN "✅ Secret file extension size decoded successfully!\n" RESET);

    return e_success;
}

Status decode_secret_file_extn(Decode *de_code)
{
    int i;
    char buffer[8];
    for (i = 0; i < de_code->ext_size; i++)
    {
        fread(buffer, 1, 8, de_code->fptr_src);
        de_code->ext_file[i] = decode_byte_to_lsb(buffer);
    }
    de_code->ext_file[i] = '\0';

    // Success message added
    printf(GREEN "✅ Secret file extension decoded successfully!\n" RESET);

    return e_success;
}

Status decode_secret_file_size(Decode *de_code)
{
    char buffer[32];
    fread(buffer, 1, 32, de_code->fptr_src);
    de_code->secert_size = decode_size_to_lsb(buffer);

    if (de_code->secert_size == 0)
    {
        printf(RED "❌ The secret file size is 0\n" RESET);
        return e_failure;
    }

    // Success message added
    printf(GREEN "✅ Secret file size decoded successfully!\n" RESET);

    return e_success;
}

Status decode_secret_file_data(Decode *de_code)
{
    char ch, buffer[8];
    for (int i = 0; i < de_code->secert_size; i++)
    {
        fread(buffer, 1, 8, de_code->fptr_src);
        ch = decode_byte_to_lsb(buffer);
        putc(ch, de_code->fptr_dest);
    }

    // Success message added
    printf(GREEN "✅ Secret file data decoded successfully!\n" RESET);

    return e_success;
}

unsigned char decode_byte_to_lsb(char *buffer_char)
{
    unsigned char ch = 0,bit;

    for (int i = 0; i < 8; i++)
    {
        bit = buffer_char[i] & 1;
        ch = ch | ( bit << i );
        // ch = (ch << 1);
    }
    return ch;
}

int decode_size_to_lsb(char *imageBuffer)
{
    int size = 0;
    int bit ;
    for (int i = 0; i < 32; i++)
    {
        bit = imageBuffer[i] & 1;
        size = size |( bit << i);
    }
    return size;
}