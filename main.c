#define _CRT_SECURE_NO_WARNINGS

#include "defines.h"
#include "segment.h"
#include "trees.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ROWS 5
#define COLS 3

void main1();
void main2();
void main3();
void main4();
void main5();
void main6();


void main()
{
	int num;
	printf("enter the number of quastion to run: ");
	scanf("%d", &num);
	switch (num)
	{
	case 1:main1();
		break;
	case 2: main2();
		break;
	case 3:main3();
		break;
	case 4: main4();
		break;
	case 5: main5();
		break;
	case 6: main6();
		break;
	default: printf("quastion does not exist.");
		exit(-1);
	}
}

void main1()
{
	grayImage img;
	img.col = 5;
	img.rows = 5;
	img.pixels = (unsigned char**)malloc(sizeof(char*) * 5);
	memmoryCheck(img.pixels);
	int i, j;
	imgPos pos;
	pos[0] = 2;
	pos[1] = 3;
	for (i = 0; i < 5; i++) {
		img.pixels[i] = (char*)malloc(sizeof(char) * 5);
		memmoryCheck(img.pixels[i]);
	}
#ifdef DEBUG
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 5; j++)
		{
			img.pixels[i][j] = rand() % 255;
			printf("%4d", img.pixels[i][j]);
		}
		putchar('\n');
	}
#endif // DEBUG
	Segment* seg = findSingleSegment(&img, pos, 86);

}

void main2()
{
	grayImage img;
	imgPosCell** segments;
	img.col = 5;
	img.rows = 5;
	img.pixels = (unsigned char**)malloc(sizeof(unsigned char*) * 5);
	memmoryCheck(img.pixels);
	int i, j;
	for (i = 0; i < 5; i++) {
		img.pixels[i] = (unsigned char*)malloc(sizeof(unsigned char) * 5);
		memmoryCheck(img.pixels[i]);
	}
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 5; j++)
		{
			img.pixels[i][j] = rand() % 255;
			printf("%4d", img.pixels[i][j]);
		}
		putchar('\n');
	}
	unsigned int num_of_segments = findAllSegments(&img, 30, &segments);
#ifdef DEBUG
	printf("the number of segments is: %d\n", num_of_segments);
#endif // DEBUG

}

void main3()
{
	imgPosCell** segments;
	unsigned int row = 7, cols = 6, i, j;
	grayImage img, *res;
	res = (grayImage*)malloc(sizeof(grayImage));
	memmoryCheck(res);
	img.col = cols;
	img.rows = row;
	img.pixels = (unsigned char**)malloc(sizeof(unsigned char*) * row);
	memmoryCheck(img.pixels);
	res->pixels = (unsigned char**)malloc(sizeof(unsigned char*) * row);
	memmoryCheck(res->pixels);
	for (i = 0; i < row; i++) {
		img.pixels[i] = (char*)malloc(sizeof(char) * cols);
		memmoryCheck(img.pixels[i]);
	}
	for (i = 0; i < row; i++)/*scan the img*/
		for (j = 0; j < cols; j++)
		{
			scanf("%d", &img.pixels[i][j]);
		}
	unsigned int num_of_segments = findAllSegments(&img, 0, &segments);
	res->col = cols;
	res->rows = row;
	res = colorSegments(res, segments, num_of_segments);
#ifdef DEBUG3
	printf("num of segments: %d\n", num_of_segments);
	for (i = 0; i < row; i++)/*scan the img*/
	{
		for (j = 0; j < cols; j++)
			printf("%4d", res->pixels[i][j]);
		putchar('\n');
	}
#endif // DEBUG3

}

void main4()
{
	char fname[9] = "hi.pgm";
	grayImage* img = readPGM(fname);

#ifdef DEBUG4
	unsigned int i, j, d;
	for (i = 0; i < img->rows; i++)
	{
		for (j = 0; j < img->col; j++)
		{
			d = img->pixels[i][j];
			printf("%4d", d);
		}
		putchar('\n');
	}
#endif // DEBUG4

}

void main5()
{
	grayImage img;
	img.col = COLS;
	img.rows = ROWS;
	img.pixels = (unsigned char**)malloc(sizeof(char*) * ROWS);
	memmoryCheck(img.pixels);
	int i, j;
	imgPos pos;
	pos[0] = 2;
	pos[1] = 3;
	for (i = 0; i < ROWS; i++) {
		img.pixels[i] = (char*)malloc(sizeof(char) * COLS);
		memmoryCheck(img.pixels[i]);
	}
#ifdef DEBUG
	for (i = 0; i < ROWS; i++)
	{
		for (j = 0; j < COLS; j++)
		{
			img.pixels[i][j] = rand() % 255;
			printf("%4d", img.pixels[i][j]);
		}
		putchar('\n');
	}
#endif // DEBUG
	saveCompareseed("blabla.bin", &img, 8);


}

void main6()
{
	grayImage img;
	img.col = COLS;
	img.rows = ROWS;
	img.pixels = (unsigned char**)malloc(sizeof(char*) * ROWS);
	memmoryCheck(img.pixels);
	int i, j;
	imgPos pos;
	pos[0] = 2;
	pos[1] = 3;
	for (i = 0; i < ROWS; i++) {
		img.pixels[i] = (char*)malloc(sizeof(char) * COLS);
		memmoryCheck(img.pixels[i]);
	}
#ifdef DEBUG
	for (i = 0; i < ROWS; i++)
	{
		for (j = 0; j < COLS; j++)
		{
			img.pixels[i][j] = rand() % 255;
			printf("%4d", img.pixels[i][j]);
		}
		putchar('\n');
	}
#endif // DEBUG
	saveCompareseed("blabla.bin", &img, 8);
	convertCompressedImageToPGM("blabla.bin", "filename.txt");
}