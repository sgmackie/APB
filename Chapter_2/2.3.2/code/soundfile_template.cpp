#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../../misc/psfmaster/include/portsf.h"

#define BUFFER_SIZE (1024)

//Argument list from 0
enum {ARG_NAME, ARG_INFILE, ARG_OUTFILE, ARG_NUM_ARGS};

int main(int argc, char *argv[])
{
    //Stage 1: Declare
    PSF_PROPS InputFile_Properties, OutputFile_Properties; //File properties struct from portsf
    long FramesRead, FramesRead_Total;

    //Initialise dyanmic variables to defaults
    int InputFile = -1;
    int OutputFile = -1;
    int ErrorCode = 0;
    PSF_CHPEAK *PeakData = NULL; //Peak data struct from portsf
    unsigned long BufferSize = BUFFER_SIZE; //Unsigned, buffer size cannot be a negative number
    psf_format OutputFile_Format;
    float *FramesInput;
    float *FramesOutput;

    printf("soundfile_template.exe: Used as basis for audio processing code\n");

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
                        "Usage: sound_file_gain.exe, input file, output file, buffer size, dB value\n");
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

    InputFile_Properties.format = OutputFile_Format;
    OutputFile_Properties = InputFile_Properties;

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

    //Initialise counter
    FramesRead_Total = 0;

    //Main loop
    while((FramesRead = psf_sndReadFloatFrames(InputFile, FramesInput, BufferSize)) > 0 )
    {
        //Buffer processing

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

    //Stage 6: User reporting
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

    //Check if any peak data was captured
    if(psf_sndReadPeaks(OutputFile, PeakData, NULL) > 0)
    {
        double PeakTime;
        double PeakDB;
        printf("Info: Peaks:\n");

        //Loop for every channel in the input file
        for (long i = 0; i < InputFile_Properties.chans; i++)
        {
            PeakTime = (double) PeakData[i].pos / (double) InputFile_Properties.srate; //Calculate the time in seconds according the input file sample rate
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
    
    if(FramesInput)
    {
        free(FramesInput);
    }

    if(PeakData)
    {
        free(PeakData);
    }

    //Close portsf library
    psf_finish();

    return ErrorCode;
}

