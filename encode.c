#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

// Text color macros
#define RESET "\033[0m"
#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define CYAN "\033[0;36m"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    // printf(YELLOW"width = %u\n"RESET, width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    // printf(YELLOW"height = %u\n"RESET, height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    fseek(fptr, 0, SEEK_END);
    unsigned int size = ftell(fptr);

    // printf("%d\n",size+16+32+32+32);
    return (size);
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char *dot = strrchr(argv[2], '.');
    if (dot == NULL || strcasecmp(dot, ".bmp") != 0)
    {
        printf(RED "❌ Your Source file is invalid format (only .bmp file is supported)\n" RESET);
        return e_failure;
    }
    else
    {
        encInfo->src_image_fname = argv[2];
    }

    dot = strrchr(argv[3], '.');
    if (dot == NULL || (strcasecmp(dot, ".txt") != 0 && strcasecmp(dot, ".sh") != 0) && strcasecmp(dot, ".c") != 0)
    {
        printf(RED "❌ Your Source file is invalid format (only .txt, .sh, or .c files are supported)\n" RESET);
        return e_failure;
    }
    else
    {
        encInfo->secret_fname = argv[3];
    }

    int i = 0;
    while (dot[i] != '\0')
    {
        encInfo->extn_secret_file[i] = dot[i];
        i++;
    }
    encInfo->extn_size = strlen(encInfo->extn_secret_file);

    if (argv[4] == NULL)
    {
        char *str = "very_beautiful.bmp";
        encInfo->dest_image_fname = str;
    }
    else
    {
        dot = strrchr(argv[4], '.');
        if (dot == NULL || strcasecmp(dot, ".bmp") != 0)
        {
            printf(RED "❌ Your output file is invalid format (only .bmp file is supported)\n" RESET);
            return e_failure;
        }
        else
        {
            encInfo->dest_image_fname = argv[4];
        }
    }

    // Success message added (non-intrusive)
    printf(GREEN "✅ Arguments validated successfully!\n" RESET);

    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED "❌ ERROR: Unable to open file %s\n" RESET, encInfo->src_image_fname);
        return e_failure;
    }
    
    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED "❌ ERROR: Unable to open file %s\n" RESET, encInfo->secret_fname);
        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_dest_image = fopen(encInfo->dest_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_dest_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED "❌ ERROR: Unable to open file %s\n" RESET, encInfo->dest_image_fname);
        return e_failure;
    }

    // Success message added (non-intrusive)
    printf(GREEN "✅ All files opened successfully!\n" RESET);

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    unsigned int bmp_size = get_image_size_for_bmp(encInfo->fptr_src_image);
    unsigned int secret_size = get_file_size(encInfo->fptr_secret);
    encInfo->size_secret_file = secret_size;

    if (bmp_size < (secret_size + 16 + 32 + ((encInfo->extn_size) * 8 + 32)))
    {
        printf(RED "❌ The secret message is very large for the image!\n" RESET);
        return e_failure;
    }

    // Success message added (non-intrusive)
    printf(GREEN "✅ Capacity check passed successfully!\n" RESET);

    return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image);
    rewind(fptr_dest_image);

    char buffer[54];

    fread(buffer, 1, 54, fptr_src_image);
    fwrite(buffer, 1, 54, fptr_dest_image);

    if (ftell(fptr_src_image) != ftell(fptr_dest_image))
    {
        printf(RED "❌ The Copying header file is unsuccessful\n" RESET);
        return e_failure;
    }
    else
    {
        printf(GREEN "✅ The Copying header file is successful\n" RESET);
    }

    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char buffer[8];
    for (int i = 0; i < strlen(magic_string); i++)
    {
        fread(buffer, 1, 8, encInfo->fptr_src_image);
        // printf("%c",magic_string[i]);
        encode_byte_to_lsb(magic_string[i], buffer);
        fwrite(buffer, 1, 8, encInfo->fptr_dest_image);
    }

    if ((ftell(encInfo->fptr_src_image)) != (ftell(encInfo->fptr_dest_image)))
    {
        printf(RED "❌ File pointers are not at the same position\n" RESET);
        return e_failure;
    }

    // Success message added (non-intrusive)
    printf(GREEN "✅ Magic string encoded successfully!\n" RESET);

    return e_success;
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(size, buffer);
    fwrite(buffer, 1, 32, encInfo->fptr_dest_image);

    if ((ftell(encInfo->fptr_src_image)) != (ftell(encInfo->fptr_dest_image)))
    {
        printf(RED "❌ File pointers are not at the same position\n" RESET);
        return e_failure;
    }

    // Success message added
    printf(GREEN "✅ Secret file extension size encoded successfully!\n" RESET);

    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char buffer[8];
    for (int i = 0; i < strlen(file_extn); i++)
    {
        fread(buffer, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], buffer);
        fwrite(buffer, 1, 8, encInfo->fptr_dest_image);
    }

    if ((ftell(encInfo->fptr_src_image)) != (ftell(encInfo->fptr_dest_image)))
    {
        printf(RED "❌ File pointers are not at the same position\n" RESET);
        return e_failure;
    }

    // Success message added
    printf(GREEN "✅ Secret file extension encoded successfully!\n" RESET);

    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, buffer);
    fwrite(buffer, 1, 32, encInfo->fptr_dest_image);

    if ((ftell(encInfo->fptr_src_image)) != (ftell(encInfo->fptr_dest_image)))
    {
        printf(RED "❌ File pointers are not at the same position\n" RESET);
        return e_failure;
    }

    // Success message added
    printf(GREEN "✅ Secret file size encoded successfully!\n" RESET);

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);
    char secret_data[encInfo->size_secret_file];
    fread(secret_data, 1, encInfo->size_secret_file, encInfo->fptr_secret);

    char buffer[8];
    for (int i = 0; i < encInfo->size_secret_file; i++)
    {
        fread(buffer, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(secret_data[i], buffer);
        fwrite(buffer, 1, 8, encInfo->fptr_dest_image);
    }

    if ((ftell(encInfo->fptr_src_image)) != (ftell(encInfo->fptr_dest_image)))
    {
        printf(RED "❌ File pointers are not at the same position\n" RESET);
        return e_failure;
    }

    // Success message added
    printf(GREEN "✅ Secret file data encoded successfully!\n" RESET);

    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    int ch;
    while ((ch = fgetc(fptr_src)) != EOF)
    {
        fputc(ch, fptr_dest);
    }

    if ((ftell(fptr_src)) != (ftell(fptr_dest)))
    {
        printf(RED "❌ File pointers are not at the same position\n" RESET);
        return e_failure;
    }

    // Success message added
    printf(GREEN "✅ Remaining image data copied successfully!\n" RESET);

    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        char data_bit = (data >> i) & 1;
        image_buffer[i] = image_buffer[i] & (~1);
        image_buffer[i] = image_buffer[i] | data_bit;
    }

    return e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    for (int i = 0; i < 32; i++)
    {
        char size_bit = (size >> i) & 1;
        imageBuffer[i] = imageBuffer[i] & (~1);
        imageBuffer[i] = imageBuffer[i] | size_bit;
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    /* Function call for Get File pointers for i/p and o/p files */
    if ((open_files(encInfo)) == e_failure)
    {
        return e_failure;
    }
    /* Function call for check capacity */
    else if ((check_capacity(encInfo)) == e_failure)
    {
        return e_failure;
    }
    /* Function call for Copy bmp image header */
    else if ((copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_dest_image)) == e_failure)
    {
        return e_failure;
    }
    /* Function call for Store Magic String */
    else if ((encode_magic_string(MAGIC_STRING, encInfo)) == e_failure)
    {
        return e_failure;
    }
    /*Function call for Encode extension size*/
    else if ((encode_secret_file_extn_size(encInfo->extn_size, encInfo)) == e_failure)
    {
        return e_failure;
    }
    /* Function call for Encode secret file extenstion */
    else if ((encode_secret_file_extn(encInfo->extn_secret_file, encInfo)) == e_failure)
    {
        return e_failure;
    }
    /* Function call for Encode secret file size */
    else if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        return e_failure;
    }
    /* Function call for Encode secret file data*/
    else if (encode_secret_file_data(encInfo) == e_failure)
    {
        return e_failure;
    }
    /* Function call for Copy remaining image bytes from src to stego image after encoding */
    else if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_dest_image) == e_failure)
    {
        return e_failure;
    }
    else
    {
        return e_success;
    }
}
