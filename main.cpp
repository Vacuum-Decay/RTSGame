#include<windows.h>
#include<stdint.h>
#include<xinput.h>
#include<dsound.h>

#define local_persist static
#define global_variable static
#define internal static

typedef int32_t bool32;

struct win32_offscreen_buffer
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
};

struct win32_window_dimension
{
    int Width;
    int Height;
};

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub) {
    return ERROR_DEVICE_NOT_CONNECTED;
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub) {
    return ERROR_DEVICE_NOT_CONNECTED;
}    
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);
DIRECT_SOUND_CREATE(DirectSoundCreateStub) {
    return ERROR_DEVICE_NOT_CONNECTED;
}
global_variable direct_sound_create *DirectSoundCreate_ = DirectSoundCreateStub;
#define DirectSoundCreate DirectSoundCreate_

internal void
Win32LoadXInput(void) {
    HMODULE XInputLibrary = LoadLibraryA("xinput1_3.dll");
    if(XInputLibrary) {
        //TODO: Review how the function pointers are done and stuff. the macros and typedefs are still a bit confusing to me.   
        XInputGetState = (x_input_get_state *) GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState = (x_input_set_state *) GetProcAddress(XInputLibrary, "XInputSetState");
    }
}

global_variable bool GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackbuffer;
global_variable LPDIRECTSOUNDBUFFER GlobalSecondaryBuffer;

internal void Win32InitDirectSound(HWND Window, int32_t SamplesPerSecond, int32_t BufferSize) {
    HMODULE DirectSoundLibrary = LoadLibraryA("dsound.dll");

    if(DirectSoundLibrary) {
        direct_sound_create *DirectSoundCreate = (direct_sound_create *) GetProcAddress(DirectSoundLibrary, "DirectSound");
    }
    LPDIRECTSOUND DirectSound;
    if(DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0))) {
        WAVEFORMATEX WaveFormat = {};
        WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
        WaveFormat.nChannels = 2;
        WaveFormat.nSamplesPerSec = SamplesPerSecond;
        WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
        WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
        WaveFormat.wBitsPerSample = 16;
        WaveFormat.cbSize;

        if(SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY))) {
            DSBUFFERDESC BufferDescription = {sizeof(BufferDescription)};
            BufferDescription.dwSize = sizeof(BufferDescription);
            BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
            BufferDescription.dwBufferBytes = DSBCAPS_PRIMARYBUFFER;
            LPDIRECTSOUNDBUFFER PrimaryBuffer;

            if(SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0))) {

                if(SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat))) {

                } else {

                }
            } else {

            }

        } else {

        }

        DSBUFFERDESC BufferDescription = {};
        BufferDescription.dwSize = sizeof(BufferDescription);
        BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
        BufferDescription.dwBufferBytes = BufferSize;
        BufferDescription.lpwfxFormat = &WaveFormat;
        if(SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &GlobalSecondaryBuffer, 0))) {

        } else {

        }
    } else {
        OutputDebugString("Direct sound failed to create");
    }
}

internal win32_window_dimension
Win32GetWindowDimension(HWND Window) {
    win32_window_dimension Result;

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;

    return Result;
}

