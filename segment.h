#ifndef __SIGMENT___
#define __SIGMENT___


/***** INCLUDES *****/
#include "defines.h"

/***** DEFINES *****/

/***** TYPEDEFS *****/


/******************* Function Prototypes *******************/

Segment* findSingleSegment(grayImage* img, imgPos kernel, unsigned char threshold);
unsigned int findAllSegments(grayImage *img, unsigned char threshold, imgPosCell ***segments);
grayImage* colorSegments(grayImage* img, imgPosCell** segments, unsigned int size);
grayImage* readPGM(char* fname);
void saveCompareseed(char* fname, grayImage* image, unsigned char reduced_gray_levels);
void convertCompressedImageToPGM(char* compressed_file_name, char* pgm_file_name);

#endif