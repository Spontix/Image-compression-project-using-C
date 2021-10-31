#ifndef __DEFINESSS___
#define __DEFINESSS___

/***** INCLUDES *****/

/***** DEFINES *****/
#define MEMORY_ALLOCATION_ERROR -1
#define INCORRECT_FILE -2
#define NOT_OPEN -3
#define BOOL int
#define TRUE 1
#define FALSE 0


//#define DEBUG
//#define DEBUG1
//#define DEBUG2
//#define DEBUG3
//#define DEBUG4

/***** TYPEDEFS *****/

typedef unsigned short imgPos[2]; /*{row, col}*/

typedef struct _grayImage {
	unsigned short rows, col;
	unsigned char **pixels;
}grayImage;


typedef struct _treeNode {
	imgPos position;
	struct _treeNode **similar_neighbors; /*kids array   *segment[8]   */
}TreeNode;

typedef struct _segment {/*the tree*/
	TreeNode *root;
	unsigned int size; /*the total number of tree nodes*/
}Segment;

typedef struct _imgPosCell {
	imgPos position;
	struct _imgPosCell *next, *prev;
}imgPosCell;

/******************* Function Prototypes *******************/

void memmoryCheck(void *arr);

#endif