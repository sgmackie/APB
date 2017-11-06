//Generate an exponential decay

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[])
{
    int i, num_steps;
    double step, x, ratio, time, increment, duration;

    if(argc != 4)
    {
        printf("Usage: exponential_decay duration ratio number_of_steps\n");
        return 1;
    }

    duration = atof(argv[1]);
    time = atof(argv[2]);
    num_steps = atof(argv[3]);

    increment = duration / num_steps;
    ratio = exp(-increment/time);
    x = 1.0;
    step = 0.0;

    for(i = 0; i < num_steps; i++)
    {
        printf("%.4lf\t%.8lf\n", step, x);
        x = ratio * x;
        step += increment;
    }

    return 0;
}