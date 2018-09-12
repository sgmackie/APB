#ifndef file_breakpoints_h
#define file_breakpoints_h

typedef struct BREAKPOINT_FORMAT 
{
    float64 BreakpointTime;
    float64 BreakpointValue;
} BREAKPOINT_FORMAT;

typedef struct BREAKPOINT_STREAM
{
    BREAKPOINT_FORMAT *Points;
    BREAKPOINT_FORMAT LeftPoint, RightPoint;
    uint64 NumPoints;
    float64 CurrentPosition, Increment, Width, Height;
    uint64 CounterLeft, CounterRight;
    int32 MorePoints;
} BREAKPOINT_STREAM;

//Prototypes
//Extract points read from file
BREAKPOINT_FORMAT breakpoint_Point_MaxValue(const BREAKPOINT_FORMAT *ReadPoints, int64 ReadPoints_Total);

//Function for reading input text file
BREAKPOINT_FORMAT *breakpoint_GetPoints(FILE* FilePointer, uint64 *PointerSize);

//Scans input file for out of range values, returning "1" if passing the test
int32 breakpoint_Input_RangeCheck(const BREAKPOINT_FORMAT *ReadPoints, float64 ValueMin, float64 ValueMax, uint64 BreakpointFile_Size);

//Find value for specified time, interpolating between breakpoints
float64 breakpoint_Point_ValueAtTime(const BREAKPOINT_FORMAT *ReadPoints, uint64 BreakpointFile_Size, float64 TimeSpan);

//Breakpoint stream functions
//Create stream with time spans
BREAKPOINT_STREAM *breakpoint_Stream_New(FILE *InputFile, uint64 SampleRate, uint64 *BreakpointFile_Size);

//Free stream structure
void breakpoint_Stream_Free(BREAKPOINT_STREAM *Stream);

//Find value for specified time, interpolating between breakpoints
float64 breakpoint_Stream_ValueAtTime(BREAKPOINT_STREAM *Stream);

#endif