#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../../misc/psfmaster/include/portsf.h"

enum {ARG_NAME, ARG_INFILE, ARG_OUTFILE, ARG_BUFFER_SIZE, ARG_AMPLITUDE, ARG_NUM_ARGS};

int main(int argc, char *argv[])
{
    //Variable declarations
    PSF_PROPS FileProperties; //File properties struct from portsf
    DWORD BufferSize; //Unsigned int, buffer size cannot be a negative number
    long FramesRead, FramesRead_Total;
    float AmplitudeFactor;

    int InputFile = -1;
    int OutputFile = -1;
    int ErrorCode = 0;
    PSF_CHPEAK *OutputFile_Peaks = NULL; //Peak data struct from portsf
    float *Buffer = NULL;

    printf("sound_file_gain: Change amplitude of input file\n");

    //Check number of arguments provided by user
    if(argc < ARG_NUM_ARGS)
    {
        fprintf(stderr, "Error: Insufficient number of arguments\n"
                        "Usage: sound_file_gain.exe, input file, output file, buffer size, amplitude\n");
        return 1;
    }

    //Start portsf library
    if(psf_init())
    {
        fprintf(stderr, "Error: Unable to start portsf\n");
        return 1;
    }
    
    //Open user input file
    InputFile = psf_sndOpen(argv[ARG_INFILE], &FileProperties, 0);

    if(InputFile < 0)
    {
        fprintf(stderr, "Error: Unable to open input file %s\n", argv[ARG_NAME]);
        return 1;
    }

    //Create buffer 'chunk' size from user argument
    BufferSize = (DWORD) atoi(argv[ARG_BUFFER_SIZE]);
    
    if(BufferSize < 1)
    {
        fprintf(stderr, "Error: Buffer size must be at least 1\n");
        return 1;
    }

    //Assign amplitude from user argument
    AmplitudeFactor = atof(argv[ARG_AMPLITUDE]);
    
    if(AmplitudeFactor <= 0)
    {
        fprintf(stderr, "Error: Amplitdue factor must be greater than 0\n");
        return 1;
    }

    //Create output file for for reading/writing
    OutputFile = psf_sndCreate(argv[ARG_OUTFILE], &FileProperties, 0, 0, PSF_CREATE_RDWR);

    if(OutputFile < 0)
    {
        fprintf(stderr, "Error: Unable to create output file %s\n", argv[ARG_OUTFILE]);
        ErrorCode++;
        goto memory_cleanup;
    }

    //Allocate memory for buffer according to number of channels in input file and the buffer size
    Buffer = (float*) malloc(BufferSize * FileProperties.chans * sizeof(float));

    if(Buffer == NULL)
    {
        //Puts uses less system memory than printf, applicable for severe memory errors
        puts("Error: No memory\n");
        ErrorCode++;
        goto memory_cleanup;
    }

    //Allocate memory for the peak data of the output file
    OutputFile_Peaks = (PSF_CHPEAK*) malloc(FileProperties.chans * sizeof(PSF_CHPEAK));

    if(OutputFile_Peaks == NULL)
    {
        puts("Error: No memory\n");
        ErrorCode++;
        goto memory_cleanup;
    }

    printf("Info: Copying...\n");

    //Call portsf function for reading input file frames according to the buffer
    FramesRead = psf_sndReadFloatFrames(InputFile, Buffer, BufferSize);
    FramesRead_Total = 0;
    
    //Loop while the portsf function returns 1
    while(FramesRead == 1)
    {
        //Calculate total amount of frames read from the input file
        FramesRead_Total++;

        //Counter declare
        int j = 0;
        j++;

        //Update progress for every 1000 samples
        if(j % 1000 == 0)
        {
            printf("Info: Copying %ld samples...\r", FramesRead_Total);
        }

        for(int i = 0; i < FileProperties.chans; i++)
        {
            Buffer[i] *= AmplitudeFactor;
        }

        //Check for errors writing to the output file
        if(psf_sndWriteFloatFrames(OutputFile, Buffer, BufferSize) != 1)
        {
            fprintf(stderr, "Error: Cannot write to ouput file %s\n", argv[ARG_OUTFILE]);
            ErrorCode++;
            break;
        }

        FramesRead = psf_sndReadFloatFrames(InputFile, Buffer, BufferSize);
    }

    if(FramesRead < 0)
    {
        fprintf(stderr, "Error: Cannot read input file %s, output file %s is incomplete\n", argv[ARG_INFILE], argv[ARG_OUTFILE]);
        ErrorCode++;
    }

    else
    {
        printf("\nInfo: Done.\n"
               "Info: %d sample frames copied to %s\n", FramesRead_Total, argv[ARG_OUTFILE]);
    }

    //Check if any peak data was captured
    if(psf_sndReadPeaks(OutputFile, OutputFile_Peaks, NULL) > 0)
    {
        double PeakTime;
        double PeakDB;
        printf("Info: Peaks:\n");

        //Loop for every channel in the input file
        for (long i = 0; i < FileProperties.chans; i++)
        {
            PeakTime = (double) OutputFile_Peaks[i].pos / FileProperties.srate; //Calculate the time in seconds according the input file sample rate
            PeakDB = log10(OutputFile_Peaks[i].val); //Calculate dB values from linear values
            printf("Channel %d:\t%.4f (%.4f dB) at %.4f seconds\n", i + 1, OutputFile_Peaks[i].val, PeakDB, PeakTime);
        }
    }

    //Close all files and free all allocated memory
    memory_cleanup:

    if(InputFile >= 0)
    {
        psf_sndClose(InputFile);
    }

    if(OutputFile >= 0)
    {
        psf_sndClose(OutputFile);
    }
    
    if(Buffer)
    {
        free(Buffer);
    }

    if(OutputFile_Peaks)
    {
        free(OutputFile_Peaks);
    }

    //Close portsf library
    psf_finish();

    return ErrorCode;
}