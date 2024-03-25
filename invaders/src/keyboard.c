#include <gtk/gtk.h>
#include "keyboard.h"
#include "emu-keyboard.h"

static struct EmuKeyboardMap
{
    int key;
    int si_key;
} emu_keyboard_map[] = {
    {.key = GDK_KEY_Left, .si_key = KEY_P1_LEFT},
    {.key = GDK_KEY_Z, .si_key = KEY_P1_LEFT},
    {.key = GDK_KEY_z, .si_key = KEY_P1_LEFT},

    {.key = GDK_KEY_X, .si_key = KEY_P1_SHOT},
    {.key = GDK_KEY_x, .si_key = KEY_P1_SHOT},
    {.key = GDK_KEY_space, .si_key = KEY_P1_SHOT},

    {.key = GDK_KEY_Right, .si_key = KEY_P1_RIGHT},
    {.key = GDK_KEY_C, .si_key = KEY_P1_RIGHT},
    {.key = GDK_KEY_c, .si_key = KEY_P1_RIGHT},

    {.key = GDK_KEY_less, .si_key = KEY_P2_LEFT},
    {.key = GDK_KEY_comma, .si_key = KEY_P2_LEFT},

    {.key = GDK_KEY_greater, .si_key = KEY_P2_SHOT},
    {.key = GDK_KEY_period, .si_key = KEY_P2_SHOT},

    {.key = GDK_KEY_question, .si_key = KEY_P2_RIGHT},
    {.key = GDK_KEY_slash, .si_key = KEY_P2_RIGHT},

    {.key = GDK_KEY_1, .si_key = KEY_P1_START},
    {.key = GDK_KEY_2, .si_key = KEY_P2_START},
    {.key = GDK_KEY_I, .si_key = KEY_CREDIT},
    {.key = GDK_KEY_i, .si_key = KEY_CREDIT},
    {.key = 0},
};

int keyboard_translate(int keyVal){
    for (struct EmuKeyboardMap *map = emu_keyboard_map; map->key; map++)
    {
        if (map->key == keyVal)
        {
            return map->si_key;
        }
    }
    return 0;
}