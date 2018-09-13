#include "../external/file_memory.h"

void file_GetDefaultFileProperties(PSF_PROPS &OutputFile, uint16 SampleRate)
{
    OutputFile.srate = SampleRate;
    OutputFile.chans = 1;
    OutputFile.samptype = (psf_stype) PSF_SAMP_16;
    OutputFile.chformat = STDWAVE;    
}

//Calculate the number of memory blocks by multiplying the duration of the output file by it's sample rate
void file_TotalOuputSampleSize(uint64 &CalculatedBlocks, int64 &RemainderBlocks, int32 Duration, uint16 SampleRate, int64 BufferSize)
{
    uint64 OutputSamplesTotal = (uint64) (Duration * SampleRate + 0.5);

    CalculatedBlocks = OutputSamplesTotal / BufferSize;
    RemainderBlocks = OutputSamplesTotal - CalculatedBlocks * BufferSize;
   
    if(RemainderBlocks)
    {
        CalculatedBlocks++;
    }
}

