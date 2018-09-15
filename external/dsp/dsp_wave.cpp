#include "dsp_wave.h"

#define MAX_SAMPLES (6020)

//TODO: Comment this! How does the phase increment work?

void dsp_Sine(uint16 SampleRate, uint16 Frequency, float32 PhaseStart)
{
    float32 PhaseCurrent = 0;
    float32 PhaseIncrement = Frequency * TWOPI / SampleRate;
    float32 *Samples = (float32 *) malloc(sizeof (float32) * MAX_SAMPLES);

    for(int i = 0; i < MAX_SAMPLES; i++)
    {
        Samples[i] = (float32) sin(PhaseCurrent);
        PhaseCurrent += PhaseIncrement;

        if(PhaseCurrent >= TWOPI)
        {
            PhaseCurrent -= TWOPI;
        }

        printf("%f\n", Samples[i]);
    }
}

