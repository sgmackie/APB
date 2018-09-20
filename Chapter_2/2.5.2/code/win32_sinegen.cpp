//CRT
#include <stdlib.h>
#include <windows.h>
#include <xaudio2.h>

//Type defines
#include "../../../misc/include/win32_types.h"

//Debug
#include "../../../misc/include/debug/debug_macros.h"

// //Portsf
// #include "../../../misc/psfmaster/portsf/portsf.c"
// #include "../../../misc/psfmaster/portsf/ieee80.c"

//Unity build
#include "../../../external/dsp/dsp_wave.cpp"

#define MAX_SAMPLES (6020)

typedef struct WIN32_SOUNDOUTPUT
{
    uint32 SampleRate;
    uint32 RunningSampleIndex;
    int32 BytesPerSample;
    int32 SecondaryBufferSize;
    float64 OutputSamples;
    int32 LatencySampleCount;
} WIN32_SOUNDOUTPUT;

//Instead of calling GetClientRect all the time, create struct for window dimensions and store them
typedef struct WIN32_WINDOW_DIMENSIONS
{
    int32 Width;
    int32 Height;
} WIN32_WINDOW_DIMENSIONS;

global bool32 GlobalRunning;

//Macros for XAudio2.dll function pointers
#define XAUDIO2_CREATE(FunctionName) HRESULT FunctionName(IXAudio2 **ppXAudio2, UINT32 Flags, XAUDIO2_PROCESSOR XAudio2Processor)
typedef XAUDIO2_CREATE(win32_XAudio2_Create);

