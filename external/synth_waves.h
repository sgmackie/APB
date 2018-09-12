#ifndef synth_waves_h
#define synth_waves_h

#include "math.h"

#ifndef M_PI
#define M_PI (3.1415926535897932)
#endif

#ifndef TWOPI
#define TWOPI (2 * M_PI)
#endif

//Waveform type arguments
enum {WAVE_SINE, WAVE_TRIANGLE, WAVE_SQAURE, WAVE_SAW_DOWN, WAVE_SAW_UP, WAVE_NUM_TYPES};

//Structure for oscillator
typedef struct WAVEOSC
{
    float64 TwoPi_Over_SampleRate;
    float64 CurrentFrequency;
    float64 CurrentPhase;
    float64 PhaseIncrement;
}   WAVEOSC;

//Combined creation and initilaisation function, must be freed after use due to malloc
WAVEOSC *synthesis_Osc_New(float64 SampleRate);

//Waveform types
//Sine
float64 synthesis_SineTick(WAVEOSC *SimpleOsc, float64 Frequency);

//Square
float64 synthesis_SquareTick(WAVEOSC *SimpleOsc, float64 Frequency);

//Sawtooth down
float64 synthesis_SawDownTick(WAVEOSC *SimpleOsc, float64 Frequency);

//Sawtooth up
float64 synthesis_SawUpTick(WAVEOSC *SimpleOsc, float64 Frequency);

//Triangle
float64 synthesis_TriangleTick(WAVEOSC *SimpleOsc, float64 Frequency);

//Pointer to function for waveform selection
typedef float64 (*TICKFUNCTION)(WAVEOSC *TestOsc, float64);

#endif