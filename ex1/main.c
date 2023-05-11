#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
/*
 * Диапазон             Вероятность
 * -------------------- -----------
 * [0; 128)             90%
 * [128; 16384)         5%
 * [16384; 2097152)     4%
 * [2097152; 268435455) 1%
 */
uint32_t generate_number()
{
    const int r = rand();
    const int p = r % 100;
    if (p < 90) {
        return r % 128;
    }
    if (p < 95) {
        return r % 16384;
    }
    if (p < 99) {
        return r % 2097152;
    }
    return r % 268435455;
}

size_t encode_varint(uint32_t value, uint8_t* buf)
{
    assert(buf != NULL);
    uint8_t* cur = buf;
    while (value >= 0x80) {
        const uint8_t byte = (value & 0x7f) | 0x80;
        *cur = byte;
        value >>= 7;
        ++cur;
    }
    *cur = value;
    ++cur;
    return cur - buf;
}

uint32_t decode_varint(const uint8_t** bufp)
{
    const uint8_t* cur = *bufp;
    uint8_t byte = *cur++;
    uint32_t value = byte & 0x7f;
    size_t shift = 7;
    while (byte >= 0x80) {
        byte = *cur++;
        value += (byte & 0x7f) << shift;
        shift += 7;
    }
    *bufp = cur;
    return value;
}


int main()
{
	FILE *uncompWrite = fopen("uncompressed.dat", "wb");
	FILE *compWrite = fopen("compressed.dat", "wb");
	
	for (int i = 0; i < 1000000; i++)
	{
		uint32_t value = generate_number();
		fwrite(&value, sizeof(value), 1, uncompWrite);
		uint8_t buf[4] = {};
		size_t size = encode_varint(value, buf);
		fwrite(buf, sizeof(*buf), size, compWrite);
	}
	fclose(uncompWrite);
	fclose(compWrite);
	
	
    FILE *uncompRead = fopen("uncompressed.dat", "rb");
	FILE *compRead = fopen("compressed.dat", "rb"); 

	fseek(compRead, 0, SEEK_END);
	size_t endfile = ftell(compRead);
	fseek(compRead, 0, SEEK_SET);

    fseek(uncompRead, 0, SEEK_END);
    size_t uncomp_size = ftell(uncompRead);
    fseek(uncompRead, 0, SEEK_SET);
	
	uint8_t compressed[endfile];
	fread(compressed, sizeof(*compressed), endfile, compRead);
    uint32_t uncompressed[1000000];
    fread(uncompressed, sizeof(*uncompressed), 1000000, uncompRead);

	const uint8_t *cur_comp = compressed;
    int i = 0;
	while (cur_comp < compressed + endfile)
	{
		if (uncompressed[i] != decode_varint(&cur_comp))
        {
            printf("Numbers with index %d not equal\n", i);
            i = 0;
            break;
        }
        i++;
	}

    if (i)
    {
        printf("All numbers from two files are equal\n");
    }

	fclose(uncompRead);
	fclose(compRead);

    double k = (double)uncomp_size / endfile;
    printf("Compression ratio = %f\n", k);

	return 0;
}
