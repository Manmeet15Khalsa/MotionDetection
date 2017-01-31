#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <math.h>

typedef struct point_{
    uint16_t x;
    uint16_t y;
} Point;

typedef struct arraySize_{
    uint16_t height;
    uint16_t width;
} ArraySize;

uint8_t ** create2DArray(uint16_t width, uint16_t height);

float ** readKernel(ArraySize* kSize, char * kernelFile );

float ** createEmptyKernel(int kernelSize);

void printKernel(float **kernel, ArraySize *size);

float ** flipKernel(float ** kernel, ArraySize * size);

float ** normalizeKernel(float ** kernel, ArraySize *size);

Point * computeCentroid(uint8_t ** image, uint16_t width, uint16_t height);

uint8_t ** filterImage(uint8_t ** origImage, uint8_t ** modImage, ArraySize * imageSize, float ** kernel, ArraySize *kernelSize );

void writeImage(uint8_t **image, ArraySize * size, char *fileName);

//void filter(uint8_t **image, ArraySize * imageSize, char * kernelFile, ArraySize * kSize);

void freeImage(uint8_t **image, uint16_t height);

//uint8_t ** readImage(char * fileName, uint16_t width, uint16_t height);

uint8_t ** readImage(char * fileName, uint8_t **image, uint16_t width, uint16_t height);

void processString(char * str);

int findDistance(Point *c1, Point *c2);

uint8_t findMaxDiff(uint8_t **image1, uint8_t **image2, ArraySize *size);


