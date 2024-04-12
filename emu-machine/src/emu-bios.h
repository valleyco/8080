#pragma once
#include "emu-ports.h"

#define CMD_CHAR_EXISTS 1
#define CMD_CHAR_READ 2
#define CMD_CHAR_WRITE 3

#define CMD_BLOCK_READ 4
#define CMD_BLOCK_WRITE 5

typedef void (*char_put)(char ch, void *data);
typedef int (*char_get)(void *data);
typedef void (*buffer_read)(int f_offset, int m_offset, void *data);
typedef void (*buffer_write)(int f_offset, int m_offset, void *data);

#ifdef __cplusplus
extern "C"
{
#endif
    PortDevice *emu_bios_init();
    void emu_bios_register_char_device(PortDevice* device, int n, char_get get, char_put put, void* data);
    void emu_bios_register_block_device(PortDevice* device, int n, buffer_read read, buffer_write write, void* data);
#ifdef __cplusplus
}
#endif