internal void win32_XAudio2_Init(uint8 ChannelNumber, uint32 SampleRate, uint8 BitDepth, XAUDIO2_BUFFER *AudioBuffer, uint32 AudioBufferSize)
{
    //Load .dll
    HMODULE XAudio2Library = LoadLibrary("xaudio2_9.dll");

    //XAudio2 is a COM object so COM must be initialised
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    if(XAudio2Library)
    {
        debug_PrintLine(Console, "XAudio2:\t Loaded XAudio2.dll");

        //Create XAudio2 object
        win32_XAudio2_Create *XAudio2Create = (win32_XAudio2_Create *) GetProcAddress(XAudio2Library, "XAudio2Create");
        IXAudio2 *XAudio2Instance = nullptr;

        if(XAudio2Create && SUCCEEDED(XAudio2Create(&XAudio2Instance, 0, XAUDIO2_DEFAULT_PROCESSOR)))
        {
            debug_PrintLine(Console, "XAudio2:\t Created XAudio2 instance");

            //Create mastering voice which passes all audio data to the sound card
            IXAudio2MasteringVoice *XAudio2MasteringVoice = nullptr;

            if(SUCCEEDED(XAudio2Instance->CreateMasteringVoice(&XAudio2MasteringVoice)))
            {   
                debug_PrintLine(Console, "XAudio2:\t Created XAudio2 mastering voice");

                //Set format of wave audio stream
                WAVEFORMATEX WaveFormat = {};                                                           //Init
                WaveFormat.wFormatTag = WAVE_FORMAT_PCM;                                                //Wave data format (PCM, DRM, ADPCM)
                WaveFormat.nChannels = ChannelNumber;                                                   //Number of channels
                WaveFormat.nSamplesPerSec = SampleRate;                                                 //Sampling rate
                WaveFormat.wBitsPerSample = BitDepth;                                                   //Bit depth
                WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;        //For PCM, block alignment (in bytes) must be equal to Channels * SampleRate / 8 (bits)
                WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;        //Average data transfer rate in bytes per second, for PCM, must be equal to SampleRate * BlockAlignment
                WaveFormat.cbSize = 0;                                                                  //Size (in bytes) of extra format information
                
                debug_PrintLine(Console, "XAudio2:\t Set WaveFormat properties");

                IXAudio2SourceVoice *XAudio2SourceVoice;
                if(SUCCEEDED(XAudio2Instance->CreateSourceVoice(&XAudio2SourceVoice, &WaveFormat)))
                {
                    debug_PrintLine(Console, "XAudio2:\t Created XAudio2 source voice");

                    //Set audio buffer properties
                    AudioBuffer->Flags = 0;                                     //Additional info for the buffer
                    AudioBuffer->AudioBytes = AudioBufferSize;                  //Size of the audio data in bytes (cannot exceed XAUDIO2_MAX_BUFFER_BYTES)
                    AudioBuffer->pAudioData = (const BYTE*) VirtualAlloc(0, AudioBufferSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
                    // (const BYTE*) VirtualAlloc(0, AudioBufferSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
                    AudioBuffer->PlayBegin = 0;                                 //First sample in the buffer to play
                    AudioBuffer->PlayLength = 0;                                //Length of region to play, 0 means whole buffer
                    AudioBuffer->LoopBegin = 0;                                 //Loop region starting point
                    AudioBuffer->LoopLength = 0;                                //Length in samples of loop region
                    AudioBuffer->LoopCount = XAUDIO2_LOOP_INFINITE;             //Number of times to loop through the loop region
                    AudioBuffer->pContext = nullptr;                            //Context for callbacks
                    
                    debug_PrintLine(Console, "XAudio2:\t Set audio buffer properties source voice");

                    if(SUCCEEDED(XAudio2SourceVoice->SubmitSourceBuffer(AudioBuffer)))
                    {
                        debug_PrintLine(Console, "XAudio2:\t Passed audio buffer to source voice");

                        if (SUCCEEDED(XAudio2SourceVoice->Start(0)))
                        {
                            debug_PrintLine(Console, "XAudio2:\t Play");
                        }     
                    }
                }
            }
        }   
    }
}


//Replaces GetClientRect calls
internal WIN32_WINDOW_DIMENSIONS win32_Display_WindowGetDimensions(HWND Window)
{
    WIN32_WINDOW_DIMENSIONS WindowDimensions;

    RECT ClientRect; //Rect structure for window dimensions
    GetClientRect(Window, &ClientRect); //Function to get current window dimensions in a RECT format
    
    WindowDimensions.Width = ClientRect.right - ClientRect.left;
    WindowDimensions.Height = ClientRect.bottom - ClientRect.top;

    return WindowDimensions;
}

//Callback function as Windows is free to pass this function when it pleases                                      
LRESULT CALLBACK win32_MainWindow_Callback(HWND Window, UINT UserMessage, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;

    switch(UserMessage)
    {
        case WM_SIZE:
        {   
            OutputDebugStringA("WM_SIZE\n");
            break;
        }

        case WM_DESTROY:
        {
            GlobalRunning = false;
            OutputDebugStringA("WM_DESTROY\n");
            break;
        }

        case WM_CLOSE:
        {
            GlobalRunning = false;
            OutputDebugStringA("WM_CLOSE\n");
            break;
        }

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
            break;
        }

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            //Virtual Keycode, keys without direct ANSI mappings
            uint32 VKCode = WParam;

            //Check if key was down, if bit == 30 then it's true, otherwise false
            bool32 WasDown = ((LParam & (1 << 30)) != 0);
            //Check if being held down
            bool32 IsDown = ((LParam & (1 << 31)) == 0);

            if(WasDown != IsDown)
            {
                if(VKCode == 'W')
                {
                    OutputDebugStringA("W\n");
                }
                else if(VKCode == 'A')
                {
                    OutputDebugStringA("A\n");
                }
                else if(VKCode == 'S')
                {
                    OutputDebugStringA("S\n");
                }
                else if(VKCode == 'D')
                {
                    OutputDebugStringA("D\n");
                }
                else if(VKCode == 'Q')
                {
                    OutputDebugStringA("Q\n");
                }
                else if(VKCode == 'E')
                {
                    OutputDebugStringA("E\n");
                }
                else if(VKCode == VK_UP)
                {
                    OutputDebugStringA("Up\n");
                }
                else if(VKCode == VK_DOWN)
                {
                    OutputDebugStringA("Down\n");
                }
                else if(VKCode == VK_LEFT)
                {
                    OutputDebugStringA("Left\n");
                }
                else if(VKCode == VK_RIGHT)
                {
                    OutputDebugStringA("Right\n");
                }
                else if(VKCode == VK_SPACE)
                {
                    OutputDebugStringA("Space\n");
                }
                else if(VKCode == VK_ESCAPE)
                {
                    OutputDebugStringA("Escape\n");
                }
            }

            bool32 AltKeyWasDown = ((LParam & (1 << 29)) != 0);

            if((VKCode == VK_F4) && AltKeyWasDown)
            {
                GlobalRunning = false;
            }
            
            break;
        }

        case WM_PAINT: //Call paint functions to draw to the window
        {
            WIN32_WINDOW_DIMENSIONS WindowDimensions = win32_Display_WindowGetDimensions(Window);
        }

        default:
        {
            //OutputDebugStringA("default\n");
            Result = DefWindowProc(Window, UserMessage, WParam, LParam); //Default window procedure
            break;
        }
    }

    return Result;
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int CommandShow)
{
    WNDCLASS WindowClass = {};

    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;                         //Create unique device context for this window
    WindowClass.lpfnWndProc = win32_MainWindow_Callback;                            //Call the window process
    WindowClass.hInstance = Instance;                                               //Handle instance passed from win32_WinMain
    WindowClass.lpszClassName = "XAudio2TestBench_WindowClass";                     //Name of Window class

    float32 *Samples = (float32 *) malloc((sizeof *Samples) * MAX_SAMPLES);
    OSCILLATOR *TestOsc = dsp_OscillatorCreate(44100);

    XAUDIO2_BUFFER *AudioBuffer = (XAUDIO2_BUFFER *) malloc((sizeof *AudioBuffer));

    win32_XAudio2_Init(2, 44100, 16, AudioBuffer, 48160);

    if(RegisterClass(&WindowClass))
    {
        HWND Window = CreateWindowEx(0, WindowClass.lpszClassName, "XAudio2 Test Bench", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, Instance, 0);

        if(Window)
        {
            HDC DeviceContext = GetDC(Window);

            bool32 SoundIsPlaying = false;
            GlobalRunning = true;

            while(GlobalRunning)
            {
                MSG Messages;

                while(PeekMessage(&Messages, 0, 0, 0, PM_REMOVE))
                {
                    if(Messages.message == WM_QUIT)
                    {
                        GlobalRunning = false;
                    }

                    TranslateMessage(&Messages);
                    DispatchMessage(&Messages);

                    for(int i = 0; i < MAX_SAMPLES; i++)
                    {
                        Samples[i] = dsp_TickSine(TestOsc, 440);

                        debug_PrintLine(Console, "%f", Samples[i]);
                    }
                }
            }
        }
    }

    free(Samples);

    return 0;
}


// int main(int argc, char *argv[])
// {
//     float32 *Samples = (float32 *) malloc((sizeof *Samples) * MAX_SAMPLES);
//     OSCILLATOR *TestOsc = dsp_OscillatorCreate(44100);

//     XAUDIO2_BUFFER *AudioBuffer = (XAUDIO2_BUFFER *) malloc((sizeof *AudioBuffer));

//     win32_XAudio2_Init(2, 44100, 16, AudioBuffer, 48160);

//     //TODO: Go through HH code, look how sound buffer is being filled
//     //("https://docs.microsoft.com/en-us/windows/desktop/xaudio2/how-to--play-a-sound-with-xaudio2")

//     for(int i = 0; i < MAX_SAMPLES; i++)
//     {
//         Samples[i] = dsp_TickSine(TestOsc, 440);

//         //debug_PrintLine(Console, "%f", Samples[i]);
//     }
    
//     free(Samples);

//     return 0;
// }