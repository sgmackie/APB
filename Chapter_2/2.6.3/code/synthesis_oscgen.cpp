//TODO: Continue going through code; moving variables closer to their first use, functioning out reusable code, creating header for common functions commenting

#include <stdio.h>
#include <stdlib.h>
#include "../../../misc/psfmaster/include/portsf.h"
#include "../../../external/synth_waves.h"
#include "../../../external/file_breakpoints.h"
#include "../../../external/file_memory.h"

//Unity build
#include "..\code\synthesis_wave.cpp"
#include "..\code\soundfile_breakpoints.cpp"
#include "..\..\..\misc\psfmaster\portsf\portsf.c"
#include "..\..\..\misc\psfmaster\portsf\ieee80.c"

//Argument list from 0
enum {ARG_NAME, ARG_OUTFILE, ARG_WAVEFORM, ARG_DURATION, ARG_SAMPLE_RATE, ARG_AMPLITUDE, ARG_FREQUENCY, ARG_NUM_ARGS};

//Waveform type arguments
enum {WAVE_SINE, WAVE_TRIANGLE, WAVE_SQAURE, WAVE_SAW_DOWN, WAVE_SAW_UP, WAVE_NUM_TYPES};

int main(int argc, char *argv[])
{
    //Stage 1: Declare
    //Oscillator variables
    double Frequency, Amplitude;
    int WaveType;
    WAVEOSC *TestOsc = NULL;

    //Breakpoint stream variables
    double AmplitudeValue_Min, AmplitudeValue_Max;
    BREAKPOINT_STREAM *AmplitudeStream = NULL;
    unsigned long AmplitudeStream_Size = 0;
    FILE *InputBreakpointFile = NULL;

    //Initialise dynamic variables to defaults
    int OutputFile = -1;
    int ErrorCode = 0;
    float *FramesOutput = NULL;
    psf_format OutputFile_Format = PSF_FMT_UNKNOWN;
    PSF_CHPEAK *PeakData = NULL; //Peak data struct from portsf

    printf("syntheis_sinetest.exe: Generate mono sine wave\n");
    
    //Stage 2: Argument check
    //Check command line arguments
    if(argc < ARG_NUM_ARGS)
    {
        fprintf(stderr, "Error: Insufficient number of arguments\n"
                        "Usage: syntheis_sinetest.exe, outputfile, waveform, duration, frequency, amplitude, sample rate\n"
                        "Usage: Square = 0\n"
                        "Usage: Triangle = 1\n"
                        "Usage: Sawtooth Up = 2\n"
                        "Usage: Sawtooth Down = 3\n");
        return 1;
    }

    //Check Waveform argument  
    WaveType = atoi(argv[ARG_WAVEFORM]);

    if(WaveType < WAVE_SINE || WaveType > WAVE_NUM_TYPES)
    {
        fprintf(stderr, "Error: Incorrect waveform argument\n");
        return 1;
    }

    //Stage 2: Handle output file
    //Define output file properties
    PSF_PROPS OutputFile_Properties;
    OutputFile_Properties.srate = atoi(argv[ARG_SAMPLE_RATE]);

    if(OutputFile_Properties.srate <= 0.0)
    {
        fprintf(stderr, "Error: Sample rate must be positive\n");
        return 1;
    }

    OutputFile_Properties.chans = 1;
    OutputFile_Properties.samptype = (psf_stype) PSF_SAMP_16;
    OutputFile_Properties.chformat = STDWAVE;    

    //Calculate samples required in blocks and any remainder
    double Duration = atof(argv[ARG_DURATION]);

    if(Duration <= 0.0)
    {
        fprintf(stderr, "Error: Duration must be postive\n");
        return 1;
    }

    //TODO: Tidy this function up
    MEMORYBLOCKS SampleBlocks;
    long BufferSize = BUFFER_SIZE; //Unsigned, buffer size cannot be a negative number

    file_TotalOuputSampleSize(SampleBlocks.BlocksTotal, SampleBlocks.RemainderBlocks, Duration, OutputFile_Properties.srate, BufferSize);

    Frequency = atof(argv[ARG_FREQUENCY]);

    //Handle breakpoint file
    InputBreakpointFile = fopen(argv[ARG_AMPLITUDE], "r");

    //Check if number is used instead of breakpoint file
    if(InputBreakpointFile == NULL)
    {
        Amplitude = atof(argv[ARG_AMPLITUDE]);

        if(Amplitude <= 0.0 || Amplitude > 1.0)
        {
            fprintf(stderr, "Error: Amplitude values are out of range\n");
            ErrorCode++;
            goto memory_cleanup;
        }
    }

    //Otherwise use breakpoint file
    else
    {
        AmplitudeStream = breakpoint_Stream_New(InputBreakpointFile, OutputFile_Properties.srate, &AmplitudeStream_Size);
    }

    //Call function to create oscillator
    TestOsc = synthesis_Osc_New(OutputFile_Properties.srate);
    
    if(TestOsc == NULL)
    {
        puts("Error: No memory for oscillator\n");
        ErrorCode++;
        goto memory_cleanup;
    }

    //Start portsf library
    if(psf_init())
    {
        fprintf(stderr, "Error: Unable to start portsf\n");
        ErrorCode++;
        goto memory_cleanup;
    }

    //Allocate output frame memory
    FramesOutput = (float *) malloc(BufferSize * OutputFile_Properties.chans * sizeof(float));

    if(FramesOutput == NULL)
    {
        puts("Error: No memory\n");
        ErrorCode++;
        goto memory_cleanup;
    }
    
    //Check extension of output file name
    OutputFile_Format = psf_getFormatExt(argv[ARG_OUTFILE]);
    
    if(OutputFile_Format == PSF_FMT_UNKNOWN)
    {
        fprintf(stderr, "Error: output file name %s has unknown format\n", argv[ARG_OUTFILE]);
        ErrorCode++;
        goto memory_cleanup;
    }
    
    OutputFile_Properties.format = OutputFile_Format;

    //Stage 4: Process output file
    //Allocate memory for the peak data of the output file
    PeakData = (PSF_CHPEAK*) malloc(OutputFile_Properties.chans * sizeof(PSF_CHPEAK));
    
    if(PeakData == NULL)
    {
        puts("Error: No memory\n");
        ErrorCode++;
        goto memory_cleanup;
    }

    //Create output file for for reading/writing
    OutputFile = psf_sndCreate(argv[ARG_OUTFILE], &OutputFile_Properties, 0, 0, PSF_CREATE_RDWR);
    
    if(OutputFile < 0)
    {
        fprintf(stderr, "Error: Unable to create output file %s\n", argv[ARG_OUTFILE]);
        ErrorCode++;
        goto memory_cleanup;
    }    

    //Stage 5: Processing loop
    printf("Info: Processing...\n");

    //Main loop
    for(unsigned long i = 0; i < SampleBlocks.BlocksTotal; i++)
    {
        if(i == SampleBlocks.BlocksTotal - 1)
        {
            BufferSize = SampleBlocks.RemainderBlocks;
        }

        for(long j = 0; j < BufferSize; j++)
        {
            TICKFUNCTION TickSelect = nullptr;

            if(AmplitudeStream)
            {
                Amplitude = breakpoint_Stream_ValueAtTime(AmplitudeStream);
            }
            
            FramesOutput[j] = (float) (Amplitude * TickSelect(TestOsc, Frequency));
        }

        if(psf_sndWriteFloatFrames(OutputFile, FramesOutput, BufferSize) != BufferSize)
        {
            fprintf(stderr, "Error: Cannot write to output file\n");
            ErrorCode++;
            goto memory_cleanup;
            break;
        }
    }

    //Stage 6: User reporting
    //Check if any peak data was captured
    if(psf_sndReadPeaks(OutputFile, PeakData, NULL) > 0)
    {
        double PeakTime;
        double PeakDB;
        printf("Info: Peaks:\n");

        //Loop for every channel in the input file
        for (long i = 0; i < OutputFile_Properties.chans; i++)
        {
            PeakTime = (double) PeakData[i].pos / (double) OutputFile_Properties.srate; //Calculate the time in seconds according the input file sample rate
            PeakDB = log10(PeakData[i].val); //Calculate dB values from linear values
            printf("Channel %d:\t%.4f (%.4f dB) at %.4f seconds\n", i + 1, PeakData[i].val, PeakDB, PeakTime);
        }
    }

    //Stage 7: Cleanup
    //Close all files and free all allocated memory
    memory_cleanup:

    if(OutputFile >= 0)
    {
        if(psf_sndClose(OutputFile));
        {
            printf("Warning: Problem closing output file %s\n", argv[ARG_OUTFILE]);
        }
    }

    if(FramesOutput)
    {
        free(FramesOutput);
    }

    if(TestOsc)
    {
        free(TestOsc);
    }

    if(AmplitudeStream)
    {
        breakpoint_Stream_Free(AmplitudeStream);
        free(AmplitudeStream);
    }

    if(InputBreakpointFile)
    {
        if(fclose(InputBreakpointFile))
        {
            printf("Error: Cannot close breakpoint file\n");
        }
    }

    if(PeakData)
    {
        free(PeakData);
    }

    //Close portsf library
    psf_finish();

    //Report error code
    return ErrorCode;
}
