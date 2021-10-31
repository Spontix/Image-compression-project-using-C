#define _CRT_SECURE_NO_WARNINGS_

/***** INCLUDES *****/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "trees.h"
#include "defines.h"

/***** DEFINES *****/

/***** TYPEDEFS *****/

/******************* Static Function Prototypes *******************/


/******************* Function Implementation *******************/
TreeNode* allocateTreeNode(imgPos data)
{/*allocate single node*/
	TreeNode *node = (TreeNode*)malloc(sizeof(TreeNode));
	memmoryCheck(node);
	node->position[0] = data[0];
	node->position[1] = data[1];
	node->similar_neighbors = NULL;
	return node;
}