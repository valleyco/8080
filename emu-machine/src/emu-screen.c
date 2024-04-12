/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
#include <stdio.h>
#include <string.h>
#include "emu.h"
#include "emu-screen.h"

static void update_buffer(const uint8_t *emu_buffer, uint32_t *screen_buffer);

static void screen_tick(PortDevice *device)
{
    ScreenDevice *screenDevice = (ScreenDevice *)device->data;
    screenDevice->ticks = (screenDevice->ticks + 1) % 1000;
    int pos = screenDevice->ticks % 17;
    if (pos == 0)
    {
        *(screenDevice->intr) |= (1 << 1);
    }
    else if (pos == 8)
    {
        *(screenDevice->intr) |= (1 << 2);
    }
    if ((screenDevice->ticks % (SCREEN_UPDATE_DIVISOR * 17)) == 0)
    {
        update_buffer(screenDevice->emu_buffer, screenDevice->screen_buffer);
        screenDevice->update(screenDevice->screen_buffer, SCREEN_WIDTH, SCREEN_HEIGHT, screenDevice->user_data);
    }
}

static void emu_screen_done(PortDevice *device)
{
    free(device->data);
    free(device);
}

PortDevice *emu_screen_init(int *intr, uint8_t *emu_buffer, uint32_t *screen_buffer, UpdateFn fn, void *user_data)
{
    PortDevice *device = malloc(sizeof(PortDevice));
    memset(device, 0, sizeof(PortDevice));
    ScreenDevice *screenDevice = malloc(sizeof(ScreenDevice));
    device->data = screenDevice;
    device->dispose = emu_screen_done;
    device->clock_ticks = screen_tick;
    screenDevice->intr = intr;
    screenDevice->ticks = 0;
    screenDevice->emu_buffer = emu_buffer;
    screenDevice->update = fn;
    screenDevice->user_data = user_data;
    screenDevice->screen_buffer = screen_buffer;
    return device;
}


static void update_buffer(const uint8_t *emu_buffer, uint32_t *screen_buffer)
{
    for (const uint8_t *p_image = emu_buffer; p_image < (emu_buffer + SCREEN_HEIGHT * SCREEN_WIDTH / 8); p_image++)
    {
        for (int b = 1; b & 0xff; b <<= 1)
        {
            *(screen_buffer++) = (*p_image & b) ? 0xFFFFFFFF : 0xFF000000;
        }
    }
}
