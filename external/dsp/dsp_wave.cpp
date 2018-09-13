#include "../../../external/dsp_wave.h"

#define OSC_WRAP_PHASE  if(SimpleOsc->CurrentPhase > TWOPI) SimpleOsc->CurrentPhase -= TWOPI;	\
if(SimpleOsc->CurrentPhase < 0.0) SimpleOsc->CurrentPhase += TWOPI

//Combined creation and initilaisation function, must be freed after use due to malloc
WAVEOSC *synthesis_Osc_New(float64 SampleRate)
{
    WAVEOSC *SimpleOsc;

    SimpleOsc = (WAVEOSC *) malloc(sizeof(WAVEOSC));

    if(SimpleOsc == NULL)
    {
        return NULL;
    }

    SimpleOsc->TwoPi_Over_SampleRate = TWOPI / SampleRate;
    SimpleOsc->CurrentFrequency = 0.0;
    SimpleOsc->CurrentPhase = 0.0;
    SimpleOsc->PhaseIncrement = 0.0;

    return SimpleOsc;
}

float64 synthesis_SineTick(WAVEOSC *SimpleOsc, float64 Frequency)
{
    float64 SineValue;

    SineValue = sin(SimpleOsc->CurrentPhase);

    if(SimpleOsc->CurrentFrequency != Frequency)
    {
        SimpleOsc->CurrentFrequency = Frequency;
        SimpleOsc->PhaseIncrement = SimpleOsc->TwoPi_Over_SampleRate * Frequency;
    }

    SimpleOsc->CurrentPhase += SimpleOsc->PhaseIncrement;

    OSC_WRAP_PHASE;

    return SineValue;
}

float64 synthesis_SquareTick(WAVEOSC *SimpleOsc, float64 Frequency)
{
    float64 SquareValue;

    if(SimpleOsc->CurrentFrequency != Frequency)
    {
        SimpleOsc->CurrentFrequency = Frequency;
        SimpleOsc->PhaseIncrement = SimpleOsc->TwoPi_Over_SampleRate * Frequency;
    }

    if(SimpleOsc->CurrentPhase <= M_PI)
    {
        SquareValue = 1.0;
    }

    else
    {
        SquareValue = -1;
    }

    SimpleOsc->CurrentPhase += SimpleOsc->PhaseIncrement;

    OSC_WRAP_PHASE;

    return SquareValue;
}

float64 synthesis_SawDownTick(WAVEOSC *SimpleOsc, float64 Frequency)
{
    float64 SawDownValue;

    if(SimpleOsc->CurrentFrequency != Frequency)
    {
        SimpleOsc->CurrentFrequency = Frequency;
        SimpleOsc->PhaseIncrement = SimpleOsc->TwoPi_Over_SampleRate * Frequency;
    }

    SawDownValue = 1.0 - 2.0 * (SimpleOsc->CurrentPhase * (1.0 / TWOPI));

    SimpleOsc->CurrentPhase += SimpleOsc->PhaseIncrement;

    OSC_WRAP_PHASE;

    return SawDownValue;
}

float64 synthesis_SawUpTick(WAVEOSC *SimpleOsc, float64 Frequency)
{
    float64 SawUpValue;

    if(SimpleOsc->CurrentFrequency != Frequency)
    {
        SimpleOsc->CurrentFrequency = Frequency;
        SimpleOsc->PhaseIncrement = SimpleOsc->TwoPi_Over_SampleRate * Frequency;
    }

    SawUpValue = (2.0 * (SimpleOsc->CurrentPhase * (1.0 / TWOPI))) - 1.0;

    SimpleOsc->CurrentPhase += SimpleOsc->PhaseIncrement;

    OSC_WRAP_PHASE;

    return SawUpValue;
}

float64 synthesis_TriangleTick(WAVEOSC *SimpleOsc, float64 Frequency)
{
    float64 TriangleValue;

    if(SimpleOsc->CurrentFrequency != Frequency)
    {
        SimpleOsc->CurrentFrequency = Frequency;
        SimpleOsc->PhaseIncrement = SimpleOsc->TwoPi_Over_SampleRate * Frequency;
    }

    if(SimpleOsc->CurrentPhase <= M_PI)
    {
        TriangleValue = (4.0 * (SimpleOsc->CurrentPhase * (1.0 / TWOPI))) - 1.0;
    }

    else
    {
        TriangleValue = 3.0 - 4.0 * (SimpleOsc->CurrentPhase * (1.0 / TWOPI));
    }

    SimpleOsc->CurrentPhase += SimpleOsc->PhaseIncrement;

    OSC_WRAP_PHASE;

    return TriangleValue;
}

/*     //Wrap around 2Pi
    if(SimpleOsc->CurrentPhase >= TWOPI)
    {
        SimpleOsc->CurrentPhase -= TWOPI;
    }

    //Negative number wrapping
    if(SimpleOsc->CurrentPhase < 0.0)
    {
        SimpleOsc->CurrentPhase += TWOPI;
    } */