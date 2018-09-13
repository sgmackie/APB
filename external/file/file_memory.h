#ifndef file_memory_h
#define file_memory_h

#define BUFFER_SIZE 1024

//Struct to hold both the number of memory blocks to allocate and any remainders from the calculation
typedef struct MEMORYBLOCKS
{
    uint64 BlocksTotal;
    int64 RemainderBlocks;
} MEMORYBLOCKS;


void file_GetDefaultFileProperties(PSF_PROPS &OutputFile, uint16 SampleRate);

//Calculate the number of memory blocks by multiplying the duration of the output file by it's sample rate
void file_TotalOuputSampleSize(uint64 &CalculatedBlocks, int64 &RemainderBlocks, int32 Duration, uint16 SampleRate, int64 BufferSize);

#endif