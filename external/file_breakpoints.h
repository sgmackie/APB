#ifndef file_breakpoints_h
#define file_breakpoints_h

#include "stdio.h"
#include "stdlib.h"

typedef struct BreakpointFormat 
{
    double BreakpointTime;
    double BreakpointValue;
}   BREAKPOINT_FORMAT;

typedef struct BreakpointStream
{
    BREAKPOINT_FORMAT *Points;
    BREAKPOINT_FORMAT LeftPoint, RightPoint;
    unsigned long NumPoints;
    double CurrentPosition, Increment, Width, Height;
    unsigned long CounterLeft, CounterRight;
    int MorePoints;
}   BREAKPOINT_STREAM;

//Prototypes
//Extract points read from file
BREAKPOINT_FORMAT breakpoint_Point_MaxValue(const BREAKPOINT_FORMAT* ReadPoints, long ReadPoints_Total);

//Function for reading input text file
BREAKPOINT_FORMAT *breakpoint_GetPoints(FILE* FilePointer, unsigned long* PointerSize);

//Scans input file for out of range values, returning "1" if passing the test
int breakpoint_Input_RangeCheck(const BREAKPOINT_FORMAT *ReadPoints, double ValueMin, double ValueMax, unsigned long BreakpointFile_Size);

//Find value for specified time, interpolating between breakpoints
double breakpoint_Point_ValueAtTime(const BREAKPOINT_FORMAT *ReadPoints, unsigned long BreakpointFile_Size, double TimeSpan);

//Breakpoint stream functions
//Create stream with time spans
BREAKPOINT_STREAM *breakpoint_Stream_New(FILE *InputFile, unsigned long SampleRate, unsigned long *BreakpointFile_Size);

//Free stream structure
void breakpoint_Stream_Free(BREAKPOINT_STREAM *Stream);

//Find value for specified time, interpolating between breakpoints
double breakpoint_Stream_ValueAtTime(BREAKPOINT_STREAM *Stream);

#endif