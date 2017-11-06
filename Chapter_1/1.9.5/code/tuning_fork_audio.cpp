#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI (3.141592654)
#endif

enum {ARG_NAME, ARG_OUTFILE, ARG_DURATION, ARG_FREQUENCY, ARG_SAMPLE_RATE, ARG_AMPLITUDE, ARG_TYPE, ARG_NUM_ARGS};
enum sample_type {SAMPLE_SHORT, SAMPLE_FLOAT};

int byte_order()
{
    int one = 1;
    char* endptr = (char *) &one;
    return (*endptr);
}

const char* endianness[2] = {"big_endian", "little_endian"};

int main(int argc, char* argv[])
{
    unsigned int i, num_samples;
    unsigned int max_frame = 0;
    unsigned int sample_type, endian, bit_reverse;
    double sample, duration, frequency, sample_rate, amplitude, step;
    double start, end, ratio, max_sample;
    double double_pi = 2.0 * M_PI;
    double angle_increment;

    FILE* file_ptr = NULL;
    float float_sample;
    short short_sample;

    if(argc != ARG_NUM_ARGS)
    {
        fprintf(stderr, "Usage: tuning_fork_audio, outfile.raw, duration, frequency, sample rate, amplitude, type\n");
        return 1;
    }

    duration = atof(argv[ARG_DURATION]);
    frequency = atof(argv[ARG_FREQUENCY]);
    sample_rate = atof(argv[ARG_SAMPLE_RATE]);
    amplitude = atof(argv[ARG_AMPLITUDE]);
    sample_type = (unsigned int) atoi(argv[ARG_TYPE]);

    if(sample_type > 1)
    {
        fprintf(stderr, "Error: Sample type (%d) can only be 1 or 0\n", sample_type);
        return 1;
    }

    file_ptr = fopen(argv[ARG_OUTFILE], "wb");
    
    if(file_ptr == NULL)
    {
        fprintf(stderr, "Error: Unable to create output file %s\n", argv[ARG_OUTFILE]);
        return 1;
    }

    num_samples = (int) (duration * sample_rate);
    angle_increment = double_pi * frequency / num_samples;
    step = duration / num_samples;

    start = 1.0;
    end = 1.0e-4;
    max_sample = 0.0;
    ratio = pow(end / start, 1.0 / num_samples);
    endian = byte_order();

    printf("Writing %d %s samples\n", num_samples, endianness[endian]);

    if(sample_type == SAMPLE_SHORT)
    {
        for (i = 0; i < num_samples; i++)
        {
            sample = amplitude * sin(angle_increment * i);
            sample *= start;
            start *= ratio;
            short_sample = (short) (sample * 32767.0);

            if(fwrite(&short_sample, sizeof(short), 1, file_ptr) != 1)
            {
                fprintf(stderr, "Error: Cannot write to file\n");
                return 1;
            }

            if(fabs(sample) > max_sample)
            {
                max_sample = fabs(sample);
                max_frame = i;
            }
        }
    }

    else
    {
        for (i = 0; i < num_samples; i++)
        {
            sample = amplitude * sin(angle_increment * i);
            sample *= start;
            start *= ratio;
            float_sample = (float) sample;

            if(fwrite(&short_sample, sizeof(float), 1, file_ptr) != 1)
            {
                fprintf(stderr, "Error: Cannot write to file\n");
                return 1;
            }

            if(fabs(sample) > max_sample)
            {
                max_sample = fabs(sample);
                max_frame = i;
            }
        }
    }

    fclose(file_ptr);
    printf("Done. Maximum sample value = %.8lf at frame %d\n", max_sample, max_frame);

    return 0;
}