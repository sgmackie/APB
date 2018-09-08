#define BUFFER_SIZE 1024

//Struct to hold both the number of memory blocks to allocate and any remainders from the calculation
typedef struct MEMORYBLOCKS
{
    unsigned long BlocksTotal;
    long RemainderBlocks;
} MEMORYBLOCKS;

//Calculate the number of memory blocks by multiplying the duration of the output file by it's sample rate
void file_TotalOuputSampleSize(unsigned long &CalculatedBlocks, long &RemainderBlocks, double Duration, int SampleRate, long BufferSize);

void file_TotalOuputSampleSize(unsigned long &CalculatedBlocks, long &RemainderBlocks, double Duration, int SampleRate, long BufferSize)
{
    unsigned long OutputSamplesTotal = (unsigned long) (Duration * SampleRate + 0.5);

    CalculatedBlocks = OutputSamplesTotal / BufferSize;
    RemainderBlocks = OutputSamplesTotal - CalculatedBlocks * BufferSize;
   
    if(RemainderBlocks)
    {
        CalculatedBlocks++;
    }
}