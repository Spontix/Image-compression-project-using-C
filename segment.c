#define _CRT_SECURE_NO_WARNINGS

/***** INCLUDES *****/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include"segment.h"
#include "trees.h"
/***** DEFINES *****/


/***** TYPEDEFS *****/

/******************* Static Function Prototypes *******************/
static TreeNode* findSingleSegmentAux(grayImage* img, imgPos kernel, unsigned char threshold, int** flag, unsigned char value, unsigned int* totalSize);
static TreeNode** findNeighbors(grayImage* img, imgPos kernel, unsigned char threshold, int** flag, unsigned char value, unsigned int *size1);
static int** flagTable(int col, int rows);


static imgPos* findMinValue(grayImage* img, unsigned int** flag);
static Segment* findSegment(grayImage* img, imgPos kernel, unsigned char threshold, unsigned int** flag);
static imgPosCell* convertTree(int size, Segment* tr);
static void convertTreeToArray(TreeNode* root, imgPos* str_pos, int* indx);
int myCompByPos(imgPos* pos1, imgPos* pos2);
static imgPosCell* ConverArrayToDoublyLinkedList(int size, imgPos* str_pos);
static BOOL isAllFlagsOn(grayImage* img, unsigned int** flag);
static void myRealloc(imgPosCell ***segments, unsigned int* size);
static int myCompBySize(imgPosCell** pos1, imgPosCell** pos2);
static void RemoveFromHead(imgPosCell*** segments, int size);
static imgPos* findFirstPosOff(int** flag, grayImage* img);


static grayImage* allocateIMG(unsigned int rows, unsigned int cols);


static void incorrectFileExit(void);
static void fileOpenCheck(FILE* fname);
static void getNumber(FILE* fname, char*num[]);
static void scanImageValues(FILE* pgm, grayImage* img);
static void decompreesNum(int numBits, FILE* fPGM, int* writeIndex, unsigned char* origPix, unsigned short* flag, int cols, unsigned char* mask, unsigned char buffer);
/******************* Function Implementation *******************/



/*first qustion*/
Segment* findSingleSegment(grayImage* img, imgPos kernel, unsigned char threshold)
/*returns a segment of the given kernel acording to the threshold*/
{
	Segment* tr = (Segment*)malloc(sizeof(Segment));
	memmoryCheck(tr);
	TreeNode *node;
	int** flag = flagTable(img->col, img->rows);/*indicates if the current pixel is in a segment*/
	unsigned char value;
	unsigned int i = kernel[0], j = kernel[1], totalSize = 1;
	flag[i][j] = 1;
	value = img->pixels[i][j];/*stores the vallue ocording to wich it will be determind if a pixel is part f the segment or not*/
	node = findSingleSegmentAux(img, kernel, threshold, flag, value, &totalSize);
	tr->root = node;
	tr->size = totalSize;
#ifdef DEBUG
	printf("total size= %d", totalSize);
#endif // DEBUG
	return tr;
}

static TreeNode** findNeighbors(grayImage* img, imgPos kernel, unsigned char threshold, int** flag, unsigned char value, unsigned int *size1)
/*the function bukds the neighbors arry and returns it, if there are no nighbors returns NULL*/
{
	TreeNode** similar = (TreeNode**)malloc(sizeof(TreeNode*) * 8);/*the nighbors array*/
	memmoryCheck(similar);
	unsigned int size = 0;
	short i, j;
	unsigned char min_value , max_value = value + threshold;
	if (threshold > value)
		min_value = 0;
	else min_value = value - threshold;
	imgPos temp;/*temporary pos*/
	for (i = kernel[0] - 1; i <= (kernel[0] + 1); i++)/*go over the rows*/
	{
		if (i == -1)
			i++;
		temp[0] = i;
		for (j = kernel[1] - 1; j <= kernel[1] + 1; j++)/*go over the cols*/
		{
			if (j == -1)
				j++;
			temp[1] = j;
			if ((flag[i][j] == 0) && ((img->pixels[i][j] >= min_value) && (img->pixels[i][j] <= max_value)))/*if the flag is off, adds the pixel to the segment and turns the flag on*/
				/*if the value is in range add to neighbrs*/
			{
				flag[i][j] = 1;
				similar[size] = allocateTreeNode(temp);
				size++;
			}
			if (j == img->col - 1)
				break;
		}
		if (i == img->rows - 1)/*i==7*/
			break;/* exit the loop*/
	}
	*size1 = size;
	if (size == 0) {
		free(similar);
		return NULL;
	}
	similar = (TreeNode**)realloc(similar, sizeof(TreeNode*) * (size + 1));/*realucate according to the actual number of neighbors and add one more place fot NULL pointer*/
	memmoryCheck(similar);
	similar[size] = NULL;
	return similar;
}

