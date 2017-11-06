#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI (3.141592654)
#endif

enum {ARG_NAME, ARG_OUTFILE, ARG_DURATION, ARG_FREQUENCY, ARG_SAMPLE_RATE, ARG_AMPLITUDE, ARG_NUM_ARGS};

int main(int argc, char* argv[])
{
    int i, num_samples;
    double sample, duration, frequency, sample_rate, amplitude, max_sample;
    double start, end, ratio, angle_increment;
    double double_pi = 2.0 * M_PI;

    FILE* file_pointer = NULL;

    if(argc != ARG_NUM_ARGS)
    {
        fprintf(stderr, "Usage: tuning_fork, outfile.txt, duration, frequency, sample rate, amplitude\n");
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
    amplitude = atof(argv[ARG_AMPLITUDE]);

    num_samples = (int) (duration * sample_rate);
    angle_increment = double_pi * frequency / sample_rate;
    start = 1.0;
    end = 1.0e-4;
    max_sample = 0.0;
    ratio = pow(end / start, 1.0 / num_samples);

    for (i = 0; i < num_samples; i++)
    {
        sample = amplitude * sin(angle_increment * i);
        sample *= start;
        start *= ratio;
        fprintf(file_pointer, "%.8lf\n", sample);

        if(fabs(sample) > max_sample)
        {
            max_sample = fabs(sample);
        }

    }

    fclose(file_pointer);
    printf("Done. Maximum sample value = %.8lf\n", max_sample);

    return 0;
}