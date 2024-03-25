#pragma once
#include <stddef.h>
#include "emu.h"
#include "emu-device.h"

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 224
#define SCREEN_BUFFER_LOCATION 0x2400
#define SCREEN_UPDATE_DIVISOR 6

typedef void (*UpdateFn)(u_int32_t *rgba_pixels, int width, int height, void *user_data);
typedef struct
{
    int ticks;
    int *intr;
    u_int32_t *screen_buffer;
    u_int8_t *emu_buffer;
    UpdateFn update;
    void *user_data;

} ScreenDevice;
#ifdef __cplusplus
extern "C"
{
#endif

    PortDevice *emu_screen_init(int *intr, u_int8_t *emu_buffer, u_int32_t *screen_buffer, UpdateFn fn, void *user_data);
#ifdef __cplusplus
}
#endif