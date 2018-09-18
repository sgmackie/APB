//CRT
#include <stdlib.h>

//Type defines
#include "../../../misc/include/win32_types.h"

//Debug
#include "../../../misc/include/debug/debug_macros.h"

//Portsf
#include "../../../misc/psfmaster/portsf/portsf.c"
#include "../../../misc/psfmaster/portsf/ieee80.c"

//Unity build
#include "../../../external/file/file_memory.cpp"
#include "../../../external/dsp/dsp_wave.cpp"

#define MAX_SAMPLES (6020)

int main(int argc, char *argv[])
{
    float32 *Samples = (float32 *) malloc((sizeof *Samples) * MAX_SAMPLES);
    OSCILLATOR *TestOsc = dsp_OscillatorCreate(44100);

    for(int i = 0; i < MAX_SAMPLES; i++)
    {
        Samples[i] = dsp_TickSine(TestOsc, 440);

        //debug_PrintLine(Console, "%f", Samples[i]);
    }
    
    free(Samples);

    return 0;
}