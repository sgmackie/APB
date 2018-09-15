//CRT
#include <stdio.h>
#include <stdlib.h>

//Type defines
#include "../../../misc/include/win32_types.h"

//Portsf
#include "../../../misc/psfmaster/include/portsf.h"

//Unity build
#include "../../../external/file/file_memory.cpp"
#include "../../../external/file/file_breakpoints.cpp"
#include "../../../external/dsp/dsp_wave.cpp"

#include "../../../misc/psfmaster/portsf/portsf.c"
#include "../../../misc/psfmaster/portsf/ieee80.c"

int main(int argc, char *argv[])
{
    dsp_Sine(44100, 440, 0);

    return 0;
}




//TODO: Push Macbook commits
//TODO: Add 40 minutes to Kanban + 40 minutes + 20 minutes