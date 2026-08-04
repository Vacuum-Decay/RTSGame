#if !defined(RTS_H)

struct game_offscreen_buffer
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
};
internal void GameUpdateAndRender(game_offscreen_buffer *Buffer, int XOffset, int YOffset);

#define RTS_H
#endif