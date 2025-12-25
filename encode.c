#include<stdio.h>
#include "encode.h"
#include "common.h"
#include "types.h"

// decoding function
Status do_encoding(EncodeInfo *encInfo)
{
    printf("Opening required files...\n");
    if(open_files(encInfo) == e_success)
    {
        printf("Files opened successfully.\n");
        printf("Checking image capacity...\n");

        if(check_capacity(encInfo) == e_success)
        {
            printf("Image has enough capacity to encode the secret file.\n");
            printf("Copying BMP header (54 bytes)...\n");

            if(copy_bmp_header(encInfo->fptr_src_image , encInfo->fptr_stego_image) == e_success)
            {
                printf("BMP header copied successfully.\n");
                printf("Encoding magic string...\n");

                if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
                {
                    printf("Magic string encoded successfully!\n");
                    printf("Encoding secret file extension size...\n");

                    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)==e_success)
                    {
                        printf("Secret file extension size encoded successfully.\n");
                        printf("Encoding secret file extension...\n");

                        if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo)==e_success)
                        {
                            printf("Secret file extension encoded successfully.\n");
                            printf("Encoding secret file size...\n");

                            if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)
                            {
                                printf("Secret file size encoded successfully.\n");
                                printf("Encoding secret file data...\n");

                                if(encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("Secret file data encoded successfully.\n");
                                    printf("Copying remaining image data...\n");

                                    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
                                    {
                                        printf("Remaining data copied successfully.\n");
                                        printf("Encoding completed successfully!\n");
                                        return e_success;
                                    }

                                }

                            }
                        }

                    }

                }

            }

        }
        else
        {
            printf("Error: Image does not have enough capacity to hide the secret file.\n");
            return e_failure;
        }

    }
    else
    {
        printf("Error: Unable to open required files.\n");
        return e_failure;
    }
    
}

/* ------------------ ENCODING STEPS ------------------ */

// 1st encoding step (Magic String)
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char data[strlen(magic_string)];
    strcpy(data,magic_string);
    if(encode_data_to_image(data, strlen(data), encInfo->fptr_src_image, encInfo->fptr_stego_image) ==e_success);
    return e_success;
}

Status encode_data_to_image(char *data , int size , FILE * src ,FILE *stego)
{
    char image_buffer[8];
    for(int i=0;i<size;i++)
    {
        fread(image_buffer,sizeof(image_buffer),1,src);
        if(encode_byte_to_lsb(data[i],image_buffer)==e_success)
        fwrite(image_buffer,sizeof(image_buffer),1,stego);
    }
    return e_success;

}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0 ;i<8 ;i++)
    {
        image_buffer[i] = (image_buffer[i] & ~(1)) |  (data >> (7-i) & 1);
    }
    return e_success;
}

Status encode_size_to_lsb(long exten_size ,char *extension)
{
    for(int i=0;i<32;i++)
    {
        extension[i] = (extension[i] & ~(1)) | (exten_size >> (31-i)&1);
    }
    return e_success;
}

// 2nd step of encoding
Status encode_secret_file_extn_size(long  exten_size, EncodeInfo *encInfo)
{
    char extension[32];
    fread(extension,sizeof(extension),1,encInfo->fptr_src_image);
    if(encode_size_to_lsb(exten_size,extension)==e_success)
    {
        fwrite(extension,sizeof(extension),1,encInfo->fptr_stego_image);
        return e_success ;
    }
}

// 3rd step of encoding 
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char extn[8];
    for(int i=0 ;file_extn[i] != '\0' ;i++)
    {
        fread(extn,sizeof(extn),1,encInfo->fptr_src_image);
        if(encode_byte_to_lsb(file_extn[i],extn) == e_success)
        fwrite(extn,sizeof(extn),1,encInfo->fptr_stego_image);
    }
    return e_success;
}

// 4th step of encoding
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buff[32];
    fread(buff,sizeof(buff),1,encInfo->fptr_src_image);
    if(encode_size_to_lsb(file_size,buff) == e_success)
    fwrite(buff,sizeof(buff),1,encInfo->fptr_stego_image);
    return e_success;
}