internal void RenderWeirdGradient(win32_offscreen_buffer *Buffer, int XOffset, int YOffset) {
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

internal void Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height) {

    if(Buffer->Memory) {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->Width = Width;
    Buffer->Height = Height;

    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    // The line below was an error. Initialized to a local variable instead of the global one.
    // int BytesPerPixel = 4;
    Buffer->BytesPerPixel = 4;
    int BitmapMemorySize = (Buffer->Width * Buffer->Height)*Buffer->BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
 
    int Pitch = Width*Buffer->BytesPerPixel;

    RenderWeirdGradient(Buffer, 128, 0);
}

internal void Win32UpdateWindow(HDC DeviceContext, RECT *ClientRect, win32_offscreen_buffer *Buffer, int X, int Y, int Width, int Height) {
    int WindowWidth = ClientRect->right - ClientRect->left;
    int WindowHeight = ClientRect->bottom - ClientRect->top;    
    StretchDIBits(DeviceContext,
        0, 0, Buffer->Width, Buffer->Height,
        0, 0, WindowWidth, WindowHeight,
        Buffer->Memory,
        &(Buffer->Info),
        DIB_RGB_COLORS, SRCCOPY);
}

internal LRESULT CALLBACK
MainWindowCallback(HWND Window,
                   UINT Message,
                   WPARAM WParam,
                   LPARAM LParam)
{
    LRESULT Result = 0;

    switch(Message) {
        case WM_SIZE: 
        {
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            int Width = ClientRect.right - ClientRect.left;
            int Height = ClientRect.bottom - ClientRect.top;
            Win32ResizeDIBSection(&GlobalBackbuffer, Width, Height);
        } break;
        case WM_CLOSE:
        {
            GlobalRunning = false;
        } break;
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WN__ACTIVATEAPP\n");
        } break;
        case WM_DESTROY:
        {
            GlobalRunning = false;
        } break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32_t VKCode = WParam;
            bool WasDown = ((LParam & (1 << 30)) != 0);
            bool IsDown = ((LParam & (1<< 31)) == 0);
            if(IsDown != WasDown) {
                switch(VKCode) {
                    case 'W': {
                        
                    } break;
                    case 'A': {
                        OutputDebugStringA("You are pressing the A key");
                    } break;
                    case 'S': {
                        OutputDebugStringA("You are pressing the S key");
                    } break;
                    case 'D': {
                        OutputDebugStringA("D key");
                        if(IsDown) {
                            OutputDebugStringA("IsDown");
                        }
                        if(WasDown) {
                            OutputDebugStringA("WasDown");
                        }
                    } break;
                    case 'Q': {

                    } break;
                    case 'E': {

                    } break;
                    case VK_UP: {

                    } break;
                    case VK_LEFT: {

                    } break;
                    case VK_DOWN: {

                    } break;
                    case VK_RIGHT: {

                    } break;
                    case VK_ESCAPE: {

                    } break;
                    case VK_SPACE: {

                    } break;
                    
                    // case VK_LSHIFT:
                    case VK_SHIFT: {
                        OutputDebugStringA("Adding to control group");
                    } break;

                    // case VK_LCONTROL:
                    case VK_CONTROL: {
                        OutputDebugStringA("Creating a new control group");
                    } break;

                    // case VK_LMENU:
                    case VK_MENU: {
                        OutputDebugStringA("Removing from all other control groups");
                    } break;

                    case VK_TAB: {
                        OutputDebugStringA("Cycling through control group");
                    }
                    default: {

                    }break;
                }
                bool32 AltKeyWasDown = ((LParam & (1 << 29)) != 0);
                if((VKCode == VK_F4) && AltKeyWasDown) {
                    GlobalRunning = false;
                }
            }            
        } break;
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;

            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            
            Win32UpdateWindow(DeviceContext, &ClientRect, &GlobalBackbuffer, X, Y, Width, Height);
        } break;
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }

    return(Result);

    
}

