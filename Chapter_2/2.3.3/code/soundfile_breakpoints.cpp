#include "stdio.h"
#include "stdlib.h"
#include "../../../misc/include/breakpoints.h"

#define NPOINTS (64)

//Extract points read from file
BREAKPOINT_FORMAT breakpoint_max_point_value(const BREAKPOINT_FORMAT* ReadPoints, long ReadPoints_Total)
{
    int i;
    BREAKPOINT_FORMAT Point;

    //Start from first point
    Point.BreakpointTime = ReadPoints[0].BreakpointTime;
    Point.BreakpointValue = ReadPoints[0].BreakpointValue;

    for(i = 0; i < ReadPoints_Total; i++)
    {
        if(Point.BreakpointValue < ReadPoints[i].BreakpointValue)
        {
            Point.BreakpointValue = ReadPoints[i].BreakpointValue;
            Point.BreakpointTime = ReadPoints[i].BreakpointTime; 
        }
    }

    return Point;
}

//Function for reading input text file
BREAKPOINT_FORMAT* breakpoint_get_breakpoints(FILE* FilePointer, unsigned long* PointerSize)
{
    int TextFile_Output;
    long ReadPoints_Total = 0, ArraySize = 64;
    double ReadPoints_LastTime = 0.0;
    BREAKPOINT_FORMAT* ReadPoints = NULL;
    char Breakpoint_LineLength[80];

    //If file fails to open
    if(FilePointer == NULL)
    {
        return NULL;
    }

    ReadPoints = (BREAKPOINT_FORMAT*) malloc(sizeof(BREAKPOINT_FORMAT) * ArraySize);

    if(ReadPoints == NULL)
    {
        return NULL;
    }

    while(fgets(Breakpoint_LineLength, 80, FilePointer))
    {
        TextFile_Output = sscanf(Breakpoint_LineLength, "%lf%lf", &ReadPoints[ReadPoints_Total].BreakpointTime, &ReadPoints[ReadPoints_Total].BreakpointValue);

        //Check if empty Breakpoint_LineLength in text file, contine loop if there is
        if(TextFile_Output < 0)
        {
            continue;
        }
        
        if(TextFile_Output == 0)
        {
            printf("Line &d has non-nummeric data\n", ReadPoints_Total+1);
            break;
        }
   
        if(TextFile_Output == 1)
        {
            printf("Incomplete breakpoint found at Point %d\n", ReadPoints_Total+1);
            break;
        }

        if(ReadPoints[ReadPoints_Total].BreakpointTime < ReadPoints_LastTime)
        {
            printf("Data error at Point %d: Time no inscreasing\n", ReadPoints_Total+1);
            break;
        }

        ReadPoints_LastTime = ReadPoints[ReadPoints_Total].BreakpointTime;

        if(++ReadPoints_Total == ArraySize)
        {
            BREAKPOINT_FORMAT* tmp;
            ArraySize += NPOINTS;
        
            tmp = (BREAKPOINT_FORMAT*) realloc(ReadPoints, sizeof(BREAKPOINT_FORMAT) *ArraySize);

            //Release all memory
            if(tmp == NULL)
            {
                ReadPoints_Total = 0;
                free(ReadPoints);
                ReadPoints = NULL;
                break;
            }

            ReadPoints = tmp;
        }
    }

    if(ReadPoints_Total)
    {
        *PointerSize = ReadPoints_Total;
    }    

    return ReadPoints;
}

//Scans input file for out of range values, returning "1" if passing the test
int breakpoint_inputfile_range_check(const BREAKPOINT_FORMAT *ReadPoints, double ValueMin, double ValueMax, unsigned long BreakpointFile_Size)
{
    int RangeCheck = 1;

    for(unsigned long i = 0; i < BreakpointFile_Size; i++)
    {
        if(ReadPoints[i].BreakpointValue < ValueMin || ReadPoints[i].BreakpointValue > ValueMax)
        {
            RangeCheck = 0;
            break;
        }
    }
    
    return RangeCheck;
}

//Find value for specified time, interpolating between breakpoints
double breakpoint_value_at_breakpoint_time(const BREAKPOINT_FORMAT *ReadPoints, unsigned long BreakpointFile_Size, double TimeSpan)
{
    unsigned long i;
    BREAKPOINT_FORMAT TimeLeft, TimeRight;
    double Fraction, Value, Width;

    //Scan until span of specified time is found
    for(i = 1; i < BreakpointFile_Size; i++)
    {
        if(TimeSpan <= ReadPoints[i].BreakpointTime)
        break;
    }

    if(i == BreakpointFile_Size)
    {
        return ReadPoints[i-1].BreakpointValue;
    }

    TimeLeft = ReadPoints[i-1];
    TimeRight = ReadPoints[i];

    //Check for instant jump, where two points have the same time
    Width = TimeRight.BreakpointTime - TimeLeft.BreakpointTime;

    if(Width == 0.0)
    {
        return TimeRight.BreakpointValue;
    }

    //Get value from this span of times using linear interpolation
    Fraction = (TimeSpan - TimeLeft.BreakpointTime) / Width;
    Value = TimeLeft.BreakpointValue + ((TimeRight.BreakpointValue - TimeLeft.BreakpointValue) * Fraction);

    return Value;
}