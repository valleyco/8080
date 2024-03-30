#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "emu-sound.h"
#include "sound_data.h"
#include "sound_data_len.h"
#include "misc.h"

/* This value is based on 11 khz wave file*/
#define SAMPLES_PER_MICRO 11

typedef enum
{
    SHOOT,
    EXPLOSION,
    INVADER_KILLED,
    FAST_INVADER_1,
    FAST_INVADER_2,
    FAST_INVADER_3,
    FAST_INVADER_4,
    UFO_HIGH_PITCH,
    UFO_LOW_PITCH,
    SOUND_MAX,
} Sounds;

typedef enum
{
    BIT_UFO_LOW_PITCH = 1,
    BIT_SHOOT = 2,
    BIT_INVADER_KILLED = 8,
    BIT_UFO_HIGH_PITCH,
} SoundBits_0;

typedef enum
{
    BIT_FAST_INVADER_1 = 1,
    BIT_FAST_INVADER_2 = 2,
    BIT_FAST_INVADER_3 = 4,
    BIT_FAST_INVADER_4 = 8,
    BIT_EXPLOSION = 16
} SoundBits_2;

static int map[][8] = {
    {UFO_LOW_PITCH, SHOOT, 0, INVADER_KILLED, 0, 0, 0, 0},
    {FAST_INVADER_1, FAST_INVADER_2, FAST_INVADER_3, FAST_INVADER_4, EXPLOSION, 0, 0, 0},
};

#define OUTPUT_BUFFER_SIZE (1 << 15)
#define OUTPUT_BUFFER_MASK (OUTPUT_BUFFER_SIZE - 1)

typedef struct
{
    int ports[2];
    bool status[SOUND_MAX];
    uint16_t buffer[OUTPUT_BUFFER_SIZE]; // ~3 sec
    int64_t frame_start_time;
    int buffer_start_pos;
    int ticks;

} SoundDevice;

static uint8_t *audio8_data[] = {
    shoot_wav,
    explosion_wav,
    invaderkilled_wav,
    fastinvader1_wav,
    fastinvader2_wav,
    fastinvader3_wav,
    fastinvader4_wav,
    ufo_highpitch_wav,
    ufo_lowpitch_wav,
};

static int audio8_data_len[] = {
    SHOOT_WAV_LEN,
    EXPLOSION_WAV_LEN,
    INVADERKILLED_WAV_LEN,
    FASTINVADER1_WAV_LEN,
    FASTINVADER2_WAV_LEN,
    FASTINVADER3_WAV_LEN,
    FASTINVADER4_WAV_LEN,
    UFO_HIGHPITCH_WAV_LEN,
    UFO_LOWPITCH_WAV_LEN,
};

#define SOUND_COUNT ((int)sizeof(audio8_data) / (int)sizeof(audio8_data[0]))

static uint16_t *audio16_buffer[SOUND_COUNT];

typedef struct
{
    int wave_id;
    int audio_pos;   // position to the audio buffer to be played
    bool is_playing; // position to the audio buffer to be played

} CallbackData;

static CallbackData callback_data[SOUND_COUNT];

static void init_sound_data()
{

    for (int i = 0; i < SOUND_COUNT; i++)
    {
        audio16_buffer[i] = malloc(sizeof(audio16_buffer[0]) * audio8_data_len[i]);
        for (int s = 0; s < audio8_data_len[i]; s++)
        {
            audio16_buffer[i][s] = (audio8_data[i][s]) * 64;
        }
        callback_data[i].audio_pos = audio8_data_len[i];
    }
}
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"

static int send_sound_data(SoundDevice *sd, int length){
    const int64_t now = get_hr_time();
    const int64_t diff = now - sd->frame_start_time;
    int samples_count = SAMPLES_PER_MICRO * diff;
    return samples_count + length;
}

#pragma GCC diagnostic pop


static void dump_sound(SoundDevice *sd, int samples_pos, int sound)
{
    const int sound_len = audio8_data_len[sound];
    const int linear_end_pos = sd->buffer_start_pos + samples_pos + sound_len;
    const uint16_t *source = audio16_buffer[sound];
    const int to_write = linear_end_pos > OUTPUT_BUFFER_SIZE ? OUTPUT_BUFFER_SIZE - sd->buffer_start_pos : sound_len;
    uint16_t *pos = sd->buffer + sd->buffer_start_pos;

    while (pos < sd->buffer + sd->buffer_start_pos + to_write)
    {
        *pos = 0;
        *(pos++) += *(source++);
    }

    const int left_to_write = sound_len - to_write;
    for (pos = sd->buffer; pos < sd->buffer + left_to_write; pos++)
    {
        *pos = 0;
        *pos += *(source++);
    }
}

static void port_write(PortDevice *device, int port, int v)
{
    SoundDevice *sd = ((SoundDevice *)device->data);
    sd->ports[port] = v;
    const int64_t now = get_hr_time();
    const int64_t diff = now - sd->frame_start_time;
    int samples_pos = SAMPLES_PER_MICRO * diff;
    for (int i = 0; i < 8; i++)
    {
        const int sound = map[port][i];
        const bool status = ((1 << i) & v) != 0;
        if (status != sd->status[sound] && status)
        {
            dump_sound(sd, samples_pos, sound);
        }
        sd->status[sound] = status;
    }
}

static inline void port_write_0(PortDevice *device, int v)
{
    port_write(device, 0, v);
}

static inline void port_write_1(PortDevice *device, int v)
{
    port_write(device, 1, v);
}

static void sound_tick(PortDevice *device)
{
    bool *status = ((SoundDevice *)device->data)->status;
    ((SoundDevice *)device->data)->ticks++;
    for (int i = 0; i < SOUND_MAX; i++)
    {
        status[i] = 0;
    }
}

static void emu_sound_done(PortDevice *device)
{
    free(device->data);
    free(device);
}

PortDevice *emu_sound_init()
{
    static void (*port_write_array[])(PortDevice *g, int v) = {port_write_0, port_write_1};
    PortDevice *device = (PortDevice *)malloc(sizeof(PortDevice));
    memset(device, 0, sizeof(PortDevice));
    device->dispose = emu_sound_done;
    device->readPortCount = 0;
    device->writePortCount = 2;
    device->write = (PortWrite *)port_write_array;
    device->clock_ticks = sound_tick;
    device->data = malloc(sizeof(SoundDevice));
    memset(device->data, 0, sizeof(SoundDevice));
    init_sound_data();
    return device;
}

void emu_sound_start(PortDevice *device)
{
    SoundDevice *sd = ((SoundDevice *)device->data);
    sd->frame_start_time = get_hr_time();
}
