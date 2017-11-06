#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../../misc/psfmaster/include/portsf.h"

enum {ARG_NAME, ARG_INFILE, ARG_OUTFILE, ARG_BUFFER_SIZE, ARG_NUM_ARGS};

int main(int argc, char *argv[])
{
    PSF_PROPS FileProperties;
    long FramesRead, FramesRead_Total;

    int InputFile = -1;
    int OutputFile = -1;
    int ErrorCode = 0;
    psf_format OutputFile_Format = PSF_FMT_UNKNOWN;
    PSF_CHPEAK *OutputFile_Peaks = NULL;
    float *Buffer = NULL;
    DWORD BufferSize;

    printf("file_to_float: Convert soundfile to float sample format\n");

    if(argc < ARG_NUM_ARGS)
    {
        fprintf(stderr, "Error: Insufficient number of arguments\n"
                        "Usage: file_to_float.exe, input file, output file, buffer size\n");
        return 1;
    }

    if(psf_init())
    {
        fprintf(stderr, "Error: Unable to start portsf\n");
        return 1;
    }
    
    InputFile = psf_sndOpen(argv[ARG_INFILE], &FileProperties, 0);

    if(InputFile < 0)
    {
        fprintf(stderr, "Error: Unable to open input file %s\n", argv[ARG_NAME]);
        return 1;
    }

    if(FileProperties.samptype == PSF_SAMP_IEEE_FLOAT)
    {
        printf("Info: Input file %s is already in float format\n", argv[ARG_INFILE]);
        return 1;
    }

    BufferSize = (DWORD) atoi(argv[ARG_BUFFER_SIZE]);
    
    if(BufferSize < 1)
    {
        fprintf(stderr, "Error: Buffer size must be at least 1\n");
        return 1;
    }

    FileProperties.samptype = PSF_SAMP_IEEE_FLOAT;
    OutputFile_Format = psf_getFormatExt(argv[ARG_OUTFILE]);

    if(OutputFile_Format == PSF_FMT_UNKNOWN)
    {
        fprintf(stderr, "Error: Output file %s has unknown format\n", argv[ARG_OUTFILE]);
        ErrorCode++;
        goto memory_cleanup;
    }

    FileProperties.format = OutputFile_Format;
    OutputFile = psf_sndCreate(argv[ARG_OUTFILE], &FileProperties, 0, 0, PSF_CREATE_RDWR);

    if(OutputFile < 0)
    {
        fprintf(stderr, "Error: Unable to create output file %s\n", argv[ARG_OUTFILE]);
        ErrorCode++;
        goto memory_cleanup;
    }

    Buffer = (float*) malloc(FileProperties.chans * sizeof(float) * BufferSize);

    if(Buffer == NULL)
    {
        //Puts uses less system memory than printf, applicable for severe memory errors
        puts("Error: No memory\n");
        ErrorCode++;
        goto memory_cleanup;
    }

    OutputFile_Peaks = (PSF_CHPEAK*) malloc(FileProperties.chans * sizeof(PSF_CHPEAK));

    if(OutputFile_Peaks == NULL)
    {
        puts("Error: No memory\n");
        ErrorCode++;
        goto memory_cleanup;
    }

    printf("Info: Copying...\n");

    FramesRead = psf_sndReadFloatFrames(InputFile, Buffer, BufferSize);
    FramesRead_Total = 0;
    
    while(FramesRead == 1)
    {
        FramesRead_Total++;

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
        printf("Info: Done.\n"
               "Info: %d sample frames copied to %s\n", FramesRead_Total, argv[ARG_OUTFILE]);
    }

    if(psf_sndReadPeaks(OutputFile, OutputFile_Peaks, NULL) > 0)
    {
        double PeakTime;
        double PeakDB;
        printf("Info: Peaks:\n");

        for (long i = 0; i < FileProperties.chans; i++)
        {
            PeakTime = (double) OutputFile_Peaks[i].pos / FileProperties.srate;
            PeakDB = log10(OutputFile_Peaks[i].val);
            printf("Channel %d:\t%.4f (%.4f dB) at %.4f seconds\n", i + 1, OutputFile_Peaks[i].val, PeakDB, PeakTime);
        }
    }

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

    psf_finish();

    return ErrorCode;
}