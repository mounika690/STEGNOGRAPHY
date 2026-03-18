#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
    DecInfo decInfo;
    OperationType ret =check_operation_type(argv);
    if( ret == e_encode)
    {
        if(read_and_validate_encode_args(argv,&encInfo)== e_failure)
        {
            return e_failure;
        }
        if(do_encoding(&encInfo) == e_failure)
        {
            return e_failure;
        }
        
        printf("Encoded SUccessfully\n");
    }
    

    else if( ret == e_decode)
    {
        if(read_and_validate_decode_args(argv,&decInfo)== e_failure)
        {
            return e_failure;
        }
        if(do_decoding(&decInfo) == e_failure)
        {
            return e_failure;
        }
        
        printf("Decoded SUccessfully\n");
    }
    
    else
    {
        printf("Unsupported operations given\n");
    }
    
    return 0;

}
