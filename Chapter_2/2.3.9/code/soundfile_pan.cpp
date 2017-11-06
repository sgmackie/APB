#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../../misc/psfmaster/include/portsf.h"
#include "../../../misc/include/breakpoints.h"

#define BUFFER_SIZE 1024

typedef struct pan_position
{
    double PanLeft;
    double PanRight;
} PAN_POSITION;

//Prototypes
PAN_POSITION pan_constant_power_panning(double PanPosition);

//Argument list from 0
enum {ARG_NAME, ARG_INFILE, ARG_OUTFILE, ARG_BREAKPOINT_FILE, ARG_NUM_ARGS};

int main(int argc, char *argv[])
{
    //Stage 1: Declare
    PSF_PROPS InputFile_Properties, OutputFile_Properties; //File properties struct from portsf
    long FramesRead, FramesRead_Total;
    psf_format OutputFile_Format;
    PAN_POSITION PanPosition_Current;

    //Breakpoint variables
    unsigned long BreakpointFile_Size;
    FILE *BreakpointFile = NULL;
    BREAKPOINT_FORMAT *ReadPoints = NULL;
    double TimeIncrement;
    double SampleTime;

    //Initialise dyanmic variables to defaults
    int InputFile = -1;
    int OutputFile = -1;
    int ErrorCode = 0;
    double PanPosition = 0;
    float *FramesInput = NULL;
    float *FramesOutput = NULL;
    PSF_CHPEAK *PeakData = NULL; //Peak data struct from portsf
    unsigned long BufferSize = BUFFER_SIZE; //Unsigned, buffer size cannot be a negative number

    printf("soundfile_pan.exe: Pan mono file in stereo from given position values\n");

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
                    return 1;
                
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
                        "Usage: soundfile_pan.exe, input file, output file, pan position\n");
        return 1;
    }

    //Check pan arguments
    if(PanPosition > 1.0 || PanPosition < -1.0)
    {
        fprintf(stderr, "Error: Pan position values out of range\n");
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
    OutputFile_Properties.chans = 2;

    //Allocate memory for output file frames
    FramesOutput = (float*) malloc(BufferSize * OutputFile_Properties.chans * sizeof(float));

    if(FramesOutput == NULL)
    {
        puts("Error: No memory\n"); //Puts uses less system memory than printf, applicable for severe memory errors
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
    if(!breakpoint_inputfile_range_check(ReadPoints, -1.0, 1.0, BreakpointFile_Size))
    {
        fprintf(stderr, "Error: Breakpoint values out of range\n");
        ErrorCode++;
        goto memory_cleanup;
    } 
    
    //Stage 5: Processing loop
    printf("Info: Processing...\n");

    FramesRead_Total = 0;
    PanPosition_Current = pan_constant_power_panning(PanPosition);
    TimeIncrement = 1.0 / InputFile_Properties.srate;
    SampleTime = 0.0;

    //Main loop
    while((FramesRead = psf_sndReadFloatFrames(InputFile, FramesInput, BufferSize)) > 0 )
    {
        //Buffer processing
        int Input_i, Output_i;
        double PanPosition_Stereo;

        for(Input_i = 0, Output_i = 0; Input_i < FramesRead; Input_i++)
        {
            PanPosition_Stereo = breakpoint_value_at_breakpoint_time(ReadPoints, BreakpointFile_Size, SampleTime);
            PanPosition_Current = pan_constant_power_panning(PanPosition_Stereo);
            FramesOutput[Output_i++] = (float) (FramesInput[Input_i]) * PanPosition_Current.PanLeft;
            FramesOutput[Output_i++] = (float) (FramesInput[Input_i]) * PanPosition_Current.PanRight;
            SampleTime += TimeIncrement;
        }

        //Calculate total amount of frames read from the input file
        FramesRead_Total++;
        
        //Update progress for every X number of samples
        if(FramesRead_Total % BufferSize == 0)
        {
            printf("Info: Copying %ld samples...\r", FramesRead_Total);
        }

        //Check for errors writing to the output file
        if(psf_sndWriteFloatFrames(OutputFile, FramesOutput, FramesRead) != FramesRead)
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

PAN_POSITION pan_constant_power_panning(double PanPosition)
{
    PAN_POSITION StereoPosition;
    
    //Pi values for 1/4 of a sinusoid
    const double PiOver2 = 4.0 * atan(1.0) * 0.5;
    const double RootOver2 = sqrt(2.0) * 0.5;

    //Scale position to fit Pi range
    double ScaledPosition = PanPosition * PiOver2;
    double Angle = ScaledPosition * 0.5;

    StereoPosition.PanLeft = RootOver2 * (cos(Angle) - sin(Angle));
    StereoPosition.PanRight = RootOver2 * (cos(Angle) + sin(Angle));

    return StereoPosition;
}