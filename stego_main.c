#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;

    /* Validate command-line argument count */
    if (argc < 3 || argc > 5)
    {
        printf("\n❌ Invalid Command Line Arguments!\n");
        printf("Usage:\n");
        printf("  Encode → ./a.out -e <src_img> <secret_file> <stego_img>\n");
        printf("  Decode → ./a.out -d <stego_img> <output_file>\n\n");
        return 0;
    }

    /* Check whether user wants Encode or Decode */
    if (check_operation_type(argv) == e_encode)
    {
        printf("\n==============================\n");
        printf("🔐 MODE SELECTED : ENCODING\n");
        printf("==============================\n");

        /* Step 1: Validate encoding arguments */
        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("✔ Arguments validated successfully\n");

            /* Step 2: Perform encoding */
            if (do_encoding(&encInfo) == e_success)
            {
                printf("🎉 Encoding completed successfully!\n");

                fclose(encInfo.fptr_src_image);
                fclose(encInfo.fptr_stego_image);
                // fclose(encInfo.fptr_secret);
            }
            else
            {
                printf("❌ ERROR: Encoding failed!\n");
            }
        }
        else
        {
            printf("❌ ERROR: Invalid encoding arguments!\n");
        }
    }

    /* ------------------ DECODE SECTION ------------------ */
    else if (check_operation_type(argv) == e_decode)
    {
        printf("\n==============================\n");
        printf("🔓 MODE SELECTED : DECODING\n");
        printf("==============================\n");

        decodeInfo decInfo;

        /* Step 1: Validate decode args */
        if (read_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf("✔ Decode arguments validated\n");

            /* Step 2: Perform decoding */
            if (do_decoding(&decInfo) == e_success)
            {
                printf("🎉 Decoding completed successfully!\n");

                // /* Compare decoded output with original secret */
                // FILE *fp = fopen("backup.txt", "r");
                // if (fp == NULL)
                // {
                //     printf("❌ Unable to open backup.txt for comparison\n");
                //     return e_failure;
                // }

                // char str1[20];
                // fscanf(fp, "%s", str1);

                // encInfo.secret_fname = str1;
                // encInfo.fptr_secret = fopen(encInfo.secret_fname, "r");

                // if (encInfo.fptr_secret == NULL)
                // {
                //     perror("fopen");
                //     printf("❌ Unable to open original secret file %s\n", encInfo.secret_fname);
                //     return e_failure;
                // }

                // /* Compare character-by-character */
                // char ch, ch1;
                // char str[200], str2[100];
                // int i = 0;

                // while ((ch = fgetc(encInfo.fptr_secret)) != EOF &&(ch1 = fgetc(decInfo.fptr_output)) != EOF)
                // {
                //     str[i] = ch;
                //     str2[i] = ch1;
                //     i++;
                // }

                // str[i] = '\0';
                // str2[i] = '\0';

                // printf("\n--------------- FILE COMPARISON ---------------\n");

                // if (strcmp(str, str2) == 0)
                //     printf("✔ Encoded and Decoded information MATCHES! 🎉\n");
                // else
                //     printf("❌ Encoded and Decoded information does NOT match!\n");

                // printf("-----------------------------------------------\n");
            }
        }
        else
        {
            printf("❌ ERROR: Invalid decode arguments!\n");
        }
    }

    /* Invalid argument (neither encode nor decode) */
    else
    {
        printf("\n❌ ERROR: Invalid operation type: %s\n", argv[1]);
    }

    return 0;
}
