#if !defined(RTS_H)

struct platform_window;
struct platform_sound_device;

typedef struct game_context {
    struct platform_window *Window;
    struct platform_sound_device *Device;
} game_context;

platform_window *PlatformOpenWindow(const char *Title);
void PlatformCloseWindow(platform_window *Window);
platform_sound_device *PlatformOpenSoundDevice();
void PlatformCloseSoundDevice(platform_sound_device *Device);

void GameMain(game_context *Context);
void GameShutdown(game_context *Context);

#define RTS_H
#endif