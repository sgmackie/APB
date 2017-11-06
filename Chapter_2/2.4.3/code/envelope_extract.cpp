#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../../misc/psfmaster/include/portsf.h"

#define BUFFER_SIZE (1024)
#define WINDOW_SIZE_MSECS_DEFAULT (15)

//Prototypes
double max_sample_value(float *ReadBuffer, unsigned long BlockSize);

//Argument list from 0
enum {ARG_NAME, ARG_INFILE, ARG_OUTFILE, ARG_NUM_ARGS};

int main(int argc, char *argv[])
{
    //Stage 1: Declare
    PSF_PROPS InputFile_Properties; //File properties struct from portsf
    long FramesRead, FramesRead_Total;
    unsigned long WindowSize, NumPoints;
    double BreakTime;

    //Initialise dyanmic variables to defaults
    int InputFile = -1;
    int ErrorCode = 0;
    double WindowDuration = WINDOW_SIZE_MSECS_DEFAULT;
    FILE *BreakpointFile = NULL;
    PSF_CHPEAK *PeakData = NULL; //Peak data struct from portsf
    unsigned long BufferSize = BUFFER_SIZE; //Unsigned, buffer size cannot be a negative number
    float *FramesInput;

    printf("envelope_extract.exe: Create an evelope breakpoint file from an audio file\n");

    //Stage 2: Argument check
    //Process optional flags
    if(argc > 1)
    {
        char CommandFlag;

        while(argv[1][0] == '-')
        {
            CommandFlag = argv[1][1];

            switch(CommandFlag)
            {
                case('\0'):
                    fprintf(stderr, "Error: Missing flag name\n");
                    ErrorCode++;
                    return 1;
                case('w'):
                    WindowDuration = atof(&argv[1][2]);
                    if(WindowDuration <= 0.0)
                    {
                        fprintf(stderr, "Error: Value for window duration argument must be positive\n");
                        ErrorCode++;
                        return 1;
                    }
                default:
                    break;
            }

            argc--;
            argv++;
        }
    }

    //Check command line arguments
    if(argc < ARG_NUM_ARGS)
    {
        fprintf(stderr, "Error: Insufficient number of arguments\n"
                        "Usage: envelope_extract.exe, window size [-w], input file, output file\n");
        return 1;
    }

    //Start portsf library
    if(psf_init())
    {
        fprintf(stderr, "Error: Unable to start portsf\n");
        return 1;
    }
    
    //Stage 3: Process input file
    //Open user input file
    InputFile = psf_sndOpen(argv[ARG_INFILE], &InputFile_Properties, 0);

    if(InputFile < 0)
    {
        fprintf(stderr, "Error: Unable to open input file %s\n", argv[ARG_INFILE]);
        return 1;
    }

    //Check if input file is mono
    if(InputFile_Properties.chans != 1)
    {
        fprintf(stderr, "Error: Input file (%d channels) must be mono\n", InputFile_Properties.chans);
        ErrorCode++;
        goto memory_cleanup;
    }

    //Allocate memory for input file samples    
    WindowDuration /= 1000; //Convert to seconds
    WindowSize = (unsigned long) (WindowDuration * InputFile_Properties.srate);

    FramesInput = (float*) malloc(WindowSize * sizeof(float));

    if(FramesInput == NULL)
    {
        puts("Error: No memory\n"); //Puts uses less system memory than printf, applicable for severe memory errors
        ErrorCode++;
        goto memory_cleanup;
    }

    //Stage 4: Process output file
    BreakpointFile = fopen(argv[ARG_OUTFILE], "w");

    if(BreakpointFile == NULL)
    {
        fprintf(stderr, "Error: Unable to create breakpoint file\n");
        ErrorCode++;
        goto memory_cleanup;
    }

    //Stage 5: Processing loop
    printf("Info: Processing...\n");

    //Initialise counters
    FramesRead_Total = 0;
    BreakTime = 0.0;
    NumPoints = 0;

    //Main loop
    while((FramesRead = psf_sndReadFloatFrames(InputFile, FramesInput, WindowSize)) > 0 )
    {
        //Buffer processing
        double Amplitude;
        Amplitude = max_sample_value(FramesInput, FramesRead);
        
        if(fprintf(BreakpointFile, "%f\t%f\n", BreakTime, Amplitude) < 2)
        {
            fprintf(stderr, "Error: Failed to write breakpoint file\n");
            ErrorCode++;
            goto memory_cleanup;
        }

        NumPoints++;
        BreakTime += WindowDuration;

        //Calculate total amount of frames read from the input file
        FramesRead_Total++;
        
        //Update progress for every X number of samples
        if(FramesRead_Total % BufferSize == 0)
        {
            printf("Info: Copying %ld samples...\r", FramesRead_Total);
        }
    }

    //Stage 6: User reporting
    if(FramesRead < 0)
    {
        fprintf(stderr, "Error: Cannot read input file %s, output file %s is incomplete\n", argv[ARG_INFILE], argv[ARG_OUTFILE]);
        ErrorCode++;
    }

    else
    {
        printf("\nInfo: Done.\n"
               "Info: %d breakpoints written to %s\n"
               "Info: %d errors\n", NumPoints, argv[ARG_OUTFILE], ErrorCode);
    }

    //Stage 7: Cleanup
    //Close all files and free all allocated memory
    memory_cleanup:

    if(InputFile >= 0)
    {
        if(psf_sndClose(InputFile));
        {
            printf("Warning: Problem closing input file %s\n", argv[ARG_INFILE]);
        }
    }

    if(BreakpointFile)
    {
        if(fclose(BreakpointFile))
        {
            fprintf(stderr, "Error: Failed to close breakpoint file\n");
        }
    }
    
    if(FramesInput)
    {
        free(FramesInput);
    }

    //Close portsf library
    psf_finish();

    return ErrorCode;
}

//Calculate absolute maximum value of input file
double max_sample_value(float *ReadBuffer, unsigned long BlockSize)
{
    double AbsoluteValue;
    double PeakValue = 0.0;
    
    for (unsigned long i = 0; i < BlockSize; i++)
    {
        AbsoluteValue = fabs(ReadBuffer[i]);

        if(AbsoluteValue > PeakValue)
        {
            PeakValue = AbsoluteValue;
        }
    }

    return PeakValue;
}
