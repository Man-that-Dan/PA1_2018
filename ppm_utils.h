#ifndef PPM_UTILS
#define PPM_UTILS

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"

// First meaningful line of the PPM file
typedef struct header {
  char MAGIC_NUMBER[3];
  unsigned int HEIGHT, WIDTH, MAX_COLOR;
} header_t;

// Represents an RGB pixel with integer values between 0-255
typedef struct pixel {
  uint8_t R, G, B;
} pixel_t;

// PPM Image representation
typedef struct image {
  header_t header;      // 15 bytes
  pixel_t* pixels;      // 4 bytes
} image_t;

header_t read_header(FILE* image_file);

image_t* read_ppm(FILE* image_file);
image_t* read_p6(FILE* image_file, header_t header);
image_t* read_p3(FILE* image_file, header_t header);


void write_header(FILE* out_file, header_t header);
void write_p6(FILE* out_file, image_t* image);
void write_p3(FILE* out_file, image_t* image);


/*Functions for Programming Assignment 1*/
image_t* removeNoiseAverage(image_t* img[]);
image_t* removeNoiseMedian(image_t* image[]);

void ckws_comments (FILE *infileptr);
void openInputFiles(char* name, FILE* inPut[]);
void sort(unsigned int* arr, int n);
void swap(unsigned int* a, unsigned int* b);
#endif
