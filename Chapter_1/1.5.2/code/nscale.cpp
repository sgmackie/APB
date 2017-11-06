//Prints frequency of MIDI note from a given scale

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[])
{
	//Declare  variables
	
	int notes, midinote, i, counter;
	double frequency, ratio, c0, c5;
	double intervals[24];
	int test;
	
	//Argument number check
	
	if(argc != 3)
	{
		printf("Usage: nscale notes midinote\n");
		return 1;
	}
	
	notes = atoi(argv[1]);
	
	//Check 1st argument
	
	if(notes < 1)
	{
		printf("Error: notes must be positive\n");
		return 1;
	}
		
	if(notes > 24)
	{
		printf("Error: notes cannot be bigger than 24\n");
		return 1;
	}
	
	midinote = atoi(argv[2]);
	
	//Check 2nd argument
	
	if(midinote < 0)
	{
		printf("Error: cannot have negative MIDI notes\n");
		return 1;
	}
	
	if(midinote > 127)
	{
		printf("Error: maximum MIDI note is 127\n");
		return 1;
	}
	
	//Calculation
	
	ratio = pow(2.0, 1.0/12.0);
	c5 = 220.0 * pow(ratio, 3);
	c0 = c5 * pow(0.5, 5);
	frequency = c0 * pow(ratio, midinote);
	
	ratio = pow(2.0,1.0/ notes);
	
	for(i = 0; i < notes; i++)
	{
		intervals[i] = frequency;
		frequency *= ratio;
	}
	
	//Read array and printf
	
	for(i = 0, counter = 0; i < notes, counter < notes; i++, counter++)
	{
		printf("%d:%f\n", counter, intervals[i]);
	}
	
	return 0;
}