//5th step of encoding (Secret Data)
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char buff[encInfo->size_secret_file];
    char buff1[8];
    int i;
    rewind(encInfo->fptr_secret);
    fread(buff,sizeof(buff),1,encInfo->fptr_secret);
    for(i=0;i<sizeof(buff);i++)
    {

        fread(buff1,sizeof(buff1),1,encInfo->fptr_src_image);
        if(encode_byte_to_lsb(buff[i],buff1) == e_success)
        
        fwrite(buff1,sizeof(buff1),1,encInfo->fptr_stego_image);
    }
    
    return e_success;

}

// 6th step of encode
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while(fread(&ch,sizeof(char),1,fptr_src)>0)
    {
        fwrite(&ch,sizeof(char),1,fptr_dest);
    }
    return e_success;
}

/* ------------------ ARGUMENT VALIDATION ------------------ */

OperationType check_operation_type(char *argv[]) 
{
    if(strstr(argv[1],"-e") != NULL)
    {
        return e_encode;
    }
    else if(strstr(argv[1],"-d") != NULL)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strstr(argv[2] ,".bmp") !=NULL) 
    {
        
        int len = strlen(argv[2]);
        char * str1 = strstr(argv[2],".bmp");
        if(*(str1+4) != '\0')
        {
            printf("Reason: please give the valid extension for input file <.bmp> \n");
            return e_failure ;
        }
        encInfo->src_image_fname = argv[2];
        char *str;
        if(argv[3] ==NULL)
        return e_failure;
        if(strstr(argv[3],".") !=NULL)
        {
            str = strstr(argv[3],".");

            for(int i=0 ;str[i] != '\0';i++ )
            {
                encInfo->extn_secret_file[i]=str[i];
            }
	    printf("extn = %s\n", encInfo->extn_secret_file);


            encInfo->secret_fname = argv[3];

            if(argv[4]!=NULL)
            {
                if(strstr(argv[4],".bmp")!=NULL)
                {
                    int len = strlen(argv[4]);
                    char * str1 = strstr(argv[4],".bmp");
                    if(*(str1+4) != '\0')
                    {
                        printf("Reason: please give the valid extension for output file <.bmp> \n");
                        return e_failure ;
                    }

                    encInfo->stego_image_fname = argv[4];
                }
                else
                {
                    return e_failure;
                }
            }
            else
            {
                encInfo->stego_image_fname="stego.bmp";
            }
            return e_success; 
        }
        else
        {
            printf("Reason: please give the valid secret file name '.'should be there\n");
            return e_failure;
        }
    }
    else
    {
        printf("Reason: Please give the valid file name <.bmp> as a extension\n");
        return e_failure;
    }
}

/* ------------------ FILE OPEN ------------------ */

Status open_files(EncodeInfo *encInfo)
{
    printf("Opening source image: %s\n", encInfo->src_image_fname);
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        printf("Error: Unable to open source image %s\n", encInfo->src_image_fname);
        return e_failure;
    }

    printf("Opening secret file: %s\n", encInfo->secret_fname);
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        printf("Error: Unable to open secret file %s\n", encInfo->secret_fname);
        return e_failure;
    }

    printf("Creating stego image: %s\n", encInfo->stego_image_fname);
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        printf("Error: Unable to create stego image %s\n", encInfo->stego_image_fname);
        return e_failure;
    }

    return e_success;
}

/* ------------------ CAPACITY CHECK ------------------ */

Status check_capacity(EncodeInfo *encInfo)
{
    printf("Calculating image capacity...\n");

    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    printf("Secret file size: %ld bytes\n",encInfo->size_secret_file);

    if(encInfo->image_capacity > ((strlen(MAGIC_STRING)*8)+sizeof(encInfo->extn_secret_file)*8+sizeof(encInfo->extn_secret_file)*8+encInfo->size_secret_file*8+sizeof((encInfo->size_secret_file))*8 ))
    {
        return e_success;
    }
    else
    {
        printf("Error: Not enough capacity to hide the secret file.\n");
        return e_failure;
    }
}

/* ------------------ BMP SIZE ------------------ */

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    fseek(fptr_image, 18, SEEK_SET);

    fread(&width, sizeof(int), 1, fptr_image);
    printf("Image width = %u\n", width);

    fread(&height, sizeof(int), 1, fptr_image);
    printf("Image height = %u\n", height);

    rewind(fptr_image);

    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

/* ------------------ COPY HEADER ------------------ */

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char ch[54];
    fread(ch,sizeof(ch),1,fptr_src_image);
    fwrite(ch,sizeof(ch),1,fptr_dest_image);
    return e_success;
}
