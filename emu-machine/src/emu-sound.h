#pragma once
#include <stdint.h>
#include <stddef.h>
#include "emu-device.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef int (*callbackfn)(uint16_t buffer, int size);
    PortDevice *emu_sound_init();
    void emu_sound_start(PortDevice *device);
#ifdef __cplusplus
}
#endif