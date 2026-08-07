#if !defined(win32_RTS_h)

struct win32_offscreen_buffer
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
};

struct win32_window_dimension
{
    int Width;
    int Height;
};

struct win32_sound_output {
    int SamplesPerSecond;
    uint32_t RunningSampleIndex;
    int BytesPerSample;
    int SecondaryBufferSize;
    int LatencySampleCount;
};

#define win32_RTS_H
#endif