static TreeNode* findSingleSegmentAux(grayImage* img, imgPos kernel, unsigned char threshold, int** flag, unsigned char value, unsigned int* totalSize)
{/*recursevly finds the segment by finding the neighbors of evry pixel in the segment*/
	TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
	memmoryCheck(node);
	unsigned int i, size;
	node->position[0] = kernel[0];/*the position of the current root is the guven kernel*/
	node->position[1] = kernel[1];
	node->similar_neighbors = findNeighbors(img, kernel, threshold, flag, value, &size);/*the function returns neigbors array*/
	if (size == 0)/*if there weren't found nighbors*/
	{
		node->similar_neighbors = NULL;
		return node;
	}
#ifdef DEBUG3
	printf("pos=%d %d\n   ", node->position[0], node->position[1]);
	if (size == 0)
		printf("no neighbors/n");
	else for (i = 0; i < size; i++)
		printf("%d %d  ", node->similar_neighbors[i]->position[0], node->similar_neighbors[i]->position[1]);
	putchar('\n');
#endif // DEBUG3

	*totalSize += size;
	for (i = 0; i < size; i++)
		node->similar_neighbors[i] = findSingleSegmentAux(img, node->similar_neighbors[i]->position, threshold, flag, value, totalSize);
	return node;
}

static int** flagTable(int col, int rows)
{/*allocates new table and nullifies all flags to  0*/
	int** table = (int**)malloc(sizeof(int*)*rows);
	memmoryCheck(table);
	int i, j;
	for (i = 0; i < rows; i++) {
		table[i] = (int*)malloc(sizeof(int)*col);
		memmoryCheck(table[i]);
	}
	for (i = 0; i < rows; i++)
		for (j = 0; j < col; j++)
			table[i][j] = 0;
	return table;
}


/*second quastion*/
unsigned int findAllSegments(grayImage *img, unsigned char threshold, imgPosCell ***segments)
{/*the function returns the number of segments that were found*/
	unsigned int** flag = flagTable(img->col, img->rows);
	imgPos* minPos;/*the posotion of he current minimal value*/
	Segment* tr = (Segment*)malloc(sizeof(Segment));
	memmoryCheck(tr);
	unsigned int logicSize = 0, actualSize = 2;/*in order to know when to reallocate the array*/
	*segments = (imgPosCell**)malloc(sizeof(imgPosCell*) * actualSize);
	memmoryCheck(*segments);
	while (!isAllFlagsOn(img, flag))/*if not all the pixels in a segment*/
	{
		minPos = findMinValue(img, flag);/*return the position of the minimal value*/
		tr = findSegment(img, *minPos, threshold, flag);/*finds the segment with minPos as kernel*/
		if (logicSize == actualSize)/*if all the array is full, doublle its size*/
			myRealloc(segments, &actualSize);
		(*segments)[logicSize] = convertTree(tr->size, tr);/* converts a single tree into a doubly linkes list of single segment*/
		logicSize++;
	}
	*segments = (imgPosCell**)realloc(*segments, sizeof(imgPosCell*) * (logicSize));
	memmoryCheck(*segments);
	qsort(*segments, logicSize, sizeof(imgPosCell*), myCompBySize);/*sorts the array by the segments sizes.
																the sizes stored in the first list node in the first place of the posotion*/
#ifdef DEBUG2
	unsigned int i;
	printf("the sizes are: ");
	for (i = 0; i < logicSize; i++)
		printf("%d   ", (*segments)[i]->position[0]);
	putchar('\n');
#endif // DEBUG2
	RemoveFromHead(segments, logicSize);/*after the list is sorted, remove the nodes with the sizes of the segment*/
	return logicSize;
}

