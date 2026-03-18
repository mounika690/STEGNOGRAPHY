#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
  Structure to store information required for
  encoding secret file to source Image
  Info about output and intermediate data is
  also stored
 */

typedef struct DECINFO
{
    //stego info
    char *stego_fname;
    FILE *fptr_stego;


    //output file info
    char output_fname[20];
    FILE *fptr_output ;

    //decode things
    int extn_size;
    char extn[10];
    int file_size;
}DecInfo;


/*decode function prototypes*/

//read and validate Decode 
Status read_and_validate_decode_args(char *argv[],DecInfo *decInfo);

//do decoding
Status do_decoding(DecInfo *decInfo);


Status decode_magic_string(char *decode_magic_string,DecInfo *decInfo);


char decode_lsb_to_byte(char * buffer);


int decode_lsb_to_size(char * buffer);


Status decode_extn_size(DecInfo *decInfo);


Status decode_extn(DecInfo *decInfo);


Status decode_file_size(DecInfo *decInfo);


Status decode_file_data(DecInfo *decInfo);
#endif