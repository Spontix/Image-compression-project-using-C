#define _CRT_SECURE_NO_WARNINGS

/***** INCLUDES *****/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "defines.h"
/***** DEFINES *****/


/***** TYPEDEFS *****/

/******************* Static Function Prototypes *******************/



/******************* Function Implementation *******************/
void memmoryCheck(void *arr)
{
	if (arr == NULL)
	{
		fprintf(stderr,"memory alocation error");
		exit(MEMORY_ALLOCATION_ERROR);
	}
}