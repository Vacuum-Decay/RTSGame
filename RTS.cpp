#if !defined RTS_H
#include "RTS.h"


internal void RenderWeirdGradient(game_offscreen_buffer *Buffer, int XOffset, int YOffset) {
    int Width = Buffer->Width;
    int Height = Buffer->Height;

    int Pitch = Width*Buffer->BytesPerPixel;
    uint8_t *Row = (uint8_t *) Buffer->Memory;
    for(int Y = 0; Y < Buffer->Height; Y++) {
        
        uint32_t *Pixel = (uint32_t *) Row;

        for(int X = 0; X < Buffer->Width; X++) {

            uint8_t Blue  = (X + XOffset);
            uint8_t Green = (Y + YOffset);
            uint8_t Red   = 255;

            *Pixel++ = ((Red << 16) | (Green << 8) | Blue);
        }
        Row += Pitch;
    }
}

internal void GameUpdateAndRender(game_offscreen_buffer *Buffer, int XOffset, int YOffset) {
    RenderWeirdGradient(Buffer, XOffset, YOffset);
}

#define RTS_H
#endif