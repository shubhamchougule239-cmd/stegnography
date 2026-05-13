// Heading Files
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

// Text color macros
#define RESET  "\033[0m"
#define RED    "\033[0;31m"
#define GREEN  "\033[0;32m"
#define YELLOW "\033[0;33m"
#define CYAN   "\033[0;36m"

OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
    // Variable Declaration
    EncodeInfo details;
    Decode extract;
    int check;

    // Checking the operation
    check = check_operation_type(argv[1]);

    if (check == e_decode)
    {
        printf(CYAN "==============================================\n" RESET);
        printf("                 Decoding                     \n");
        printf(CYAN "==============================================\n" RESET);

        // Checking the argument count
        if (argc == 3 || argc == 4)
        {
            // Function call for decoding
            if ((do_decoding(&extract, argv)) == e_failure)
            {
                printf(RED "❌ Decoding is incomplete\n" RESET);
                return e_failure;
            }
            else
            {
                printf(GREEN "✅ Decoding completed successfully!\n" RESET);
                printf(CYAN "==============================================\n" RESET);
                return e_success;
            }
        }
        else
        {
            // The argument is not satisfied
            printf(RED "⚠️ The argument count is not satisfied.\n" RESET);
            return e_failure;
        }
    }
    else if (check == e_encode)
    {
        // Checking the arguments
        if (argc == 4 || argc == 5)
        {
            printf(CYAN "==============================================\n" RESET);
            printf("                 Encoding                     \n");
            printf(CYAN "==============================================\n" RESET);

            check = read_and_validate_encode_args(argv, &details);
            if (check != e_success)
            {
                printf(RED "⚠️ Encoding arguments invalid\n" RESET);
                return e_failure;
            }

            // Operation function call
            check = do_encoding(&details);
            if (check == e_success)
            {
                printf(GREEN "✅ File successfully encoded!\n" RESET);
                printf(CYAN "==============================================\n" RESET);
                return e_success;
            }
            else
            {
                printf(RED "❌ Encoding failed\n" RESET);
                return e_failure;
            }
        }
        else
        {
            // The argument is not satisfied
            printf(RED "⚠️ The argument count is not satisfied.\n" RESET);
            return e_failure;
        }
    }
    else
    {
        // The invalid operation
        printf(RED "❌ The operation is invalid.\n" RESET);
        return e_failure;
    }
}

OperationType check_operation_type(char *symbol)
{
    // Checking the argument no. 2 And returning the operation type
    if (strcmp(symbol, "-e") == 0)
    {
        return e_encode;
    }
    else if (strcmp(symbol, "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}