static imgPos* findMinValue(grayImage* img, unsigned int** flag)/*מחזירים פוינטר כי אי אפשר להחזיר מערך*/
{/*finds the minimal value in the image
 return  it's position*/
	imgPos* res;
	res = (imgPos*)malloc(sizeof(imgPos));
	memmoryCheck(res);
	unsigned int i, j;
	res = findFirstPosOff(flag, img);/*finds the first position with its flag off*/
	i = (*res)[0];
	j = (*res)[1];
	unsigned char min = img->pixels[i][j];/*storing the value of the first location with the flags off as current minimal value*/
	for (i = 0; i < img->rows; i++)/*runs over the img and searching for the fist minimal value with the flag off*/
		for (j = 0; j < img->col; j++)
		{
			if ((flag[i][j] == 0) && (min > img->pixels[i][j]))
			{
				min = img->pixels[i][j];/*remembers the value of the min*/
				(*res)[0] = i;/*stores the position of the min*/
				(*res)[1] = j;
			}
		}
#ifdef DEBUG4
	printf("row=%d, col=%d\n", (*res)[0], (*res)[1]);
#endif // DEBUG4
	flag[(*res)[0]][(*res)[1]] = 1;/*turns the flag on for the current minimal value*/
	return res;/*return the mins position*/
}

static imgPos* findFirstPosOff(int** flag, grayImage* img)
{/*returns the first position with the flag off */
	unsigned int i, j;
	imgPos* res = (imgPos*)malloc(sizeof(imgPos));
	memmoryCheck(res);
	for (i = 0; i < img->rows; i++)
		for (j = 0; j < img->col; j++)
			if (flag[i][j] == 0)
			{
				(*res)[0] = i;
				(*res)[1] = j;
				return res;
			}
	return NULL;/*if all the flags are on thee function will not be called*/
}

static BOOL isAllFlagsOn(grayImage* img, unsigned int** flag)
{/*if al the flags on returns true else return false*/
	unsigned int i, j;
	for (i = 0; i < img->rows; i++)
		for (j = 0; j < img->col; j++)
		{
			if (flag[i][j] == 0)
			{
#ifdef DEBUG
				printf("flagson=FALSE\n");
#endif // DEBUG
				return FALSE;/*found a flag that is off*/
			}
		}
#ifdef DEBUG
	printf("flagson=TRUE\n");
#endif // DEBUG
	return TRUE;
}

static Segment* findSegment(grayImage* img, imgPos kernel, unsigned char threshold, unsigned int** flag)
/*returns a segment of the given kernel acording to the threshold*/
{
	Segment* tr = (Segment*)malloc(sizeof(Segment));
	memmoryCheck(tr);
	TreeNode *node;
	unsigned char value;
	unsigned int i = kernel[0], j = kernel[1], totalSize = 1;
	flag[i][j] = 1;
	value = img->pixels[i][j];/*stores the vallue ocording to wich it will be determind if a pixel is part f the segment or not*/
	node = findSingleSegmentAux(img, kernel, threshold, flag, value, &totalSize);
	tr->root = node;
	tr->size = totalSize;
#ifdef DEBUG2
	printf("total size= %d\n", totalSize);
#endif // DEBUG2
	return tr;
}

