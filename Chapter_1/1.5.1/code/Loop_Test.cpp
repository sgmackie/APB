//While loop test

#include <stdio.h>

int main()
{
	int up = 0;
	int down = 9;
	
	while(up < 10)
			printf("%d ", up++);
		
	printf("\n");
	
	while(down >= 0)
		{
			printf("%d ", down);
			down--;
		}
	
	return 0;
}