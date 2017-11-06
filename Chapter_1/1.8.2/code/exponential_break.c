#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[])
{
    int i, num_points;
    double value_start, value_end, value_range;
    double duration, step, position_start, position_end, current_step;
    double factor, offset;
    const double lowest_amplitude = 1.0e-4; // Expression for -80db

    if (argc != 5)
    {
        fprintf(stderr, "Usage: exponential_break, duration, number of points, start value, end value\n");
        return 1;
    }

    duration = atof(argv[1]);

    if(duration <= 0.0)
    {
        fprintf(stderr, "Error: Duration (%f) must be positive\n", duration);
        return 1;
    }

    num_points = atoi(argv[2]);

    if(num_points <= 0)
    {
        fprintf(stderr, "Error: Number of points (%d) must be positive\n", num_points);
        return 1;
    }

    step = duration / num_points;

    value_start = atof(argv[3]);
    value_end = atof(argv[4]);

    value_range = value_end - value_start;

    if(value_range == 0.0)
    {
        fprintf(stderr, "Error: Start (%f) and end (%f) values are the same \n", value_start, value_end);
        return 1;
    }

    if(value_start > value_end)
    {
        position_start = 1.0;
        position_end = lowest_amplitude;
        value_range = -value_range;
        offset = value_end;
    }    

    else
    {
        position_start  = lowest_amplitude;
        position_end = 1.0;
        offset = value_start;
    }

    current_step = 0.0;

    factor = pow(position_end / position_start, 1.0 / num_points);

    for (i = 0; i < num_points; i++)
    {
        fprintf(stdout, "%.4lf\t%.8lf\n", current_step, offset + (position_start * value_range));
        position_start *= factor;
        current_step += step;
    }

    //fprintf(stdout, "%.4lf\t%.8lf\n", current_step, offset + (position_start * value_range));
    fprintf(stderr, "Done\n");

    return 0;
}