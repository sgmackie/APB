//Accepts command line arguments to convert MIDI notes

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[])
{
	double c5, c0, semitone_ratio, frequency;
	int midinote;
	
	semitone_ratio = pow(2, 1.0/12);
	c5 = 220.0 * pow(semitone_ratio, 3);
	c0 = c5 * pow(0.5, 5);
	
	if(argc != 2)
		{
			printf("%s: Converts MIDI note to frequency\n", argv[0]);
			printf("Usage: %s MIDI note\n", argv[0]);
			
			return 1;
		}
		
	midinote = atoi(argv[1]);
	
	if(midinote < 0, midinote > 127)
		{
			printf("%s is outside MIDI range", argv[1]);
			
			return 1;
		}		

	frequency = c0 * pow(semitone_ratio, midinote);
	printf("Frequency of MIDI note %d = %f\n", midinote, frequency);
		
	return 0;
}