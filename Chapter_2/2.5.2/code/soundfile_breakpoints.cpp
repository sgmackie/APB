#include "stdio.h"
#include "stdlib.h"
#include "../../../misc/include/breakpoints.h"

#define NPOINTS (64)

//Extract points read from file
BREAKPOINT_FORMAT breakpoint_Point_MaxValue(const BREAKPOINT_FORMAT* ReadPoints, long ReadPoints_Total)
{
    int i;
    BREAKPOINT_FORMAT MaxPoint;

    //Start from first point
    MaxPoint.BreakpointTime = ReadPoints[0].BreakpointTime;
    MaxPoint.BreakpointValue = ReadPoints[0].BreakpointValue;

    for(i = 0; i < ReadPoints_Total; i++)
    {
        if(MaxPoint.BreakpointValue < ReadPoints[i].BreakpointValue)
        {
            MaxPoint.BreakpointValue = ReadPoints[i].BreakpointValue;
            MaxPoint.BreakpointTime = ReadPoints[i].BreakpointTime; 
        }
    }

    return MaxPoint;
}

//Function for reading input text file
BREAKPOINT_FORMAT *breakpoint_GetPoints(FILE *FilePointer, unsigned long *PointerSize)
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
            fprintf(stderr, "Error: Line &d has non-nummeric value\n", ReadPoints_Total+1);
            break;
        }
   
        if(TextFile_Output == 1)
        {
            fprintf(stderr, "Error: Incomplete breakpoint found at point %d\n", ReadPoints_Total+1);
            break;
        }

        if(ReadPoints[ReadPoints_Total].BreakpointTime < ReadPoints_LastTime)
        {
            fprintf(stderr, "Error: Time not increasing at point %d\n", ReadPoints_Total+1);
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
int breakpoint_Input_RangeCheck(const BREAKPOINT_FORMAT *ReadPoints, double ValueMin, double ValueMax, unsigned long BreakpointFile_Size)
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
double breakpoint_Point_ValueAtTime(const BREAKPOINT_FORMAT *ReadPoints, unsigned long BreakpointFile_Size, double TimeSpan)
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

BREAKPOINT_STREAM *breakpoint_Stream_New(FILE *InputFile, unsigned long SampleRate, unsigned long *BreakpointFile_Size)
{
    BREAKPOINT_STREAM *Stream;
    BREAKPOINT_FORMAT *ReadPoints;
    unsigned long NumPoints;

    if(SampleRate == 0)
    {
        fprintf(stderr, "Error: Sample rate cannot be 0\n");
        return NULL;
    }

    Stream = (BREAKPOINT_STREAM *) malloc(sizeof(BREAKPOINT_STREAM));

    if(Stream == NULL)
    {
        return NULL;
    }

    ReadPoints = breakpoint_GetPoints(InputFile, &NumPoints);

    if(ReadPoints == NULL)
    {
        free(Stream);
        return NULL;
    }

    if(Stream->NumPoints < 2)
    {
        fprintf(stderr, "Error: Breakpoint file must have at least 2 points\n");
        free(Stream);
        return NULL;
    }

    //Initialise Stream struct
    Stream->Points = ReadPoints;
    Stream->NumPoints = NumPoints;

    //Counters
    Stream->CurrentPosition = 0.0;
    Stream->CounterLeft = 0;
    Stream->CounterRight = 1;
    Stream->Increment = 1.0 / SampleRate;

    //Create first time span
    Stream->LeftPoint = Stream->Points[Stream->CounterLeft];
    Stream->RightPoint = Stream->Points[Stream->CounterRight];
    Stream->Width = Stream->RightPoint.BreakpointTime - Stream->LeftPoint.BreakpointTime;
    Stream->Height = Stream->RightPoint.BreakpointValue - Stream->LeftPoint.BreakpointValue;
    Stream->MorePoints = 1;

    if(BreakpointFile_Size)
    {
        *BreakpointFile_Size = Stream->NumPoints;
    }

    return Stream;
}

void breakpoint_Stream_Free(BREAKPOINT_STREAM *Stream)
{
    if(Stream && Stream->Points)
    {
        free(Stream->Points);
        Stream->Points = NULL;
    }
}

double breakpoint_Stream_ValueAtTime(BREAKPOINT_STREAM *Stream)
{
    double Value, Fraction;

    //Check if going beyond breakpoint data
    if(Stream->MorePoints == 0)
    {
        return Stream->RightPoint.BreakpointValue;
    }

    if(Stream->Width == 0.0)
    {
        Value = Stream->RightPoint.BreakpointValue;
    }

    else
    {
        //Get value using linear inerpolation
        Fraction = (Stream->CurrentPosition - Stream->LeftPoint.BreakpointTime) / Stream->Width;
        Value = Stream->LeftPoint.BreakpointValue + (Stream->Height * Fraction);
    }

    //Move position for next sample
    Stream->CurrentPosition += Stream->Increment;

    if(Stream->CurrentPosition > Stream->RightPoint.BreakpointTime)
    {
        Stream->CounterLeft++;
        Stream->CounterRight++;

        if(Stream->CounterRight < Stream->NumPoints)
        {
            Stream->LeftPoint = Stream->Points[Stream->CounterLeft];
            Stream->RightPoint = Stream->Points[Stream->CounterRight];
            Stream->Width = Stream->RightPoint.BreakpointTime - Stream->LeftPoint.BreakpointTime;
            Stream->Height = Stream->RightPoint.BreakpointValue - Stream->LeftPoint.BreakpointValue;
        }
    }

    else
    {
        Stream->MorePoints = 0;
    }

    return Value;
}