#include <stdio.h>
#include<string.h>
#include "decode.h"
#include "types.h"


Status read_and_validate_decode_args(char *argv[], DecInfo *decInfo)
{
    if(argv[2] == NULL)
    {
        printf("nothing is given\n");
        return e_failure;
    }
    else if(strstr(argv[2],".bmp") == NULL)
    {
        printf(".bmp is not given\n");
        return e_failure;
    }
    decInfo->stego_fname = argv[2];

    if(argv[3] == NULL)
    {
        printf("no file given so create one to get secret\n");
        strcpy(decInfo->output_fname , "output");
    }
    else if(strstr(argv[3],".") != NULL)
    {
        printf("o/p file should not have extension\n");
        return e_failure;
    }
    else
    {
        strcpy(decInfo->output_fname,argv[3]);

    }

    printf("decoding\n");
    return e_success;
}


Status do_decoding(DecInfo *decInfo)
{
    decInfo->fptr_stego = fopen(decInfo->stego_fname,"r");
    if(decInfo->fptr_stego == NULL)
    {
        printf("file is not opened\n");
        return e_failure;
    }


    printf("Before %ld\n",ftell(decInfo->fptr_stego));
    fseek(decInfo->fptr_stego,54,SEEK_SET);
    printf("After %ld\n",ftell(decInfo->fptr_stego));

    char decode_ms[10];
    decode_magic_string(decode_ms,decInfo);
    char magic_string[10];
    printf("Enter the magic string : \n");
    scanf("%s",magic_string);
    printf("%s\n",decode_ms);
    if(strcmp(magic_string,decode_ms)!=0)
    {
        printf("Magic string is incorrect\n");
        return e_failure;
    }

    printf("Magic string is : %s\n",decode_ms);


    decode_extn_size(decInfo);
    
    
    decode_extn(decInfo);
    
    
    strcat(decInfo->output_fname,decInfo->extn);
    
    
    decInfo->fptr_output = fopen(decInfo->output_fname,"w");
    
    
    if(decInfo->fptr_output == NULL)
    {
        printf("Failed to open output file\n");
        return e_failure;
    }
    
    
    decode_file_size(decInfo);
    
    
    decode_file_data(decInfo);
    
    
    fclose(decInfo->fptr_stego);
    
    fclose(decInfo->fptr_output);
    
    return e_success;

  
}

Status decode_magic_string(char *decode_magic_string,DecInfo *decInfo)
{
    int i;

    char temp[8];
    
    for(i=0; i<2; i++)
    {
        fread(temp,1,8,decInfo->fptr_stego);
        decode_magic_string[i] = decode_lsb_to_byte(temp);
    }
    
    decode_magic_string[i] = '\0';
    
}


char decode_lsb_to_byte(char * buffer)
{
    unsigned char ch=0;
    
    for(int i=0; i<8; i++)
    {
        ch = ch<<1;
        ch = ch |(buffer[i] & 1);
    }
    return ch;
    
}


int decode_lsb_to_size(char * buffer)
{
    unsigned char n=31,get;
    int i,size=0;
    
    for(i=0; i<=31; i++)
    {
        get = buffer[i] & 0x00000001;
        get = get << n;
        size = size | get;
        n--;
    }
    return size;
}


Status decode_extn_size(DecInfo *decInfo)
{
    char temp[32];
    fread(temp,1,32,decInfo->fptr_stego);
    decInfo->extn_size = decode_lsb_to_size(temp);
}


Status decode_extn(DecInfo *decInfo)
{
    char temp[8];
    int i;
    for( i=0;i<decInfo->extn_size;i++)
    {
        fread(temp,1,8,decInfo->fptr_stego);
        decInfo->extn[i]=decode_lsb_to_byte(temp);
    }

    decInfo->extn[i] = '\0';
    
}


Status decode_file_size(DecInfo *decInfo)
{
    char temp[32];
    fread(temp,1,32,decInfo->fptr_stego);
    decInfo->file_size = decode_lsb_to_size(temp);
    printf("%d\n",decInfo->file_size);
}


Status decode_file_data(DecInfo *decInfo)
{
    char temp[8];
    char ch;
    for(int i=0;i<decInfo->file_size;i++)
    {
        fread(temp,1,8,decInfo->fptr_stego);
        ch = decode_lsb_to_byte(temp);
        fprintf(decInfo->fptr_output,"%c",ch);
    }
}