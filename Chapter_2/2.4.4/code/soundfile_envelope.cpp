#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../../misc/psfmaster/include/portsf.h"
#include "../../../misc/include/breakpoints.h"

#define BUFFER_SIZE 1024

//Argument list from 0
enum {ARG_NAME, ARG_INFILE, ARG_OUTFILE, ARG_BREAKPOINT_FILE, ARG_NUM_ARGS};

int main(int argc, char *argv[])
{
    //Stage 1: Declare
    PSF_PROPS InputFile_Properties, OutputFile_Properties; //File properties struct from portsf
    long FramesRead, FramesRead_Total;

    //Breakpoint variables
    unsigned long BreakpointFile_Size, CounterLeft, CounterRight;
    BREAKPOINT_FORMAT LeftPoint, RightPoint;
    double TimeIncrement, CurrentPosition, Width, Height;
    int MorePoints;

    //Initialise dyanmic variables to defaults
    int InputFile = -1;
    int OutputFile = -1;
    int ErrorCode = 0;
    float *FramesInput = NULL;
    float *FramesOutput = NULL;
    psf_format OutputFile_Format = PSF_FMT_UNKNOWN;
    PSF_CHPEAK *PeakData = NULL; //Peak data struct from portsf
    unsigned long BufferSize = BUFFER_SIZE; //Unsigned, buffer size cannot be a negative number
    FILE *BreakpointFile = NULL;
    BREAKPOINT_FORMAT *ReadPoints = NULL;

    printf("soundfile_pan.exe: Pan mono file in stereo from given position values\n");

    //Stage 2: Argument check

    //Check command line arguments
    if(argc < ARG_NUM_ARGS)
    {
        fprintf(stderr, "Error: Insufficient number of arguments\n"
                        "Usage: soundfile_pan.exe, input file, output file, pan position\n");
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
        ErrorCode++;
        goto memory_cleanup;
    }

    //Check if input file is mono
    if(InputFile_Properties.chans != 1)
    {
        fprintf(stderr, "Error: Input file must be mono\n");
        ErrorCode++;
        goto memory_cleanup;
    }

    //Allocate memory for input file samples
    FramesInput = (float*) malloc(BufferSize * InputFile_Properties.chans * sizeof(float));

    if(FramesInput == NULL)
    {
        puts("Error: No memory\n"); //Puts uses less system memory than printf, applicable for severe memory errors
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

    //Copy struct from input file to output file
    InputFile_Properties.format = OutputFile_Format;
    OutputFile_Properties = InputFile_Properties;

    //Breakpoint file processing
    //Open text file
    BreakpointFile = fopen(argv[ARG_BREAKPOINT_FILE], "r");
    
    if(BreakpointFile == NULL)
    {
        fprintf(stderr, "Error: Unable to open breakpoint file %s\n", argv[ARG_BREAKPOINT_FILE]);
        ErrorCode++;
        goto memory_cleanup;
    }    

    //Call function to gather breakpoints
    BreakpointFile_Size = 0;
    ReadPoints = breakpoint_get_breakpoints(BreakpointFile, &BreakpointFile_Size);

    if(ReadPoints == NULL)
    {
        fprintf(stderr, "Error: No breakpoints read\n");
        ErrorCode++;
        goto memory_cleanup;
    }

    //Check number of breakpoints in file
    if(BreakpointFile_Size < 2)
    {
        fprintf(stderr, "Error: At least two breakpoints required\n");
        ErrorCode++;
        goto memory_cleanup;
    }

    //Check if first time value isn't 0
    if(ReadPoints[0].BreakpointTime != 0.0)
    {
        fprintf(stderr, "Error: First breakpoint time (%lf) must start from 0\n", ReadPoints[0].BreakpointTime);
        ErrorCode++;
        goto memory_cleanup;
    }

    //Check if breakpoint values are out of range
    if(!breakpoint_inputfile_range_check(ReadPoints, 0.0, 1.0, BreakpointFile_Size))
    {
        fprintf(stderr, "Error: Breakpoint values out of range\n");
        ErrorCode++;
        goto memory_cleanup;
    } 
       
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

    //Processed frames counter
    FramesRead_Total = 0;

    //Initial time position for reading envelope
    TimeIncrement = 1.0 / InputFile_Properties.srate;
    
    //Counters to track through breakpoint file
    CurrentPosition = 0.0;
    CounterLeft = 0;
    CounterRight = 1;

    //Create first time span
    LeftPoint = ReadPoints[CounterLeft];
    RightPoint = ReadPoints[CounterRight];
    Width = RightPoint.BreakpointTime - LeftPoint.BreakpointTime;
    Height = RightPoint.BreakpointValue - LeftPoint.BreakpointValue; 
    MorePoints = 1;

    //Main loop
    while((FramesRead = psf_sndReadFloatFrames(InputFile, FramesInput, BufferSize)) > 0 )
    {
        //Buffer processing
        int Input_i;
        double Amplitude, Fraction;

        for(Input_i = 0; Input_i < FramesRead; Input_i++)
        {
            if(MorePoints)
            {
                if(Width == 0.0)
                {
                    Amplitude = RightPoint.BreakpointValue;
                }
    
                else
                {
                    Fraction = (CurrentPosition - LeftPoint.BreakpointTime) / Width;
                    Amplitude = LeftPoint.BreakpointValue + (Height * Fraction);
                }
    
                CurrentPosition += TimeIncrement;
    
                //Check if end of span is reached, move to next
                if(CurrentPosition > RightPoint.BreakpointTime)
                {
                    CounterLeft++;
                    CounterRight++;
    
                    //Move to next span
                    if(CounterRight < BreakpointFile_Size)
                    {
                        LeftPoint = ReadPoints[CounterLeft];
                        RightPoint = ReadPoints[CounterRight];
                        Width = RightPoint.BreakpointTime - LeftPoint.BreakpointTime;
                        Height = RightPoint.BreakpointValue - LeftPoint.BreakpointValue; 
                    }

                    else
                    {
                        MorePoints = 0;
                    }
                }
            }

            FramesInput[Input_i] = (float) (FramesInput[Input_i] * Amplitude);
        }

        //Calculate total amount of frames read from the input file
        FramesRead_Total++;
        
        //Update progress for every X number of samples
        if(FramesRead_Total % BufferSize == 0)
        {
            printf("Info: Copying %ld samples...\r", FramesRead_Total);
        }

        //Check for errors writing to the output file
        if(psf_sndWriteFloatFrames(OutputFile, FramesInput, FramesRead) != FramesRead)
        {
            fprintf(stderr, "Error: Cannot write to ouput file %s\n", argv[ARG_OUTFILE]);
            ErrorCode++;
            break;
        }
    }

    if(FramesRead < 0)
    {
        fprintf(stderr, "Error: Cannot read input file %s, output file %s is incomplete\n", argv[ARG_INFILE], argv[ARG_OUTFILE]);
        ErrorCode++;
    }

    else
    {
        printf("\nInfo: Done.\n"
               "Info: %d errors\n", ErrorCode);
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

    if(InputFile >= 0)
    {
        if(psf_sndClose(InputFile));
        {
            printf("Warning: Problem closing input file %s\n", argv[ARG_INFILE]);
        }
    }

    if(OutputFile >= 0)
    {
        if(psf_sndClose(OutputFile));
        {
            printf("Warning: Problem closing output file %s\n", argv[ARG_OUTFILE]);
        }
    }
    
    if(BreakpointFile)
    {
        fclose(BreakpointFile);
    }

    if(FramesInput)
    {
        free(FramesInput);
    }

    if(FramesOutput)
    {
        free(FramesOutput);
    }

    if(PeakData)
    {
        free(PeakData);
    }

    if(ReadPoints)
    {
        free(ReadPoints);
    }

    //Close portsf library
    psf_finish();

    return ErrorCode;
}