
#include "decode.h"
#include<stdlib.h>

/*---------------------------------------------------------
 * Step-10 : Main decoding process
 *---------------------------------------------------------*/
Status do_decoding(decodeInfo *decInfo)
{
    if (open1_files(decInfo) == e_success)
    {
        printf("Opened input BMP file successfully\n");

        if (skip_header(decInfo->fptr_input) == e_success)
        {
            printf("Skipped 54-byte BMP header\n");

            if (decode_magic_string(strlen(MAGIC_STRING), decInfo) == e_success)
            {
                int extn_size = decode_secret_file_extn_size(decInfo);
                printf("Decoded extension size : %d\n", extn_size);

                if (decode_secret_file_extn(extn_size, decInfo) == e_success)
                {
                    printf("Decoded secret file extension\n");

                    if (open2_files(decInfo) == e_success)
                    {
                        printf("Opened output file: %s\n", decInfo->output_fname);

                        int size = decode_secret_file_size(decInfo);

                        if (decode_secret_file_data(size, decInfo) == e_success)
                        {
                            printf("Decoding of secret file completed successfully\n");
                            return e_success;
                        }
                    }
                }
            }
            else
            {
                return e_failure;
            }
        }
    }
    else
    {
        printf("Failed to open input file: %s\n", decInfo->input_fname);
        return e_failure;
    }

    return e_success;
}
/*---------------------------------------------------------
 * Step-1 : Read & Validate decode arguments
 *---------------------------------------------------------*/
Status read_validate_decode_args(char * argv[], decodeInfo *decInfo)
{
    if (strstr(argv[2], ".bmp") != NULL )
    {
        int len = strlen(argv[2]);
        char * str = strstr(argv[2] ,".bmp");
        if(*(str+4) != '\0')
        {
            printf("Reason: Please give the valid <.bmp> as extension for input file\n");
            return e_failure;
        }
        decInfo->input_fname = argv[2];

        if (argv[3] != NULL)
        {
            if (strchr(argv[3], '.') != NULL)
            {
                decInfo->output_fname = argv[3];

                int i;
                for (i = 0; decInfo->output_fname[i] != '.'; i++);
                decInfo->output_fname[i] = '\0';

                return e_success;
            }
            else
            {
                decInfo->output_fname = argv[3];
                return e_success;
            }
        }
        else
        {
            decInfo->output_fname =(char*)malloc(sizeof("output")+4);
            if(decInfo->output_fname ==NULL)
            {
                perror("ERROR: ");
                return e_failure;
            }
            strcpy(decInfo->output_fname,"output");
            return e_success;
        }
    }
    else
    {
        printf("Reason: please give the valid <.bmp> as extension for input file\n"); 
        return e_failure;
    }
}

/*---------------------------------------------------------
 * Step-2 : Opening files
 *---------------------------------------------------------*/
Status open1_files(decodeInfo * decInfo)
{
    decInfo->fptr_input = fopen(decInfo->input_fname, "r");
    if (decInfo->fptr_input == NULL)
        return e_failure;

    return e_success;
}

Status open2_files(decodeInfo * decInfo)
{
    decInfo->fptr_output = fopen(decInfo->output_fname, "w+");
    if (decInfo->fptr_output == NULL)
        return e_failure;

    return e_success;
}

/*---------------------------------------------------------
 * Step-3 : Skip BMP header (54 bytes)
 *---------------------------------------------------------*/
Status skip_header(FILE *input)
{
    fseek(input, 54, SEEK_SET);
    return e_success;
}

/*---------------------------------------------------------
 * Step-4 : LSB decode helper functions
 *---------------------------------------------------------*/
char decode_byte_from_lsb(char data, char *buff)
{
    for (int i = 0; i < 8; i++)
        data = (buff[i] & 1) | (data << 1);

    return data;
}

int decode_size_from_lsb(int size, char *buff)
{
    for (int i = 0; i < 32; i++)
        size = (buff[i] & 1) | (size << 1);

    return size;
}

/*---------------------------------------------------------
 * Step-5 : Decode Magic String
 *---------------------------------------------------------*/
Status decode_magic_string(int size, decodeInfo *decInfo)
{
    char buff[8];
    char str[size + 1];
    char data = 0;
    int i;

    for (i = 0; i < size; i++)
    {
        fread(buff, sizeof(buff), 1, decInfo->fptr_input);
        str[i] = decode_byte_from_lsb(data, buff);
    }

    str[i] = '\0';
    printf("Decoded Magic String \n");

    if (strcmp(str, MAGIC_STRING) == 0)
    {
        printf("Magic string verified successfully\n");
        return e_success;
    }
    else
    {
        printf("Magic string mismatch! Decoding failed\n");
        return e_failure;
    }
}

/*---------------------------------------------------------
 * Step-6 : Decode extension size
 *---------------------------------------------------------*/
int decode_secret_file_extn_size(decodeInfo *decInfo)
{
    char buff[32];
    int size = 0;

    fread(buff, sizeof(buff), 1, decInfo->fptr_input);
    size = decode_size_from_lsb(size, buff);

    return size;
}

/*---------------------------------------------------------
 * Step-7 : Decode extension
 *---------------------------------------------------------*/
Status decode_secret_file_extn(int size, decodeInfo * decInfo)
{
    char str[size + 1];
    char buff[8];
    char data = 0;
    int i;
    for (i = 0; i < size; i++)
    {
        fread(buff, sizeof(buff), 1, decInfo->fptr_input);
        str[i] = decode_byte_from_lsb(data, buff);
    }

    str[i] = '\0';
    printf("Decoded extension : %s\n", str);
    strcat(decInfo->output_fname,str);
    return e_success;
}

/*---------------------------------------------------------
 * Step-8 : Decode secret file size
 *---------------------------------------------------------*/
int decode_secret_file_size(decodeInfo * decInfo)
{
    int size = 0;
    char buff[32];

    fread(buff, sizeof(buff), 1, decInfo->fptr_input);
    size = decode_size_from_lsb(size, buff);

    printf("Secret file size : %d bytes\n", size);
    return size;
}

/*---------------------------------------------------------
 * Step-9 : Decode secret file data
 *---------------------------------------------------------*/
Status decode_secret_file_data(int size, decodeInfo *decInfo)
{
    char str[size + 1];
    char buff[8];
    char data = 0;
    int i;

    for (i = 0; i < size; i++)
    {
        fread(buff, sizeof(buff), 1, decInfo->fptr_input);
        str[i] = decode_byte_from_lsb(data, buff);
    }

    str[i] = '\0';

    fwrite(str, size, 1, decInfo->fptr_output);
    rewind(decInfo->fptr_output);

    printf("Secret file data decoded successfully\n");
    return e_success;
}


