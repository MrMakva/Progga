#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include "coder.h"
#include "command.h"

int encode(uint32_t code_point, CodeUnits *code_units)
{
    if (code_point < 0x80)
    {
        code_units->length = 1;
        code_units->code[0] = code_point;
    }
    else if (code_point < 0x800)
    {
        code_units->length = 2;
        code_units->code[0] = 0xc0 | (code_point >> 6);
        code_units->code[1] = 0x80 | (code_point & 0x3f);
    }
    else if (code_point < 0x10000)
    {
        code_units->length = 3;
        code_units->code[0] = 0xe0 | (code_point >> 12);
        code_units->code[1] = 0x80 | ((code_point >> 6) & 0x3f);
        code_units->code[2] = 0x80 | (code_point & 0x3f);
    }
    else if (code_point < 0x200000)
    {
        code_units->length = 4;
        code_units->code[0] = 0xf0 | (code_point >> 18);
        code_units->code[1] = 0x80 | ((code_point >> 12) & 0x3f);
        code_units->code[2] = 0x80 | ((code_point >> 6) & 0x3f);
        code_units->code[3] = 0x80 | (code_point & 0x3f);
    }
    else
    {
        return -1;
    }

    return 0;
}

uint32_t decode(const CodeUnits *code_unit)
{
    uint32_t *code_point;
    uint8_t buf[4];
    if (code_unit->length == 1)
    {
        return (uint32_t)code_unit->code[0];
    }
    else if (code_unit->length == 2)
    {
        buf[0] = (code_unit->code[0] << 6) | ((code_unit->code[1] << 2) >> 2);
        buf[1] = ((code_unit->code[0] & 0x1f) >> 2);
        code_point = (uint32_t *)buf;
        return *code_point;
    }
    else if (code_unit->length == 3)
    {
        buf[0] = (code_unit->code[2] & 0x3f) | (code_unit->code[1] << 6);
        buf[1] = ((code_unit->code[1] >> 2) & 0x0f) | (code_unit->code[0] << 4);
        code_point = (uint32_t *)buf;
        return *code_point;
    }
    else if (code_unit->length == 4)
    {
        buf[0] = (code_unit->code[3] & 0x3f) | (code_unit->code[2] << 6);
        buf[1] = ((code_unit->code[2] >> 2) & 0x0f) | (code_unit->code[1] << 4);
        buf[2] = ((code_unit->code[1] >> 4) & 0x03) | ((code_unit->code[0] & 0x07) << 2);
        code_point = (uint32_t *)buf;
        return *code_point;
    }
}

int read_next_code_unit(FILE *in, CodeUnits *code_units)
{
    code_units->length = 0;
    while (code_units->length == 0 && !feof(in))
    {
        uint8_t *buf = code_units->code;
        fread(buf, sizeof(uint8_t), 1, in);
        if ((*buf & 0x80) == 0x00)
        {
            code_units->length = 1;
        }
        else if ((*buf >> 5) == 0x06)
        {
            code_units->length = 2;
            buf++;
            fread(buf, sizeof(uint8_t), 1, in);
        }
        else if ((*buf >> 4) == 0x0e)
        {
            code_units->length = 3;
            buf++;
            fread(buf, sizeof(uint8_t), 2, in);
        }
        else if ((*buf >> 3) == 0x1e)
        {
            code_units->length = 4;
            buf++;
            fread(buf, sizeof(uint8_t), 3, in);
        }
    }
    return 0;
}

int write_code_unit(FILE *out, const CodeUnits *code_unit)
{
    return fwrite(code_unit->code, 1, code_unit->length, out);
}