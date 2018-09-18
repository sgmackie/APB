#include "dsp_wave.h"

//Allocate memory for oscillator structure
OSCILLATOR *dsp_OscillatorAlloc()
{
    OSCILLATOR *Result = (OSCILLATOR *) malloc((sizeof *Result));

    if(!Result)
    {
        return nullptr;
    }

    return Result;
}

//Initialise elements of oscillator (can be used to reset)
void dsp_OscillatorInit(OSCILLATOR *Oscillator, uint32 SampleRate)
{
    Oscillator->TwoPiOverSampleRate = TWOPI / SampleRate;
    Oscillator->FrequencyCurrent = 0;
    Oscillator->PhaseCurrent = 0;
    Oscillator->PhaseIncrement = 0;
}

//Allocation and initialisation functions in one
OSCILLATOR *dsp_OscillatorCreate(uint32 SampleRate)
{
    OSCILLATOR *Result = dsp_OscillatorAlloc();
    dsp_OscillatorInit(Result, SampleRate);

    return Result;
}

//Calculate sine wave samples
float64 dsp_TickSine(OSCILLATOR *Oscillator, float32 Frequency)
{
    float64 Result;

    Result = (float64) sin(Oscillator->PhaseCurrent); //Input in radians

    if(Oscillator->FrequencyCurrent != Frequency)
    {
        Oscillator->FrequencyCurrent = Frequency;
        Oscillator->PhaseIncrement = Oscillator->TwoPiOverSampleRate * Frequency;
    }

    Oscillator->PhaseCurrent += Oscillator->PhaseIncrement; //Increase phase by the calculated cycle increment
    
    //Wrap phase 2*Pi as precaution against sin(x) function on different compilers failing to wrap large scale values internally
    if(Oscillator->PhaseCurrent >= TWOPI)
    {
        Oscillator->PhaseCurrent -= TWOPI;
    }

    if(Oscillator->PhaseCurrent < 0)
    {
        Oscillator->PhaseCurrent += TWOPI;
    }

    return Result;
}