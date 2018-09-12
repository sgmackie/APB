#ifndef file_memory_h
#define file_memory_h

#define BUFFER_SIZE 1024

//Struct to hold both the number of memory blocks to allocate and any remainders from the calculation
typedef struct MEMORYBLOCKS
{
    uint64 BlocksTotal;
    int64 RemainderBlocks;
} MEMORYBLOCKS;

//Calculate the number of memory blocks by multiplying the duration of the output file by it's sample rate
void file_TotalOuputSampleSize(uint64 &CalculatedBlocks, int64 &RemainderBlocks, float64 Duration, int32 SampleRate, int64 BufferSize);

void file_TotalOuputSampleSize(uint64 &CalculatedBlocks, int64 &RemainderBlocks, float64 Duration, int32 SampleRate, int64 BufferSize)
{
    uint64 OutputSamplesTotal = (uint64) (Duration * SampleRate + 0.5);

    CalculatedBlocks = OutputSamplesTotal / BufferSize;
    RemainderBlocks = OutputSamplesTotal - CalculatedBlocks * BufferSize;
   
    if(RemainderBlocks)
    {
        CalculatedBlocks++;
    }
}

void file_GetDefaultFileProperties(PSF_PROPS &OutputFile)
{
    OutputFile.chans = 1;
    OutputFile.samptype = (psf_stype) PSF_SAMP_16;
    OutputFile.chformat = STDWAVE;    
}

#endif