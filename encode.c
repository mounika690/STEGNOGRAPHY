#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
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
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
    printf("\n");
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

OperationType check_operation_type(char *argv[])
{
    if(argv[1] == NULL)
    {
        printf("unsupported input\n");
        return e_unsupported;
    }
    else if(strcmp(argv[1],"-e")==0)
    {
        printf("encoding will start\n");
        return e_encode;
    }
    else if(strcmp(argv[1],"-d")==0)
    {
        printf("decoding will start\n");
        return e_decode;
    }
    

    return e_unsupported;
}


Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(argv[2] == NULL)
    {
        printf("no input is given\n");
        return e_failure;
    } 
    else if(strstr(argv[2],".bmp")==NULL)
    {
        printf(".bmp file is not there\n");
        return e_failure;
    }
    encInfo->src_image_fname = argv[2];
    printf("%s\n",encInfo->src_image_fname);


    if(argv[3] == NULL)
    {
        printf("no input is given\n");
        return e_failure;
    } 
    else if(strstr(argv[3],".")==NULL)
    {
        printf(".txt/.c file is not there\n");
        return e_failure;
    }
    encInfo->secret_fname = argv[3];
    printf("%s\n",encInfo->secret_fname);


    if(argv[4] == NULL)
    {
        encInfo->stego_image_fname = "stego.bmp";
        printf("%s\n", encInfo->stego_image_fname );
    } 
    else if(strstr(argv[4],".bmp") == NULL)
    {
        printf(".bmp file is not there\n");
        return e_failure;
    }
    encInfo->stego_image_fname = argv[4];
    printf("%s\n",encInfo->stego_image_fname);

    return e_success;
    
}

Status do_encoding(EncodeInfo *encInfo)
{
    if(check_capacity(encInfo) == e_failure)
    {
        printf("source file have less byte\n");
        return e_failure;
    }

    rewind(encInfo->fptr_src_image);

    printf("\n");
    //copy bmp header
    copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    
    //encode magic string
   
    encode_magic_string(MAGIC_STRING,encInfo);
    
    //encode extension size
    encode_extn_file_size(encInfo->extn_size,encInfo);
    
    //encode secret file extension
    char *extn = strstr(encInfo->secret_fname,".");
    encode_secret_file_extn(extn, encInfo);

    //encode secret file size extension
    encode_secret_file_size(encInfo->sec_file_size,encInfo);
    
    //encode secrete file data
    encode_secret_file_data(encInfo);

    //copying remaining image as it as 
    copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    
    //closing all files
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_stego_image);
    fclose(encInfo->fptr_secret);


    return e_success;
}


Status check_capacity(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_failure)
    {
        printf("files are not open\n");
        return e_failure;
    }
    encInfo->image_size = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->sec_file_size = get_file_size(encInfo->fptr_secret);
    encInfo->extn_size = strlen(strstr(encInfo->secret_fname,"."));
    printf("\n");
    int capacity = ((2 + 4 + encInfo->extn_size + 4 + encInfo->sec_file_size)*8) + 54;
    printf("%d %d %d %d\n",encInfo->image_size,capacity,encInfo->extn_size,encInfo->sec_file_size);
    if(encInfo->image_size <= capacity)
    {
        return e_failure;
    }
    return e_success;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    int size = ftell(fptr);
    rewind(fptr);
    return size;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("Before copying %ld %ld\n",ftell(fptr_src_image),ftell(fptr_dest_image));
    char ch[54];
    fread(ch,1,54,fptr_src_image);
   fwrite(ch,1,54,fptr_dest_image);

    printf("after copying %ld %ld\n",ftell(fptr_src_image),ftell(fptr_dest_image));

    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("before insert magic string %ld %ld\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));
    char temp[8];
    for(int i=0;i<2;i++)
    {
        fread(temp,1,8,encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], temp);
        fwrite(temp,1,8,encInfo->fptr_stego_image);
    }
    printf("after inserting %ld %ld\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));
    
    return e_success;
}


Status encode_byte_to_lsb(char data, char *image_buffer)
{
    //image buffer size is 8 bytes

    char n = 7;
    for(int i = 0; i < 8; i++)
    {
        //clear lsb of image_buffer[i]
        image_buffer[i] = image_buffer[i] & 0xfe;

        //get bit from data
        char res = data & (1 << n);

        //take res bit to lsb
        res = res >> n;

        //write res bit to lsb of buffer[i]
        image_buffer[i] = image_buffer[i] | res;

        n--;
    }
    return e_success;
}

Status encode_size_to_lsb(int data, char *image_buffer)
{
    //image buffer size is 32 bytes

    int n = 31;
    for(int i = 0; i < 32; i++)
    {
        //clear lsb of image_buffer[i]
        image_buffer[i] = image_buffer[i] & 0xfffffffe;

        //get bit from data
        int res = data & (1 << n);

        //take res bit to lsb
        res = res >> n;

        //write res bit to lsb of buffer[i]
        image_buffer[i] = image_buffer[i] | res;

        n--;
    }
    return e_success;
}


Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("before inserting secret file extn %ld %ld\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));
    char temp[8];
    for( int i =0; file_extn[i]; i++)
    {
        fread(temp,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], temp);
        fwrite(temp,8,1,encInfo->fptr_stego_image);
    }
    printf("after inserting secret file extn %ld %ld\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));  
}

Status encode_extn_file_size(int extn_size, EncodeInfo *encInfo )
{
    printf("before inserting file extn  %ld %ld\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));   
    char temp[32];
    fread(temp,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(extn_size, temp); 
    fwrite(temp,32,1,encInfo->fptr_stego_image);
    printf("after inserting file extn %ld %ld\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));
    
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("Before inserting secret file extn %ld %ld\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));
    
    char temp[32];
    
    fread(temp,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,temp);
    fwrite(temp,32,1,encInfo->fptr_stego_image);
    
    printf("After inserting secret file extn %ld %ld\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));
}


Status encode_secret_file_data(EncodeInfo *encInfo)
{
    
    printf("before inserting secret data %ld %ld\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));
    
    char ch;
    char temp[8];
    while(fread(&ch, 1, 1, encInfo->fptr_secret) == 1)
    {
        fread(temp, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(ch, temp);
        fwrite(temp, 1, 8, encInfo->fptr_stego_image);
    }
    printf("after inserting secret data %ld %ld\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));
    
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    
    printf("copying remaining image as it as %ld %ld\n",ftell(fptr_src),ftell(fptr_dest));
    
    char ch;
    
    while(feof(fptr_src) == 0)
    {
        fread(&ch,1,1,fptr_src);
        fwrite(&ch,1,1,fptr_dest);
    }
    
    printf("after copying %ld %ld\n",ftell(fptr_src),ftell(fptr_dest)); 
    
    return e_success;
}


