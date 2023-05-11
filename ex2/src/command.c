#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "coder.h"
#include "command.h"

int encode_file(const char *in_file_name, const char *out_file_name)
{
    FILE *input = fopen(in_file_name, "r");
    FILE *output = fopen(out_file_name, "wb");

    if (!input)
    {
        printf("ERROR: Wrong source to input file\n");
        return -1;
    }
    if (!output)
    {
        fclose(input);
        return -1;
    }

    fseek(input, 0, SEEK_END);
    size_t end_of_input_file_stream = ftell(input);
    fseek(input, 0, SEEK_SET);

    while(ftell(input) < end_of_input_file_stream)
    {
        uint32_t code_point;
        fscanf(input, "%" SCNx32, &code_point);

        CodeUnits code_unit;
        if (encode(code_point, &code_unit))
        {
            printf("ERROR: Failed to encode number\n");
            return -1;
        }
        write_code_unit(output, &code_unit);
    }

    fclose(input);
    fclose(output);
    return 0;
}

int decode_file(const char *in_file_name, const char *out_file_name)
{
    FILE *input = fopen(in_file_name, "rb");
    FILE *output = fopen(out_file_name, "w");

    if (!input)
    {
        printf("ERROR: Wrong source to input file\n");
        return -1;
    }
    if (!output)
    {
        fclose(input);
        return -1;
    }

    fseek(input, 0, SEEK_END);
    size_t end_of_input_file_stream = ftell(input);
    fseek(input, 0, SEEK_SET);
    
    while(ftell(input) < end_of_input_file_stream)
    {
        CodeUnits code_unit;
        if (read_next_code_unit(input, &code_unit))
        {
            printf("ERROR: Failed reading next code unit. End of file reached\n");
        }
        uint32_t code_point = decode(&code_unit);
        fprintf(output, "%" PRIx32 "\n", code_point);
    }

    fclose(input);
    fclose(output);
}