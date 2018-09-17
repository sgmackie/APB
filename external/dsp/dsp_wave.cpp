#include "dsp_wave.h"

#define MAX_SAMPLES (6020)

void dsp_Sine(float32 *Samples, uint16 SampleRate, uint16 Frequency, float32 PhaseStart)
{
    float32 PhaseCurrent = PhaseStart; //Initial is 0 for sine, Pi/2 for cosine
    float32 PhaseIncrement = Frequency * TWOPI / SampleRate; //Generate 1 cycle for a given sample rate and frequency
    
    for(int i = 0; i < MAX_SAMPLES; i++)
    {
        Samples[i] = (float32) sin(PhaseCurrent); //Start by indexing the current phase position
        PhaseCurrent += PhaseIncrement; //Increase phase by the calculated cycle increment

        //Wrap phase 2*Pi as precaution against sin(x) function on different compilers failing to wrap largescale values internally
        if(PhaseCurrent >= TWOPI)
        {
            PhaseCurrent -= TWOPI;
        }

        // debug_PrintLine("%f", Samples[i]);
    }
}
