#ifndef RTS_H
#include<stdint.h>
#define bool32 int32_t

#define Kilobytes(Value) ((Value) * 1024LL)
#define Megabytes(Value) (Kilobytes(Value) * 1024LL)
#define Gigabytes(Value) (Megabytes(Value) * 1024LL)
#define Terrabytes(Value) (Gigabytes(Value) * 1024LL)

#ifdef RTS_SLOW
#define Assert(expression) if(!(expression)) {*(int*)0 = 0;}
#else
#define Assert(expression)
#endif

inline uint32_t SafeTruncateUInt64(uint64_t Value) {
    Assert(Value <= 0xFFFFFFFF);
    uint32_t Result = (uint32_t)Value;
    return Result;
}

#if RTS_INTERNAL
DEBUGPlatformReadEntireFile(char *Filename)
DEBUGPlatformFreeFileMemory(void *Memory);
internal bool32 DEBUGPlatformWriteEntireFile(char *FileName, uint32_t MemorySize, void *Memory);
#endif

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

struct game_offscreen_buffer
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
};

struct game_sound_output_buffer {
    int SamplesPerSecond;
    int SampleCount;
    int16_t *Samples;
};

struct game_button_state{
    int HalfTransitionCount;
    bool32 EndedDown;
};

struct game_controller_input {
    bool32 IsAnalog;

    float StartX;
    float StartY;
    float MinX;
    float MaxX;

    float EndX;
    float EndY;
    float MinY;
    float MaxY;

    union {
        game_button_state Buttons[6];
        struct{
            game_button_state Up;
            game_button_state Down;
            game_button_state Left;
            game_button_state Right;
            game_button_state LeftShoulder;
            game_button_state RightShoulder;
        };
    };
};

struct game_input {
    game_controller_input Controllers[4];
};

struct game_memory {
    bool32 IsInitialized;
    

    uint64_t PermanentStorageSize;
    void     *PermanentStorage;

    uint64_t TransientStorageSize;
    void     *TransientStorage;
};

struct game_state {
    int XOffset;
    int YOffset;
    int ToneHz;
};

internal void GameUpdateAndRender(game_input *Input, game_offscreen_buffer *Buffer, game_sound_output_buffer *SoundBuffer);

#define RTS_H
#endif