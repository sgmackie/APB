#include <stdio.h>
#include <stdlib.h>

#define NPOINTS (64)

//Create the breakpoint structure, example: 2secs - 0.6amplitude
typedef struct BreakpointFormat 
{
    double breakpoint_time;
    double breakpoint_value;
}   BREAKPOINT_FORMAT;

//Extract points
BREAKPOINT_FORMAT breakpoint_max_point_value(const BREAKPOINT_FORMAT* points, long total_points)
{
    int i;
    BREAKPOINT_FORMAT point;

    //Start from first point
    point.breakpoint_time = points[0].breakpoint_time;
    point.breakpoint_value = points[0].breakpoint_value;

    for(i = 0; i < total_points; i++)
    {
        if(point.breakpoint_value < points[i].breakpoint_value)
        {
            point.breakpoint_value = points[i].breakpoint_value;
            point.breakpoint_time = points[i].breakpoint_time; 
        }
    }

    return point;
}


//Function for reading input text file
BREAKPOINT_FORMAT* get_breakpoints(FILE* fp, long* psize)
{
    int text_file_output;
    long total_points = 0, array_size = 64;
    double lasttime = 0.0;
    BREAKPOINT_FORMAT* points = NULL;
    char breakpoint_line[80];

    //If file fails to open
    if(fp == NULL)
    {
        return NULL;
    }

    points = (BREAKPOINT_FORMAT*) malloc(sizeof(BREAKPOINT_FORMAT) * array_size);

    if(points == NULL)
    {
        return NULL;
    }

    while(fgets(breakpoint_line, 80, fp))
    {
        text_file_output = sscanf(breakpoint_line, "%lf%lf", &points[total_points].breakpoint_time, &points[total_points].breakpoint_value);

        //Check if empty breakpoint_line in text file, contine loop if there is
        if(text_file_output < 0)
        {
            continue;
        }
        
        if(text_file_output == 0)
        {
            printf("Line &d has non-nummeric data\n", total_points+1);
            break;
        }
   
        if(text_file_output == 1)
        {
            printf("Incomplete breakpoint found at point %d\n", total_points+1);
            break;
        }

        if(points[total_points].breakpoint_time < lasttime)
        {
            printf("Data error at point %d: Time no inscreasing\n", total_points+1);
            break;
        }

        lasttime = points[total_points].breakpoint_time;

        if(++total_points == array_size)
        {
            BREAKPOINT_FORMAT* tmp;
            array_size += NPOINTS;
        
            tmp = (BREAKPOINT_FORMAT*) realloc(points, sizeof(BREAKPOINT_FORMAT) *array_size);

            //Release all memory
            if(tmp == NULL)
            {
                total_points = 0;
                free(points);
                points = NULL;
                break;
            }

            points = tmp;
        }
    }
    if(total_points)
    {
        *psize = total_points;
    }    

    return points;
}


int main(int argc, char* argv[])
{
    long array_size;
    double dur;
    BREAKPOINT_FORMAT point, *points;
    FILE* fp;

    printf("breakdur: Find duration of breakpoint file\n");

    if(argc < 0)
    {
        printf("Usage: breakdur inputfile.txt \n");
        return 1;
    }

    fp = fopen(argv[1], "r");
    if(fp == NULL)
    {   
        printf("Unable to open file\n");
        return 1;
    }

    array_size = 0;
    points = get_breakpoints(fp, &array_size);

    if(points == NULL)
    {
        printf("No breakpoints read\n");
        fclose(fp);
        return 1;
    }

    if(array_size < 2)
    {
        printf("Error, at least two breakpoints required\n");
        free(points);
        fclose(fp);
        return 1;
    }

    if(points[0].breakpoint_time != 0.0)
    {
        printf("First breakpoint breakpoint_time must be 0.0\n");
        free(points);
        fclose(fp);
        return 1;
    }

    printf("Read %d breakpoints\n", array_size);
    dur = points[array_size-1].breakpoint_time;
    printf("Duration: %f seconds\n", dur);
    point = breakpoint_max_point_value(points, array_size);
    printf("Maximum breakpoint_value: %f at %f secs\n", point.breakpoint_value, point.breakpoint_time);
    
    free(points);
    fclose(fp);

    return 0;
}