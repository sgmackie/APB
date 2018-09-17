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

int main(int argc, char *argv[])
{
    float32 *Samples = (float32 *) malloc((sizeof *Samples) * MAX_SAMPLES);

    dsp_Sine(Samples, 44100, 440, 0);

    free(Samples);

    return 0;
}