#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../../misc/psfmaster/include/portsf.h"

//Prototypes
double max_sample_value(float *ReadBuffer, unsigned long BlockSize);

//Argument list from 0
enum {ARG_NAME, ARG_INFILE, ARG_OUTFILE, ARG_BUFFER_SIZE, ARG_AMPLITUDE, ARG_NUM_ARGS};

int main(int argc, char *argv[])
{
    //Variable declarations
    PSF_PROPS FileProperties; //File properties struct from portsf
    long FramesRead, FramesRead_Total, BlockSize;
    float AmplitudeFactor, ScaleFactor;
    double dBValue;
    unsigned long BufferSize; //Unsigned, buffer size cannot be a negative number

    double InputFile_Peak = 0.0;
    int InputFile = -1;
    int OutputFile = -1;
    int ErrorCode = 0;
    PSF_CHPEAK *CalculatedPeaks = NULL; //Peak data struct from portsf
    float *Buffer = NULL;

    printf("sound_file_normalise: Normalise output copied from input file\n");

    //Check number of arguments provided by user
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
    
    //Open user input file
    InputFile = psf_sndOpen(argv[ARG_INFILE], &FileProperties, 0);

    if(InputFile < 0)
    {
        fprintf(stderr, "Error: Unable to open input file %s\n", argv[ARG_NAME]);
        return 1;
    }

    //Create buffer 'chunk' size from user argument
    BufferSize = atof(argv[ARG_BUFFER_SIZE]);
    
    if(BufferSize < 1)
    {
        fprintf(stderr, "Error: Buffer size must be at least 1\n");
        return 1;
    }

    //Assign amplitude from user argument
    dBValue = atof(argv[ARG_AMPLITUDE]);
    
    if(dBValue > 0.0)
    {
        fprintf(stderr, "Error: dB value cannot be positive\n");
        return 1;
    }

    AmplitudeFactor = (float) pow(10.0, dBValue / 20.0);

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
    CalculatedPeaks = (PSF_CHPEAK*) malloc(FileProperties.chans * sizeof(PSF_CHPEAK));

    if(CalculatedPeaks == NULL)
    {
        puts("Error: No memory\n");
        ErrorCode++;
        goto memory_cleanup;
    }

    //Read peak header from input file
    if(psf_sndReadPeaks(InputFile, CalculatedPeaks, NULL) > 0)
    {
        //Loop for every channel in the input file
        for (long i = 0; i < FileProperties.chans; i++)
        {
            if(CalculatedPeaks[i].val > InputFile_Peak)
            {
                InputFile_Peak = CalculatedPeaks[i].val;
            }
        }
    }

    //Scan file for peaks if format doesn't include peak header
    else
    {
        FramesRead = psf_sndReadFloatFrames(InputFile, Buffer, BufferSize);
        printf("Info: No input peak data found, scanning file\n");

        //Scan file for maximum peak value
        while (FramesRead > 0)
        {
            BlockSize = FramesRead * FileProperties.chans;
            double ThisPeak = max_sample_value(Buffer, BlockSize);
    
            if(ThisPeak > InputFile_Peak)
            {
                InputFile_Peak = ThisPeak;
            }
    
            FramesRead = psf_sndReadFloatFrames(InputFile, Buffer, BufferSize);
        }

        if(FramesRead < 0)
        {
            fprintf(stderr, "Error: Could not scan input file\n");
            goto memory_cleanup;
        }

        //Rewind file to 0
        if((psf_sndSeek(InputFile, 0, PSF_SEEK_SET)) < 0)
        {
            fprintf(stderr, "Error: Unable to rewind input file\n");
            ErrorCode++;
            goto memory_cleanup;
        }
    }

    //Check if input file is silent
    if(InputFile_Peak == 0.0)
    {
        printf("Info: Input file is silent, no output file created\n");
        goto memory_cleanup;
    }

    ScaleFactor = (float) (AmplitudeFactor / InputFile_Peak);

    printf("Info: Normalising by scale factor %.4f...\n", ScaleFactor);

    //Call portsf function for reading input file frames according to the buffer
    FramesRead = psf_sndReadFloatFrames(InputFile, Buffer, BufferSize);
    FramesRead_Total = 0;
    
    //Loop while the portsf function returns 1
    while(FramesRead > 0)
    {
        //Calculate the file size of the buffer
        BlockSize = FramesRead * FileProperties.chans;

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

        for(int i = 0; i < BlockSize; i++)
        {
            Buffer[i] *= ScaleFactor;
        }

        //Check for errors writing to the output file
        if(psf_sndWriteFloatFrames(OutputFile, Buffer, FramesRead) != FramesRead)
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
    if(psf_sndReadPeaks(OutputFile, CalculatedPeaks, NULL) > 0)
    {
        double PeakTime;
        double PeakDB;
        printf("Info: Peaks:\n");

        //Loop for every channel in the input file
        for (long i = 0; i < FileProperties.chans; i++)
        {
            PeakTime = (double) CalculatedPeaks[i].pos / FileProperties.srate; //Calculate the time in seconds according the input file sample rate
            PeakDB = log10(CalculatedPeaks[i].val); //Calculate dB values from linear values
            printf("Channel %d:\t%.4f (%.4f dB) at %.4f seconds\n", i + 1, CalculatedPeaks[i].val, PeakDB, PeakTime);
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

    if(CalculatedPeaks)
    {
        free(CalculatedPeaks);
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