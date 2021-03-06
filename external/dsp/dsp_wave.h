#ifndef dsp_wave_h
#define dsp_wave_h

#include "math.h"

#ifndef M_PI
#define PI (3.1415926535897932)
#endif

#ifndef TWOPI
#define TWOPI (2 * PI)
#endif

//Structs
//Create oscillator structure
typedef struct OSCILLATOR
{
    float64 TwoPiOverSampleRate; //2 * Pi / Sample rate is a constant variable
    float64 FrequencyCurrent;
    float64 PhaseCurrent;
    float64 PhaseIncrement; //Store calculated phase increment
} OSCILLATOR;

//Prototypes
//Allocate memory for oscillator structure
OSCILLATOR *dsp_OscillatorAlloc();

//Initialise elements of oscillator (can be used to reset)
void dsp_OscillatorInit(OSCILLATOR *Oscillator, uint32 SampleRate);

//Allocation and initialisation functions in one
OSCILLATOR *dsp_OscillatorCreate(uint32 SampleRate);

//Calculate sine wave samples
float64 dsp_TickSine(OSCILLATOR *Oscillator, float32 Frequency);

#endif