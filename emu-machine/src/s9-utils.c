#include <stdio.h>
#include <string.h>
#include "s9-utils.h"

#define is_hex(x) ((x >= '0' && x <= '9') || (x >= 'A' && x <= 'F'))

static inline int hex_val(char ch)
{
    if (ch >= '0' && ch <= '9')
    {
        return ch - '0';
    }
    return ch - 'A' + 10;
    //     else if (ch >= 'A' && ch <= 'F')
    //     {
    //         return ch - 'A' + 10;
    //     }
    //     return -1;
}

static inline int get_byte(const char **pp, int *checksum)
{
    int val = hex_val(**pp) << 4;
    (*pp)++;
    val |= hex_val(**pp);
    (*pp)++;
    *checksum += val;
    return val;
}

static int s9_validate(const char *line)
{
    const char *p = line;
    int len = strlen(p);
    if (len & 1)
    {
        return S9_ERROR_WRONG_LEN;
    }

    if (len < 8)
    {
        return S9_ERROR_WRONG_LEN;
    }

    if (*p != 'S')
    {
        return S9_ERROR_S_EXPECTED;
    }

    p++;

    if (*p < '0' || *p > '9' || *p == '4')
    {
        return S9_ERROR_RECORD_TYPE;
    }

    while (*(++p))
    {
        if (!is_hex(*p))
        {
            return S9_ERROR_NOT_HEX;
        }
    }

    return 0;
}

void s9_init(S9Decode *decode, void *memory)
{
    memset(decode, 0, sizeof(*decode));
    decode->buffer = memory;
}

int s9_play(const char *line, S9Decode *decode)
{
    static const int address_len[] = {2, 2, 3, 4, 0, 2, 3, 4, 3, 2};
    int s9_error = s9_validate(line);
    if (s9_error)
    {
        return s9_error;
    }
    const char *p = line + 1;
    int type = *p - '0';

    if (type == 0)
    {
        return 0;
    }

    p++;
    int checksum = 0;
    int count = get_byte(&p, &checksum);

    decode->bytes = count - (address_len[type] + 1);
    decode->address = 0;

    for (int i = address_len[type]; i; i--)
    {
        decode->address = (decode->address << 8) |  get_byte(&p, &checksum);
    }

    const size_t expected_len = 4 + count * 2;

    if (strlen(line) != expected_len)
    {
        return S9_ERROR_SIZE_MISMATCH;
    }

    switch (type)
    {
    case 1:
    case 2:
    case 3:
        for (
            unsigned char *dest = decode->buffer + decode->address;
            dest < decode->buffer + decode->address + decode->bytes;
            dest++)
        {

            *dest =  get_byte(&p, &checksum);
        }
        break;
    case 5:
        decode->rec_count = get_byte(&p, &checksum) << 8;
        decode->rec_count |= get_byte(&p, &checksum);
        break;
    case 6:
        decode->rec_count = get_byte(&p, &checksum) << 16;
        decode->rec_count |= get_byte(&p, &checksum) << 8;
        decode->rec_count |= get_byte(&p, &checksum);
        break;
    case 7:
        decode->start = get_byte(&p, &checksum) << 24;
    /* FALLTHRU */
    case 8:
        decode->start |= get_byte(&p, &checksum) << 16;
    /* FALLTHRU */
    case 9:
        decode->start |= get_byte(&p, &checksum) << 8;
        decode->start |= get_byte(&p, &checksum);
    }

    get_byte(&p, &checksum);

    if ((checksum & 0xFF) != 0xFF)
    {
        return S9_ERROR_CHECKSUM;
    }
    return 0;
}
