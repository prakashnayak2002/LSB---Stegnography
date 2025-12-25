#ifndef DECODE_H
#define DECODE_H

#include<stdio.h>
#include<string.h>
#include"types.h"
#include "common.h"

typedef struct DecodeInfo
{
    // Input file about
    char * input_fname;
    FILE * fptr_input;

    // output file about
    char * output_fname;
    FILE * fptr_output;
}decodeInfo;


Status read_validate_decode_args(char * argv[],decodeInfo * decInfo);

Status do_decoding(decodeInfo * decInfo);

Status open1_files(decodeInfo * decInfo );

Status open2_files(decodeInfo * decInfo);


Status skip_header(FILE *input);

Status decode_magic_string(int size,decodeInfo *decInfo);

char decode_byte_from_lsb(char data ,char*buff);

int  decode_secret_file_extn_size(decodeInfo *decInfo);

int decode_size_from_lsb(int size,char *buff);

Status decode_secret_file_extn(int size ,decodeInfo * decInfo);

Status decode_secret_file_data(int size , decodeInfo *decInfo);

int decode_secret_file_size(decodeInfo * decInfo);

#endif