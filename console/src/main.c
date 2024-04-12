#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "emu-bios.h"
#include "emu.h"
#include "s9-utils.h"

void load_program(Emulator *emulator, const char *filename);
void diag_put_char(char ch, void *data)
{
    putchar(ch);
    data = data;
}

int main(int argc, char *argv[])
{
    static const int emu_bios_read_map[] = {0x11};
    static const int emu_bios_write_map[] = {0x10, 0x11};
    if (argc != 2)
    {
        printf("usage diag <bin file>\n");
        exit(1);
    }
    Emulator *emulator = emu_new();
    emulator->context->address_mask = 0xFFFF;
    emulator->context->rom_size = 0;
    PortDevice *bios = emu_bios_init();
    emu_register_device(emulator, bios, emu_bios_read_map, emu_bios_write_map);
    emu_bios_register_char_device(bios, 0, NULL, diag_put_char, NULL);
    emulator->context->PC = 0; // 0x100;
    load_program(emulator, argv[1]);
    while (!emulator->context->halt)
    {
        emu_execute(emulator, 10000);
    }
    emu_execute(emulator, 10000000);
    emu_dispose(emulator);
    printf("\ndone\n");
    return 0;
}
void load_program(Emulator *emulator, const char *filename)
{
    char buffer[540];
    S9Decode decode;
    s9_init(&decode, emulator->memory);
    FILE *f = fopen(filename, "r");
    while (fgets(buffer, sizeof(buffer), f) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = 0;
        int status = s9_play(buffer, &decode);
        printf("%d %02x %04x - %s\n", status, decode.bytes, decode.address, buffer);
    }
    fclose(f);
}