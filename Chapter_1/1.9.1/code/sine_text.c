#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI (3.141592654)
#endif

enum {ARG_NAME, ARG_NUM_SAMPLES, ARG_FREQUENCY, ARG_SAMPLE_RATE, ARG_NUM_ARGS};
                      
int main(int argc, char* argv[])
{
    int i, num_samples;
    double sample, frequency, sample_rate, angle_increment;
    double double_pi = 2.0 * M_PI;

    if(argc != ARG_NUM_ARGS)
    {
        fprintf(stderr, "Usage: sine_text, number of samples, frequency, sample rate\n");
        return 1;
    }

    num_samples = atoi(argv[ARG_NUM_SAMPLES]);
    frequency = atof(argv[ARG_FREQUENCY]);
    sample_rate = atof(argv[ARG_SAMPLE_RATE]);

    angle_increment = double_pi * frequency / sample_rate;
    
    fprintf(stdout, "angle increment = %f", angle_increment);

    for (i = 0; i < num_samples; i++)
    {
        sample = sin(angle_increment * i);
        //fprintf(stdout, "%.8lf\n", sample);
        fprintf(stdout, "%.8lf\t%.8lf\n", sample, sample * sample);
    }

    fprintf(stderr, "Done\n");

    return 0;
}
