#ifndef dsp_wave_h
#define dsp_wave_h

#include "math.h"

#ifndef M_PI
#define M_PI (3.1415926535897932)
#endif

#ifndef TWOPI
#define TWOPI (2 * M_PI)
#endif


void dsp_Sine(uint16 SampleRate, uint16 Frequency, float32 PhaseStart);

#endif