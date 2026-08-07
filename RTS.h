#if !defined(RTS_H)
#include<stdint.h>

struct game_offscreen_buffer
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
};

struct game_sound_output_buffer {
    int SamplesPerSecond;
    int SampleCount;
    int16_t *Samples;
};

internal void GameUpdateAndRender(game_offscreen_buffer *Buffer, int XOffset, int YOffset, game_sound_output_buffer *SoundBuffer, int ToneHz);

#define RTS_H
#endif