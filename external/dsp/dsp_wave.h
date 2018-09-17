#ifndef dsp_wave_h
#define dsp_wave_h

#include "math.h"

#ifndef M_PI
#define PI (3.1415926535897932)
#endif

#ifndef TWOPI
#define TWOPI (2 * PI)
#endif

void dsp_Sine(float32 *Samples, uint16 SampleRate, uint16 Frequency, float32 PhaseStart);

#endif