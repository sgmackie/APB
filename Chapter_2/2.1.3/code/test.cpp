#include <stdio.h>
#include <stdlib.h>
#include "portsf.h"

int main()
{
    int size = 15;
    int *ptr;
    ptr = &size; 

    printf("Address of %x contains %d", ptr, *ptr);

    return 0;
}