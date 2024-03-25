#pragma once
#include <gtk/gtk.h>
#include "emu-screen.h"

#define SCREEN_SCALE 2

typedef struct
{
    GtkImage *screenImage;
    u_int32_t buffer[SCREEN_WIDTH * SCREEN_HEIGHT];
    GdkPixbuf *pixbuf;
    GdkPixbuf *pixbufScale;

} Display;

Display *display_new(GtkImage *screenImage);
u_int32_t *display_screen_buffer(const Display *display);
void display_dispose(Display *display);
void display_update(u_int32_t *pixbuf_pixels, int width, int height, Display *display);
