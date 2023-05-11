#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "coder.h"
#include "command.h"

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("EROR: Wrong number of arguments\n");
        return 1;
    }

    const char *command = argv[1];
    const char *in_file_name = argv[2];
    const char *out_file_name = argv[3];

    if (strcmp(command, "encode") == 0)
    {
        encode_file(in_file_name, out_file_name);
    }
    else if (strcmp(command, "decode") == 0)
    {
        decode_file(in_file_name, out_file_name);
    }
    else
    {
        printf("EROR: Wrong command\n");
        return 1;
    }
    return 0;
}