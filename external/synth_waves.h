#ifndef synth_waves_h
#define synth_waves_h

#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#ifndef M_PI
#define M_PI (3.1415926535897932)
#endif

#ifndef TWOPI
#define TWOPI (2 * M_PI)
#endif

//Structure for oscillator
typedef struct WaveOsc
{
    double TwoPi_Over_SampleRate;
    double CurrentFrequency;
    double CurrentPhase;
    double PhaseIncrement;
}   WAVEOSC;

//Combined creation and initilaisation function, must be freed after use due to malloc
WAVEOSC *synthesis_Osc_New(double SampleRate);

//Waveform types
//Sine
double synthesis_SineTick(WAVEOSC *SimpleOsc, double Frequency);

//Square
double synthesis_SquareTick(WAVEOSC *SimpleOsc, double Frequency);

//Sawtooth down
double synthesis_SawDownTick(WAVEOSC *SimpleOsc, double Frequency);

//Sawtooth up
double synthesis_SawUpTick(WAVEOSC *SimpleOsc, double Frequency);

//Triangle
double synthesis_TriangleTick(WAVEOSC *SimpleOsc, double Frequency);

//Pointer to function for waveform selection
typedef double (*TICKFUNCTION)(WAVEOSC *TestOsc, double);

#endif