int CALLBACK 
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CmdLine,
        int ShowCode
) {
    Win32LoadXInput();

    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = MainWindowCallback;
    WindowClass.hInstance = Instance;
    WindowClass.hIcon;
    WindowClass.hCursor = NULL;
    WindowClass.lpszClassName = "RTSWindowClass";

    if(RegisterClass(&WindowClass)) {
        HWND Window = 
            CreateWindowEx(
                0,
                WindowClass.lpszClassName,
                "RTS Game",
                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                0,
                0,
                Instance,
                0);
        if(Window != NULL) {
            HDC DeviceContext = GetDC(Window);

            MSG Message;
            GlobalRunning = true;
            int XOffset = 0, YOffset = 0;
            
            int SamplesPerSecond = 48000;
            int Hz = 256;
            uint32_t RunningSampleIndex = 0;
            int SquareWaveCounter = 0;
            int SquareWavePeriod = SamplesPerSecond/Hz;
            int BytesPerSample = sizeof(int16_t) * 2;
            Win32InitDirectSound(Window, 48000, 48000*sizeof(int16_t) * 2);
            while(GlobalRunning) {
                if( PeekMessageA(&Message, 0, 0, 0, PM_REMOVE) ) {
                    if(Message.message == WM_QUIT) GlobalRunning = false;

                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }

                for(DWORD ControllerIndex = 0;
                    ControllerIndex < XUSER_MAX_COUNT;
                    ++ControllerIndex) 
                {
                    XINPUT_STATE ControllerState;
                    if(XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS) {
                        XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;

                        bool Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                        bool Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                        bool Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                        bool Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT); 
                        bool Start = (Pad->wButtons & XINPUT_GAMEPAD_START);
                        bool Back = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
                        bool LeftShoulder = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                        bool RightShoulder = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                        bool AButton = (Pad->wButtons & XINPUT_GAMEPAD_A);
                        bool BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
                        bool XButton = (Pad->wButtons & XINPUT_GAMEPAD_X);
                        bool YButton = (Pad->wButtons & XINPUT_GAMEPAD_Y);
                        
                        int16_t StickX = Pad->sThumbLX;
                        int16_t StickY = Pad->sThumbLY;

                        XOffset++;
                        YOffset++;
                    } else {

                    }
                }

                RenderWeirdGradient(&GlobalBackbuffer, XOffset, YOffset);

                DWORD PlayCursor;
                DWORD WriteCursor;
                if(SUCCEEDED(GetCurrentPosition(&PlayCursor, &WriteCursor))) {
                    DWORD WritePointer = ;
                    DWORD BytesToWrite = ;

                    VOID *Region1;
                    DWORD Region1Size;
                    VOID *Region2;
                    DWORD Region2Size;

                    DWORD ByteToLock = RunningSampleIndex * BytesPerSample % BufferSize;
                    DWORD BytesToWrite;
                    if(BytesToLock > PlayCursor) {
                        BytesToWrite = SecondaryBufferSize - ByteToLock;
                    }

                    if(SUCCEEDED(GlobalSecondaryBuffer->Lock(WritePointer, BytesToWrite,
                                                &Region1, &Region1Size,
                                                &Region2, &Region2Size, 0))) {
                        int16_t *SampleOut = (int16_t *)Region1;
                        DWORD Region1SampleCount = Region1Size/BytesPerSample;
                        DWORD Region2SampleCount = Region2Size/BytesPerSample;
                        for(DWORD SampleIndex = 0;
                            SampleIndex < Region1Size;
                            ++SampleIndex) {
                            
                            if(SquareWaveCounter) SquareWaveCounter = SquareWavePeriod;

                            int16_t SampleValue = (SquareWaveCounter > (SquareWavePeriod/2)) ? 16000: -16000;
                            *SampleOut++ = LEFT;
                            *SampleOut++ = RIGHT;
                            --SquareWaveCounter;
                        }
                        for(DWORD SampleIndex = 0;
                            SampleIndex < Region2Size;
                            ++SampleIndex) {

                            if(SquareWaveCounter) SquareWaveCounter = SquareWavePeriod;

                            int16_t SampleValue = (SquareWaveCounter > (SquareWavePeriod/2)) ? 16000: -16000;
                            *SampleOut++ = LEFT;
                            *SampleOut++ = RIGHT;
                            --SquareWaveCounter;
                        }
                    } else {

                    }
                } else {

                }
                RECT ClientRect;
                GetClientRect(Window, &ClientRect);
                int WindowWidth = ClientRect.right - ClientRect.left;
                int WindowHeight = ClientRect.bottom - ClientRect.top;    
 
                Win32UpdateWindow(DeviceContext, &ClientRect, &GlobalBackbuffer, 0, 0, WindowWidth, WindowHeight);
                ReleaseDC(Window, DeviceContext);
            }
        } else {

        }
    } else { 

    }

     
    return 0;
}