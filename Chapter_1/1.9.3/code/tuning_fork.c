#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI (3.141592654)
#endif

enum {ARG_NAME, ARG_OUTFILE, ARG_DURATION, ARG_FREQUENCY, ARG_SAMPLE_RATE, ARG_SLOPE, ARG_NUM_ARGS};

int main(int argc, char* argv[])
{
    int i, num_samples;
    double sample, duration, frequency, sample_rate, time, ratio, slope, angle_increment, x;
    double double_pi = 2.0 * M_PI;
    double max_sample = 0.0;

    FILE* file_pointer = NULL;

    if(argc != ARG_NUM_ARGS)
    {
        fprintf(stderr, "Usage: tuning_fork, outfile.txt, duration, frequency, sample rate, slope\n");
        return 1;
    }

    file_pointer = fopen(argv[ARG_OUTFILE], "w");

    if(file_pointer == NULL)
    {
        fprintf(stderr, "Error: Unable to create output file %s\n", argv[ARG_OUTFILE]);
        return 1;
    }

    duration = atof(argv[ARG_DURATION]);
    frequency = atof(argv[ARG_FREQUENCY]);
    sample_rate = atof(argv[ARG_SAMPLE_RATE]);
    slope = atof(argv[ARG_SLOPE]);

    num_samples = (int) (duration * sample_rate);
    angle_increment = double_pi * frequency / sample_rate;
    time = duration / num_samples;
    ratio = exp(-time / slope);
    x = 1.0;

    for (i = 0; i < num_samples; i++)
    {
        sample = sin(angle_increment * i);
        x *= ratio;
        sample *= x;
        fprintf(file_pointer, "%.8lf\n", sample);
    }

    fclose(file_pointer);
    printf("Done\n");

    return 0;
}