static imgPosCell* convertTree(int size, Segment* tr)
{
	int i;
	int  indx=0;
	imgPos* str_pos = (imgPos*)malloc(sizeof(imgPos)*size);/*daynamic araay of arrays*/
	memmoryCheck(str_pos);
	imgPosCell* res = NULL, *curr;
	convertTreeToArray(tr->root, str_pos, &indx);/*str_pos is an returned array of the segment values*/
#ifdef DEBUG2
	printf("the array is: ");
	for (i = 0; i < size; i++)
		printf("[%d],[%d]   ", str_pos[i][0], str_pos[i][1]);
	putchar('\n');
#endif // DEBUG
	qsort(str_pos, size, sizeof(imgPos), myCompByPos);/*sorts ate array by positions*/
#ifdef DEBUG2
	printf("the array is: ");
	for (i = 0; i < size; i++)
		printf("[%d],[%d]   ", str_pos[i][0], str_pos[i][1]);
	putchar('\n');
#endif // DEBUG
	res = ConverArrayToDoublyLinkedList(tr->size, str_pos);/*converts the array into a dobly linked list*/
	/*add to the begining of the linked list a position. The first place of the position helds the size of the array, and the second is 0.*/
	curr = (imgPosCell*)malloc(sizeof(imgPosCell));
	memmoryCheck(curr);
	curr->position[0] = size;
	curr->position[1] = 0;
	curr->next = res;
	curr->prev = NULL;
	res->prev = curr;
	res = curr;
	curr = NULL;
#ifdef DEBUG2
	curr = res;
	printf("the linked list is: ");
	while (curr) {
		printf("%d %d    ", curr->position[0], curr->position[1]);
		curr = curr->next;
	}
	printf("\n");
#endif // DEBUG2
	free(curr);/*the is no longer use for the curr pointer*/
	return res;
}

static void convertTreeToArray(TreeNode* root, imgPos* str_pos, int* indx)
{/*recursevly converts the tree in to an array.*/
	unsigned int i;
	str_pos[*indx][0] = root->position[0];
	str_pos[*indx][1] = root->position[1];
	(*indx)++;
	if (!(root->similar_neighbors))/*if there are no neighbors*/
		return;
	for (i = 0; root->similar_neighbors[i] != NULL; i++)
	{
		convertTreeToArray(root->similar_neighbors[i], str_pos, indx);
	}
}

int myCompByPos(imgPos* pos1, imgPos* pos2)
{/*compers by position*/
	if ((*pos1)[0] == (*pos2)[0])/*if the positions are in the same row cemper the colomns*/
		return (*pos1)[1] - (*pos2)[1];
	return (*pos1)[0] - (*pos2)[0];/*is not in the same row*/
}

static imgPosCell* ConverArrayToDoublyLinkedList(int size, imgPos* str_pos)
{/*converts the array in to a doublly linked list*/
	imgPosCell* res = (imgPosCell*)malloc(sizeof(imgPosCell));
	memmoryCheck(res);
	imgPosCell* curr, *prev = NULL;
	int i;
	res->position[0] = str_pos[0][0];/* יצרנו אתהתא הראשון ברשימה המקושרת*/
	res->position[1] = str_pos[0][1];
	res->prev = res->next = NULL;
	curr = res;
	for (i = 1; i < size; i++)
	{/*while there are values in the array, add it to the end of the linked list*/
		prev = curr;
		curr->next = (imgPosCell*)malloc(sizeof(imgPosCell));
		memmoryCheck(curr->next);
		curr->next->position[0] = str_pos[i][0];
		curr->next->position[1] = str_pos[i][1];
		curr = curr->next;
		curr->prev = prev;
		curr->next = NULL;
	}
#ifdef DEBUG2
	printf("\nthe size in the list is: %d\n", size);
#endif // DEBUG2

	return res;
}

static void myRealloc(imgPosCell ***segments, unsigned int* size)/**/
{/*reallocates the arrays to the double of its size*/
	*size *= 2;
	*segments = (imgPosCell**)realloc(*segments, sizeof(imgPosCell*) * (*size));
	memmoryCheck(*segments);
}

static int myCompBySize(imgPosCell** pos1, imgPosCell** pos2)
{/*compers by the size that is stored in the first place in the position */
	return (*pos2)->position[0] - (*pos1)->position[0];
}

static void RemoveFromHead(imgPosCell*** segments, int size)
{/*removes a list node from the head of the list*/
	int i;
	for (i = 0; i < size; i++)
	{
		(*segments)[i] = (*segments)[i]->next;
		(*segments)[i]->prev->next = NULL;
		free((*segments)[i]->prev);
		(*segments)[i]->prev = NULL;
	}
}


