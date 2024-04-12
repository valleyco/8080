#pragma once
#include <stdbool.h>

#define S9_ERROR_S_EXPECTED 1
#define S9_ERROR_RECORD_TYPE 1
#define S9_ERROR_NOT_HEX 2
#define S9_ERROR_SIZE_MISMATCH 3
#define S9_ERROR_WRONG_LEN 4
#define S9_ERROR_CHECKSUM 5

typedef struct
{
    int address;
    int rec_count;
    int bytes;
    int start;
    unsigned char *buffer;

} S9Decode;
#ifdef __cplusplus
extern "C"
{
#endif
    void s9_init(S9Decode *decode, void *memory);
    int s9_play(const char *line, S9Decode *decode);
#ifdef __cplusplus
}
#endif