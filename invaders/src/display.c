#include "display.h"
#define SCREEN_SCALE 2
#define HAS_ALPHA 1
#define BPS 8

Display *display_new(GtkImage *screenImage)
{
    Display *display = malloc(sizeof(Display));

    display->screenImage = screenImage;
    display->pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, HAS_ALPHA, BPS, SCREEN_HEIGHT, SCREEN_WIDTH); // we rotate the image so we swap the screen dimensions
    display->pixbufScale = gdk_pixbuf_new(GDK_COLORSPACE_RGB, HAS_ALPHA, BPS, SCREEN_HEIGHT * SCREEN_SCALE, SCREEN_WIDTH * SCREEN_SCALE);

    return display;
}

u_int32_t *display_screen_buffer(const Display *display)
{
    return (u_int32_t *)display->buffer;
}

void display_dispose(Display *display)
{
    g_object_unref(display->pixbuf);
    g_object_unref(display->pixbufScale);
    free(display);
}

static void rotate(const u_int32_t *source, u_int32_t *dest);
static void color_rect(u_int32_t *pixbuf_pixels, int x, int y, int w, int h, u_int32_t color);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void display_update(u_int32_t *pixbuf_pixels, int width, int height, Display *display)
{
    u_int32_t *pixels = (u_int32_t *)gdk_pixbuf_get_pixels(display->pixbuf);

    rotate(display->buffer, pixels);

    color_rect(pixels, 0, 32, 224, 32, 0xFF0000FF);   // RED
    color_rect(pixels, 0, 184, 224, 56, 0xFF00FF00);  // GREEN
    color_rect(pixels, 16, 240, 118, 16, 0xFF00FF00); // GREEN

    gdk_pixbuf_scale(display->pixbuf, display->pixbufScale, 0, 0, SCREEN_HEIGHT * SCREEN_SCALE, SCREEN_WIDTH * SCREEN_SCALE, 0, 0, SCREEN_SCALE, SCREEN_SCALE, GDK_INTERP_BILINEAR);

    gtk_image_set_from_pixbuf(display->screenImage, display->pixbufScale);
}

#pragma GCC diagnostic pop

static void color_rect(u_int32_t *pixbuf_pixels, int x, int y, int w, int h, u_int32_t color)
{
    u_int32_t *start = pixbuf_pixels + y * SCREEN_HEIGHT + x;
    u_int32_t *end = pixbuf_pixels + (y + h - 1) * SCREEN_HEIGHT + x + w;
    while (start < end)
    {
        for (u_int32_t *pos = start; pos < (start + w); pos++)
        {
           *pos &= color;
        }
        start += SCREEN_HEIGHT;
    };
}

static void rotate(const u_int32_t *source, u_int32_t *dest)
{
    const u_int32_t *sl = source;
    u_int32_t *d = dest;
    while (d < (dest + SCREEN_WIDTH * SCREEN_HEIGHT))
    {
        for (const u_int32_t *el = sl + SCREEN_WIDTH - 1; el < source + SCREEN_WIDTH * SCREEN_HEIGHT; el += SCREEN_WIDTH)
        {
            *(d++) = *el;
        }
        sl--;
    }
}
