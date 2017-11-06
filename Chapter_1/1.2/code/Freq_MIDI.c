//Calculates frequency of MIDI number
#include <stdio.h>
#include <math.h>

int main()
{
	//declare variables
	
	double semitone_ratio, c0, c5, frequency;
	
	//calculate required numbers
	
	semitone_ratio = pow(2, 1/12.0);
	//find middle C
	c5 = 220.0 * pow(semitone_ratio, 3);
	//MIDI note 0 is C, 5 octaves below Middle C
	c0 = c5 * pow(0.5, 5);
	
	//calculate frequency for given MIDI number
	int midinote = 73;
	frequency = c0 * pow(semitone_ratio, midinote);
	
	printf("MIDI Note %d has frequency %f", midinote, frequency);
	
	
	return 0;
	
}