/*thired quastion*/
grayImage* colorSegments(grayImage* img, imgPosCell** segments, unsigned int size)
{
	/*   int value=i*(255/(size-1))   */
	grayImage* res= allocateIMG(img->rows, img->col);
	unsigned int i, value, row, col;
	imgPosCell* curr;
	for (i = 0; i < size; i++)/*go over the segments array*/
	{
		curr = segments[i];/*pointer to the first node in the list*/
		value = i * (255 / (size - 1));/*calculate the value of the segment*/
		while (curr)/*while it is the same segment*/
		{
			row = curr->position[0];/*get the position*/
			col = curr->position[1];
			res->pixels[row][col] = value;/*insert the gray value to the position in the img*/
			curr = curr->next;
		}
	}
	return res;
}

static grayImage* allocateIMG(unsigned int rows, unsigned int col)
{/*allocate the pixels table*/
	/*the func returns allocated img with the rows and cols*/
	unsigned int i;
	grayImage *img = (grayImage*)malloc(sizeof(grayImage));
	memmoryCheck(img);
	img->pixels = (unsigned char**)malloc(sizeof(unsigned char*)*rows);
	memmoryCheck(img->pixels);
	for (i = 0; i < rows; i++)/*go over the rows*/
	{
		img->pixels[i] = (unsigned char*)malloc(sizeof(unsigned char)* col);
		memmoryCheck(img->pixels[i]);
	}
	img->col = col;
	img->rows = rows;
	return img;
}

/*fourth quastion*/
grayImage* readPGM(char* fname)
{
	FILE* pgm = fopen(fname, "r");/*open the PGM file*/
	fileOpenCheck(pgm);
	unsigned int rows, cols, max_value;
	grayImage* img;
	char p = getc(pgm), spam, type, *number = (char*)malloc(sizeof(char));
	memmoryCheck(number);
	if ((p != 'p')&&(p!='P')) /*if the first letter isn't p. PGM file begins with P2*/
		incorrectFileExit();
	type = getc(pgm);
	if (type != '2')
		incorrectFileExit();
	fscanf(pgm, "%c", &spam);/*reads the enter at the end of the line*/
	fscanf(pgm,"%c",&p);/*reads the next letter to see if it is a comment*/
	fscanf(pgm, "%c", &spam);
	while (p == '#') {/*if it's a comment line, go to the next line*/
		while (spam != '\n')/*while not end of the line*/
			fscanf(pgm, "%c", &spam);
		p = getc(pgm);
	}
	number[0] = p;
	getNumber(pgm, &number);/*scan the rest of the number*/
	cols = atoi(number);/*conver the number to integer*/
	fscanf(pgm, " %d %d", &rows, &max_value);
	img = allocateIMG(rows, cols);
	scanImageValues(pgm, img);
	fclose(pgm);
	return img;
}

static void getNumber(FILE* fname, char*num[])
{/*the first place int the array holds a digit*/
	/*the functions reads the rest of the number */
	char c;
	int i = 1, size = 1;
	c = getc(fname);
	while (c != ' ')/*the number ends with space*/
	{
		if (i == size)
		{
			size = size * 2 + 1;
			(*num) = (char*)realloc(*num, (size) * sizeof(char));
			memmoryCheck(num);
		}
		(*num)[i] = c;
		i++;
		c = getc(fname);
	}
	(*num) = (char*)realloc(*num, (i+1) * sizeof(char));
	(*num)[i] = '\0';
	memmoryCheck(*num);
}

static void incorrectFileExit(void)
{
	printf("incorrect file type");
	exit(INCORRECT_FILE);
}

static void fileOpenCheck(FILE* fname)
{/*if the file ddn't open, print massage and exit the program*/
	if (!fname)
	{
		fprintf(stderr,"can not open file");
		exit(NOT_OPEN);
	}
}

static void scanImageValues(FILE* pgm, grayImage* img)
{
	unsigned int i, j,d;
	for (i = 0; i < img->rows; i++)
		for (j = 0; j < img->col; j++)
		{
			fscanf(pgm, "%d", &d);
			img->pixels[i][j] = d;
		}
}

