#if !defined(RTS_H)

#include "RTS.h"

struct platform_window;
platform_window *PlatformOpenWindow(char *Title);
void PlatformCloseWindow(platform_window *Window);

#define RTS_H
#endif
