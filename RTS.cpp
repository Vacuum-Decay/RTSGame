#if !defined RTS_H
#include<math.h>
#include "RTS.h"

internal void GameOutputSound(game_sound_output_buffer *SoundBuffer, int ToneHz) {
    local_persist float tSine;
    int16_t toneVolume = 3000;
    int WavePeriod = SoundBuffer->SamplesPerSecond/ToneHz;

    int16_t *SampleOut = SoundBuffer->Samples;
    for(int SampleIndex = 0;
        SampleIndex < SoundBuffer->SampleCount;
        ++SampleIndex) {
            float SineValue = sinf(tSine);
            int16_t SampleValue = (int16_t) (SineValue * toneVolume);
            *SampleOut++ = SampleValue;
            *SampleOut++ = SampleValue;

            tSine += 2.0f *3.14159265359f / (float) WavePeriod;
    }
}

internal void RenderWeirdGradient(game_offscreen_buffer *Buffer, int XOffset, int YOffset) {
    int Width = Buffer->Width;
    int Height = Buffer->Height;

    int Pitch = Width*4;
    uint8_t *Row = (uint8_t *) Buffer->Memory;
    for(int Y = 0; Y < Buffer->Height; Y++) {
        
        uint32_t *Pixel = (uint32_t *) Row;

        for(int X = 0; X < Buffer->Width; X++) {

            uint8_t Blue  = (X + XOffset);
            uint8_t Green = (Y + YOffset);
            uint8_t Red   = 127;

            *Pixel++ = ((Red << 16) | (Green << 8) | Blue);
        }
        Row += Pitch;
    }
}

internal void GameUpdateAndRender(game_offscreen_buffer *Buffer, int XOffset, int YOffset, game_sound_output_buffer *SoundBuffer);

internal void GameUpdateAndRender(game_input *Input, game_offscreen_buffer *Buffer, game_sound_output_buffer *SoundBuffer) {
    local_persist int XOffset = 0;
    local_persist int YOffset = 0;
    local_persist int ToneHz = 256;

    game_controller_input *Input0 = &Input->Controllers[0];
    if(Input0->IsAnalog) {
        ToneHz = 256 + (int)(128.0f*(Input0->EndX));
        XOffset += (int)4.0f * (Input0->EndY);
    } else {

    }

    if(Input0->Down.EndedDown) {
        YOffset += 1;
    }

    GameOutputSound(SoundBuffer, ToneHz);
    RenderWeirdGradient(Buffer, XOffset, YOffset);
}

#define RTS_H
#endif