#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "emu.h"
#include "emu-ports.h"
// static void emu_register_device(Emulator *emulator, PortDevice *device, const int *readPortMap, const int *writePortMap);

Emulator *emu_new()
{
    Emulator *emulator = (Emulator *)malloc(sizeof(Emulator));
    memset(emulator, 0, sizeof(*emulator));
    emulator->context = (struct Context *)malloc(sizeof(struct Context));
    emulator->context->gData = (void *)emulator;
    emulator->context->memory = emulator->memory;
    emulator->context->port_read = (int (*)(void *, int))port_read;
    emulator->context->port_write = (void (*)(void *, int, int))port_write;
    emulator->context->PC = 0;
    emulator->context->halt = 0;
    emulator->context->address_mask = 0xFFFF;
    emulator->context->rom_size = 0x0000;
    emulator->clock_ticks = 0;

    return emulator;
}

void emu_register_device(Emulator *emulator, PortDevice *device, const int *readPortMap, const int *writePortMap)
{
    emulator->devices[emulator->devices_count++] = device;
    for (int n = 0; n < device->readPortCount; n++)
    {
        int emu_port = readPortMap[n];
        if (device->read[n])
        {
            if (emulator->dev_read_handler[emu_port])
            {
                printf("device read collision on port %d\n", emu_port);
                exit(1);
            }
            emulator->dev_read[emu_port] = device;
            emulator->dev_read_handler[emu_port] = device->read[n];
        }
    }

    for (int n = 0; n < device->writePortCount; n++)
    {
        int emu_port = writePortMap[n];
        if (device->write[n])
        {
            if (emulator->dev_write_handler[emu_port])
            {
                printf("device write collision on port %d\n", emu_port);
                exit(1);
            }
            emulator->dev_write[emu_port] = device;
            emulator->dev_write_handler[emu_port] = device->write[n];
        }
    }

    if (device->clock_ticks)
    {
        emulator->dev_ticks[emulator->dev_ticks_count++] = device;
    }
}

void emu_dispose(Emulator *emulator)
{
    while (emulator->devices_count--)
    {
        PortDevice *device = emulator->devices[emulator->devices_count];
        device->dispose(device);
    }
    free(emulator->context);
    free(emulator);
}

static inline bool handleTicks(Emulator *emulator, int cycles)
{
    emulator->clock_ticks += cycles;
    if (emulator->clock_ticks >= CPU_8080_CLOCKS_PER_TICK)
    {
        emulator->clock_ticks -= CPU_8080_CLOCKS_PER_TICK;
        return true;
    }
    return false;
}

static inline bool handleIntr(Emulator *emulator)
{
    if (!emulator->intr)
    {
        return 0;
    }
    int mask = 1;
    int intr = 0;
    while (intr < 8)
    {
        if (emulator->intr & mask)
        {
            emulator->intr = emulator->intr & ~mask;
            return emu_8080_rst(emulator->context, intr);
        }
        mask <<= 1;
        intr++;
    }
    return 0;
}

int emu_execute(Emulator *emulator, int clocks_ticks)
{
    int ticks = 0;
    while (ticks < clocks_ticks && !emulator->context->halt)
    {
        int intrCycle = handleIntr(emulator);
        // printf(
        //     "PC=%04x SP=%04x (SP)=%04x\n",
        //     emulator->context->PC,
        //     emulator->context->SP,
        //     emulator->context->memory[emulator->context->SP] +
        //         (emulator->context->memory[emulator->context->SP + 1] << 8));
        int cycles = intrCycle ? intrCycle : emu_8080_execute(emulator->context);
        if (handleTicks(emulator, cycles))
        {
            for (PortDevice **pd = emulator->dev_ticks; *pd; pd++)
            {
                (*pd)->clock_ticks(*pd);
            }
        }
        ticks += cycles;
    }
    return ticks;
}

int emu_handle_keyboard(Emulator *emulator, int keyVal, int isPressed)
{
    return emulator->key_event_handler ? emulator->key_event_handler(emulator->key_event_device, keyVal, isPressed) : 0;
}