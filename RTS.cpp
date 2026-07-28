#include "RTS.h"

void GameMain(game_context *Context) {
    const char *Title = "RTS Game";
    Context->Window = PlatformOpenWindow(Title);
    Context->Device = PlatformOpenSoundDevice();
}

void GameShutdown(game_context *Context) {
    PlatformCloseWindow(Context->Window);
    PlatformCloseSoundDevice(Context->Device);
}