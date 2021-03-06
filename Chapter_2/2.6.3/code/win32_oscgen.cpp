//TODO: Continue going through code; moving variables closer to their first use, functioning out reusable code, creating header for common functions commenting


// syntheis_sinetest.exe, outputfile, waveform, duration, frequency, amplitude, sample rate
// synthesis_oscgen.exe test.wav 0 10 440 0.7 44100

//CRT
#include <stdio.h>
#include <stdlib.h>

//Type defines
#include "../../../misc/include/win32_types.h"

//Portsf
#include "../../../misc/psfmaster/include/portsf.h"

//Unity build
#include "../../../external/file/file_memory.cpp"
#include "../../../external/file/file_breakpoints.cpp"
#include "../../../external/dsp/dsp_wave.cpp"

#include "../../../misc/psfmaster/portsf/portsf.c"
#include "../../../misc/psfmaster/portsf/ieee80.c"

//Argument list from 0
enum {ARG_NAME, ARG_OUTFILE, ARG_WAVEFORM, ARG_DURATION, ARG_SAMPLE_RATE, ARG_AMPLITUDE, ARG_FREQUENCY, ARG_NUM_ARGS};

int main(int argc, char *argv[])
{
    //Stage 1: Declare
    //Breakpoint stream variables
    //float64 AmplitudeValueMin, AmplitudeValueMax;

    //Initialise dynamic variables to defaults
    int8 ErrorCode = 0;

    printf("syntheis_oscgen.exe: Generate tone from input parameters\n");
    
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
    int8 WaveType = atoi(argv[ARG_WAVEFORM]);

    //!This breaks the code! Why?
    if(WaveType < WAVE_SINE || WaveType > WAVE_NUM_TYPES)
    {
        fprintf(stderr, "Error: Incorrect waveform argument\n");
        return 1;
    }

    //Stage 2: Handle output file
    //Define output file properties

    PSF_PROPS OutputFileProperties = PSF_PROPS{};
    file_GetDefaultFileProperties(OutputFileProperties, atoi(argv[ARG_SAMPLE_RATE]));
    if(OutputFileProperties.srate <= 0.0)
    {
        fprintf(stderr, "Error: Sample rate must be positive\n");
        return 1;
    }

    //Calculate samples required in blocks and any remainder
    int32 Duration = atof(argv[ARG_DURATION]);
    if(Duration <= 0.0)
    {
        fprintf(stderr, "Error: Duration must be postive\n");
        return 1;
    }

    //TODO: Tidy this function up
    MEMORYBLOCKS SampleBlocks;
    uint64 BufferSize = BUFFER_SIZE; //Unsigned, buffer size cannot be a negative number

    file_TotalOuputSampleSize(SampleBlocks.BlocksTotal, SampleBlocks.RemainderBlocks, Duration, OutputFileProperties.srate, BufferSize);

    //Handle breakpoint file
    FILE *InputBreakpointFile = fopen(argv[ARG_AMPLITUDE], "r");
    float64 Amplitude = atof(argv[ARG_AMPLITUDE]);
    BREAKPOINT_STREAM *AmplitudeStream = nullptr;

    //Check if number is used instead of breakpoint file
    if(InputBreakpointFile == nullptr)
    {
        if(Amplitude <= 0.0 || Amplitude > 1.0)
        {
            fprintf(stderr, "Error: Amplitude values are out of range\n");
            ErrorCode++;
            goto memory_cleanup;
        }
    }
    else
    {
        //Otherwise use breakpoint file
        uint64 AmplitudeStreamSize = 0;

        AmplitudeStream = breakpoint_Stream_New(InputBreakpointFile, OutputFileProperties.srate, &AmplitudeStreamSize);
    }

    //Call function to create oscillator
    WAVEOSC *TestOsc = synthesis_Osc_New(OutputFileProperties.srate);
    
    if(TestOsc == nullptr)
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
    float *FramesOutput = (float *) malloc(BufferSize * OutputFileProperties.chans * sizeof(float));

    if(FramesOutput == nullptr)
    {
        puts("Error: No memory\n");
        ErrorCode++;
        goto memory_cleanup;
    }
    
    //Check extension of output file name
    psf_format OutputFileFormat = psf_getFormatExt(argv[ARG_OUTFILE]);
    
    if(OutputFileFormat == PSF_FMT_UNKNOWN)
    {
        fprintf(stderr, "Error: output file name %s has unknown format\n", argv[ARG_OUTFILE]);
        ErrorCode++;
        goto memory_cleanup;
    }
    
    OutputFileProperties.format = OutputFileFormat;

    //Stage 4: Process output file
    //Allocate memory for the peak data of the output file
    PSF_CHPEAK *PeakData = (PSF_CHPEAK*) malloc(OutputFileProperties.chans * sizeof(PSF_CHPEAK));
    
    if(PeakData == nullptr)
    {
        puts("Error: No memory\n");
        ErrorCode++;
        goto memory_cleanup;
    }

    //Create output file for for reading/writing
    int OutputFile = psf_sndCreate(argv[ARG_OUTFILE], &OutputFileProperties, 0, 0, PSF_CREATE_RDWR);
    
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
            double Frequency = atof(argv[ARG_FREQUENCY]);
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
    if(psf_sndReadPeaks(OutputFile, PeakData, nullptr) > 0)
    {
        double PeakTime;
        double PeakDB;
        printf("Info: Peaks:\n");

        //Loop for every channel in the input file
        for (long i = 0; i < OutputFileProperties.chans; i++)
        {
            PeakTime = (double) PeakData[i].pos / (double) OutputFileProperties.srate; //Calculate the time in seconds according the input file sample rate
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
