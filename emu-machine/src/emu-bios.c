#include <string.h>
#include "emu-bios.h"

typedef struct
{
    int ch;
    void *data;
    char_put put;
    char_get get;
    bool enabled;
} CharDevice;

typedef struct
{
    int track;
    int sec;
    void *data;
    buffer_read read;
    buffer_write write;
    bool enabled;
} BlockDevice;

typedef struct
{
    int state;
    int device_id;
    int dma_offset;
    CharDevice char_device[4];
    BlockDevice block_device[4];
} BiosDevice;

typedef enum
{
    stSTART,
    stWRITE_CHAR,
    stCHAR_EXISTS,
    stCHAR_VALUE,
} CmdState;

void emu_bios_done(PortDevice *device)
{
    free(device->data);
    free(device);
}

static inline void bios_port_cmd_write(PortDevice *device, int v);
static inline void bios_port_param_write(PortDevice *device, int v);
static inline int bios_port_read(PortDevice *device);

static void (*port_write_array[])(PortDevice *g, int v) = {bios_port_cmd_write, bios_port_param_write};

PortDevice *emu_bios_init()
{
    PortDevice *device = malloc(sizeof(PortDevice));
    memset(device, 0, sizeof(PortDevice));
    BiosDevice *bios = malloc(sizeof(BiosDevice));
    device->data = bios;
    device->dispose = emu_bios_done;
    device->writePortCount = 2;
    device->write = (PortWrite *)port_write_array;
    device->readPortCount = 1;
    device->read = (PortRead *)bios_port_read;

    return device;
}

void emu_bios_register_char_device(PortDevice *device, int n, char_get get, char_put put, void *data)
{
    BiosDevice *bios = device->data;
    bios->char_device[n].data = data;
    bios->char_device[n].get = get;
    bios->char_device[n].put = put;
    bios->char_device[n].enabled = true;
}

void emu_bios_register_block_device(PortDevice *device, int n, buffer_read read, buffer_write write, void *data)
{
    BiosDevice *bios = device->data;
    bios->block_device[n].data = data;
    bios->block_device[n].read = read;
    bios->block_device[n].write = write;
    bios->block_device[n].enabled = true;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wreturn-type"

#pragma GCC diagnostic pop

static inline void bios_port_cmd_write(PortDevice *device, int v)
{
    BiosDevice *bios = device->data;
    switch (v)
    {
    case CMD_CHAR_EXISTS:
        bios->state = stCHAR_EXISTS;
        break;
    case CMD_CHAR_READ:
        bios->state = stCHAR_VALUE;
        break;
    case CMD_CHAR_WRITE:
        bios->state = stWRITE_CHAR;
        break;
    case CMD_BLOCK_READ:
        // bios->state = stCHAR_EXISTS;
        break;
    case CMD_BLOCK_WRITE:
        // bios->state = stCHAR_EXISTS;
        break;
    }
}

static inline void bios_port_param_write(PortDevice *device, int value)
{
    BiosDevice *bios = device->data;
    switch (bios->state)
    {
    case stSTART:
        // no command issued
        break;
    case stWRITE_CHAR:
        bios->char_device[0].put(value, bios->char_device[0].data);
        bios->state = stSTART;
        break;
    case stCHAR_EXISTS:
        // invalid state should be handled in read
        break;
    case stCHAR_VALUE:
        // invalid state should be handled in read
        break;
    default:
        bios->state = stSTART;
        break;
    }
}

static inline int bios_port_read(PortDevice *device)
{
    BiosDevice *bios = device->data;
    int ch;
    switch (bios->state)
    {
    case stCHAR_EXISTS:
        bios->state = stSTART;
        return bios->char_device[0].ch >= 0;
    case stCHAR_VALUE:
        bios->state = stSTART;
        ch = bios->char_device[0].ch;
        bios->char_device[0].ch = -1;
        return ch;
    default:
        return 0;
    }
}
