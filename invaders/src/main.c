/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/// #include <config.h>
#include <stdio.h>
#include <time.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <gtk/gtktypes.h>
#include <limits.h>
#include "emu.h"
#include "emu-shifter.h"
#include "emu-screen.h"
#include "emu-keyboard.h"
#include "emu-ports.h"
#include "emu-screen.h"
#include "invaders.rom.h"
#include "invaders_glade.h"
#include "display.h"
#include "keyboard.h"

static GtkImage *screenImage;
static gint64 emu_cycle_last_run;
static Emulator *emu;
static Display *display;
static guint timeout_id;

static gboolean emu_key_function(GtkWidget *gtkWidget, GdkEventKey *event, Emulator *emu )
{
    (void)gtkWidget;
    int key = keyboard_translate(event->keyval);
    return key ? emu_handle_keyboard(emu, key, event->type == GDK_KEY_PRESS) : 0;
}

/*
    Will be called periodically to run another time slice of the emulator and
    to update the screen from the emulator screen buffer memory
*/
static gboolean emulation_update(Emulator *em)
{
    gint64 now = g_get_monotonic_time() / 1000;
    gint64 diff = now - emu_cycle_last_run; // find how much time we need to cover in miliseconds
    int cycles = (CPU_8080_HZ / 1000) * diff;
    emu_execute(em, cycles);
    emu_cycle_last_run = now;
    return TRUE;
}

static Emulator *init_emulator(const Display *display)
{
    static const int emu_keyboard_read_map[] = {0, 1, 2};
    static const int emu_shifter_read_map[] = {3};
    static const int emu_shifter_write_map[] = {2, 4};
    static const int emu_sound_write_map[] = {3, 5};
    Emulator *emulator = emu_new();
    emulator->context->address_mask = 0x3FFF;
    emulator->context->rom_size = 0x2000;

    emulator->key_event_device = emu_keyboard_init(&emulator->key_event_handler);
    emu_register_device(emulator, emulator->key_event_device, emu_keyboard_read_map, NULL);
    emu_register_device(emulator, emu_screen_init(&emulator->intr, emulator->memory + SCREEN_BUFFER_LOCATION, display_screen_buffer(display), (UpdateFn)display_update, (void *)display), NULL, NULL);
    emu_register_device(emulator, emu_shifter_init(), emu_shifter_read_map, emu_shifter_write_map);
    emu_register_device(emulator, emu_sound_init(), NULL, emu_sound_write_map);
    memcpy(emulator->memory, invaders_rom, invaders_rom_len);
    return emulator;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void on_window_show(GtkWidget *window, GtkBuilder *builder){
    screenImage = GTK_IMAGE(gtk_builder_get_object(builder, "ID_SCREEN_IMAGE"));
    display = display_new(screenImage);
    emu = init_emulator(display);
    emu_cycle_last_run = g_get_monotonic_time() / 1000;
    timeout_id = g_timeout_add(100, (GSourceFunc)emulation_update, emu);
    g_signal_connect(G_OBJECT(window), "key_press_event", G_CALLBACK(emu_key_function), emu);
    g_signal_connect(G_OBJECT(window), "key_release_event", G_CALLBACK(emu_key_function), emu);

}

static void on_window_destroy(GtkWidget *window,void* data){
    g_source_remove(timeout_id);
    emu_dispose(emu);
    display_dispose(display);
}

static void on_app_activate(GtkApplication *app, GtkBuilder *builder)
{
    gtk_builder_add_from_string(builder, (const char*)invaders_glade,-1, NULL);
    gtk_builder_connect_signals(builder, NULL);

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "gtk_main_window"));
    gtk_widget_add_events(window, GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);
    g_signal_connect(G_OBJECT(window), "show", G_CALLBACK(on_window_show), builder);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(on_window_destroy), emu);

    gtk_application_add_window(app, (GtkWindow *)(window));

    gtk_widget_show_all(GTK_WIDGET(window));
}
#pragma GCC diagnostic pop

int main(int argc, char *argv[])
{
#if GLIB_CHECK_VERSION(2, 74, 0)
    GtkApplication *app = gtk_application_new("valleyco.emu.i8080", G_APPLICATION_DEFAULT_FLAGS);
#else
    GtkApplication *app = gtk_application_new("valleyco.emu.i8080", G_APPLICATION_FLAGS_NONE);
#endif
    GtkBuilder *builder = gtk_builder_new();
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), builder);

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(builder);
    g_object_unref(app);
    return status;
}