/*fifth quastion*/
/*typedef struct _grayImage {
	unsigned short rows, col;
	unsigned char** pixels;
}grayImage;*/
void saveCompareseed(char* fname, grayImage* image, unsigned char reduced_gray_levels)
{
	int shift, i, j, x, numBits, count = 0;
	unsigned char squizPix = 0, mask, origPix = 0;
	FILE* f = fopen(fname, "wb");
	fileOpenCheck(f);
	fwrite(&image->rows, sizeof(unsigned short), 1, f);/*writes the numbers of rows*/
	fwrite(&image->col, sizeof(unsigned short), 1, f);/*writes the number of colomns*/
	fwrite(&reduced_gray_levels, sizeof(unsigned char), 1, f);/*writes the new maximum gray level*/
	numBits = log2(reduced_gray_levels);	/*calculates the shits value*/
	shift = 8 - numBits;
	for (i = 0; i < image->rows; i++){/*sqeezes each pixsel to the new gray level*/
		for (j = 0; j < image->col; j++){
			mask = 0x80 >> shift;
			origPix = image->pixels[i][j] >> shift;
			for (x = 0; x < numBits; x++){/*goes over the rest of the used bites*/
				squizPix <<= 1;
				if (mask & origPix)
					squizPix |= 1;
				mask >>= 1;
				count++;
				if (count==8){/*when count==8 write the cuurent byte to the file*/
					fwrite(&squizPix, sizeof(unsigned char), 1, f);
					count = 0;
					squizPix = 0;}
			}
		}
	}
	if (count != 0){/*if M*N does not have round devision in numOfBites*/
		squizPix <<= 8 - count;/*squeezes the last byte*/
		fwrite(&squizPix, sizeof(unsigned char), 1, f);/*writes the last bite*/
	}
	fclose(f);/*closes the file*/
}

/*sixth quastion*/
void convertCompressedImageToPGM(char* compressed_file_name, char* pgm_file_name)
{
	int numBits, readBits = 0;
	unsigned short flag = 0;
	unsigned short rows, cols;
	unsigned char reduceV, buffer, mask, origPix = 0;
	int len, writeIndex=0;
	FILE* fPGM = fopen(pgm_file_name, "w");
	fileOpenCheck(fPGM);
	FILE* fCFN = fopen(compressed_file_name, "rb");
	if (!fCFN) fclose(fPGM);/*if the second file didnt open, close the first file*/
	fileOpenCheck(fCFN);
	fseek(fCFN, 0, SEEK_END);
	len = ftell(fCFN);
	fseek(fCFN, 0, SEEK_SET);
	fread(&rows, sizeof(unsigned short), 1, fCFN);
	fread(&cols, sizeof(unsigned short), 1, fCFN);
	fread(&reduceV, sizeof(unsigned char), 1, fCFN);
	fprintf(fPGM, "%c%c%c%d %d\n%d", 'p', '2', '\n',rows,cols,255);
	numBits = log2(reduceV);
	fputc('\n', fPGM);
	//shift to right "shift" times
	while (ftell(fCFN) < len){
		if (readBits == 8 || readBits == 0)
			fread(&buffer, sizeof(unsigned char), 1, fCFN);
		mask = 0x80;
		for (readBits = 0; readBits < 8; readBits++)
			decompreesNum(numBits, fPGM, &writeIndex, &origPix, &flag, cols, &mask, buffer);
	}
	fclose(fCFN);
	fclose(fPGM);
}

static void decompreesNum(int numBits, FILE* fPGM,int* writeIndex,unsigned char* origPix,unsigned short* flag, int cols,unsigned char* mask,unsigned char buffer )
{
	if ((*writeIndex) == numBits)
	{
		(*origPix) <<= 8 - numBits;
		fprintf(fPGM, "%-4d", (int)(*origPix));
		(*writeIndex) = (*origPix) = 0;
		(*flag)++;
	}
	if ((*flag) == cols) 
	{
		fputc('\n', fPGM);
		(*flag) = 0;
	}
	(*origPix) <<= 1;
	if ((*mask) & buffer)
		(*origPix) |= 1;
	(*mask) >>= 1;
	(*writeIndex)++; 
}