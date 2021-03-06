/*************************
*Daniel Toro
*dtoro@g.clemson.edu
*************************/




#include "ppm_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// function to skip white spaces and comments
// pass input file pointer a argument. No return
void ckws_comments(FILE *infileptr){
  int done = 0;
  while(done == 0){
    char currentChar = fgetc(infileptr);
    if((isspace(currentChar)) == 0 && strcmp(&currentChar, "#") != 0){
      ungetc(currentChar, infileptr);
      done = 1;
    };
    if(strcmp(&currentChar, "#") == 0){
      char newLine = 0;
      while(newLine == 0){
        currentChar = fgetc(infileptr);
        if(strcmp(&currentChar, "\n") == 0){
          newLine = 1;
          done = 1;
        };
      };
    };
  };
}

// opens input files and stores pointers in FILE pointer array given
// first argument should be "average" or "median" based on noise reduction
// method desired.
void openInputFiles(char* name, FILE* inPut[]){
  // open files named average###.ppm
  if(strcmp(name, "average") == 0){
    int i;
    char fileName[20];
    for(i = 0; i < 10; i++){
      int ext = i+1;
      sprintf(fileName, "average_%03d.ppm", ext);
      inPut[i] = fopen(fileName, "r");
      if(inPut[i] == NULL){
        printf("Error: Could not open %s", fileName);
        exit(0);
      };
    }
  };
  // open files named median###.ppm
  if(strcmp(name, "median") == 0){
    int i;
    char fileName[20];
    for(i = 0; i < 9; i++){
      int ext = i+1;
      sprintf(fileName, "median_%03d.ppm", ext);
      inPut[i] = fopen(fileName, "r");
      if(inPut[i] == NULL){
        printf("Error: Could not open %s", fileName);
        exit(0);
      };
    }
  };

}

// removes noise using average value of pixels
// takes image pointer array as argument
image_t* removeNoiseAverage(image_t* img[]){
  image_t* newImg = (image_t*) malloc(sizeof(image_t));
  newImg->header = img[0]->header;
  int sum = 0;
  int i;
  int r = 0;
  int totalPixels = (img[0]->header.HEIGHT)*(img[0]->header.WIDTH);

  newImg->pixels = (pixel_t*) malloc(sizeof(pixel_t) * totalPixels);
  while(r < totalPixels){
  // Average Red values
    for(i = 0; i < 10; i++){
      int val = img[i]->pixels[r].R;
      sum += val;
    };
    int avg = sum/10;
    newImg->pixels[r].R = avg;
    sum = 0;
  // Average Green values
    for(i = 0; i < 10; i++){
      int val = img[i]->pixels[r].G;
      sum += val;
    };
    avg = sum/10;
    newImg->pixels[r].G = avg;
    sum = 0;
  // Average Blue values
    for(i = 0; i < 10; i++){
      int val = img[i]->pixels[r].B;
      sum += val;
    };
    avg = sum/10;
    newImg->pixels[r].B = avg;
    sum = 0;
    //move on to next pixel
    r++;
  };
  return newImg;
};

//opens values using median pixel values
image_t* removeNoiseMedian(image_t* image[]){
  image_t* newImg = (image_t*) malloc(sizeof(image_t));
  newImg->header = image[0]->header;
  unsigned int nums[9];
  int i;
  int r = 0;
  int totalPixels = (image[0]->header.HEIGHT)*(image[0]->header.WIDTH);
  newImg->pixels = (pixel_t*) malloc(sizeof(pixel_t) * totalPixels);

  while(r < totalPixels){
  // Average Red values
    for(i = 0; i < 9; i++){
      int val = image[i]->pixels[r].R;
      nums[i] = val;
    };
    sort(nums, 9);
    int median = nums[4];
    newImg->pixels[r].R = median;

  // Average Green values
    for(i = 0; i < 9; i++){
      int val = image[i]->pixels[r].G;
      nums[i] = val;
    };
    sort(nums, 9);
    median = nums[4];
    newImg->pixels[r].G = median;

  // Average Blue values
    for(i = 0; i < 9; i++){
      int val = image[i]->pixels[r].B;
      nums[i] = val;
    };
    sort(nums, 9);
    median = nums[4];
    newImg->pixels[r].B = median;

    //move on to next pixel
    r++;
  };

  return newImg;
};

//swap values in two memory locations
void swap(unsigned int* a, unsigned int* b){
  unsigned int temp = *(a);
  *(a) = *(b);
  *(b) = temp;
}

//sort using comb sort
void sort(unsigned int* arr, int n){
  unsigned int* indexer = arr;
  int i;
  int gap = 4;
  double shrink = 1.3;
  int swaps = 1;
  int totalSwaps = 0;
  while(swaps > 0 && gap != 0){
    swaps = 0;
    indexer = arr;
    for(i = 0; i < n; i++, indexer++){
      if((*indexer) > (*(indexer+gap))){
        swaps++;
        swap(indexer, (indexer+gap));

      };
    gap = gap / shrink;
    gap = gap < 1 ? 1 : gap;
    };
    totalSwaps += swaps;
  };

};

//read a header using ckws_comments to skip comments and spaces
header_t read_header(FILE* image_file) {
  header_t header;
    fscanf(image_file, "%s", header.MAGIC_NUMBER);
    ckws_comments(image_file);
    fscanf(image_file, "%u", &header.WIDTH);
    ckws_comments(image_file);
    fscanf(image_file, "%u", &header.HEIGHT);
    ckws_comments(image_file);
    fscanf(image_file, "%u", &header.MAX_COLOR);
    ckws_comments(image_file);
  return header;
}

//write header to a file
void write_header(FILE* out_file, header_t header) {
  fprintf(out_file, "%s %u %u %u ",
    header.MAGIC_NUMBER, header.WIDTH, header.HEIGHT, header.MAX_COLOR);
}

//read a ppm file and store in an image structure
image_t* read_ppm(FILE* image_file) {
  header_t header = read_header(image_file);
  image_t* image = NULL;
  if (strcmp("P3", header.MAGIC_NUMBER) == 0) {
    image = read_p3(image_file, header);
  } else if (strcmp("P6", header.MAGIC_NUMBER) == 0) {
    image = read_p6(image_file, header);
  }
  return image;
}

//Read a file in p6 format
image_t* read_p6(FILE* image_file, header_t header) {
  int num_pixels = header.HEIGHT * header.WIDTH;
  image_t* image = (image_t*) malloc(sizeof(image_t));
  image->header = header;
  image->pixels = (pixel_t*) malloc(sizeof(pixel_t) * num_pixels);
  for(int i = 0; i < num_pixels; i++) {
    uint8_t r,g,b;
    fscanf(image_file, "%c%c%c", &r, &g, &b);
    image->pixels[i].R = r;
    image->pixels[i].G = g;
    image->pixels[i].B = b;
  }
  return image;
}

//read a file in p3 format
image_t* read_p3(FILE* image_file, header_t header) {
  int num_pixels = header.HEIGHT * header.WIDTH;
  image_t* image = (image_t*) malloc(sizeof(image_t));
  image->header = header;
  image->pixels = (pixel_t*) malloc(sizeof(pixel_t) * num_pixels);
  for(int i = 0; i < num_pixels; i++) {
    unsigned int r,g,b;
    fscanf(image_file, "%u %u %u ", &r, &g, &b);
    image->pixels[i].R = (uint8_t) r;
    image->pixels[i].G = (uint8_t) g;
    image->pixels[i].B = (uint8_t) b;
  }
  return image;
}

//write a file in p6 format
void write_p6(FILE* out_file, image_t* image) {
  header_t header = image->header;
  header.MAGIC_NUMBER[1] = '6';
  write_header(out_file, header);
  int num_pixels = header.HEIGHT * header.WIDTH;
  for(int i = 0; i < num_pixels; i++) {
    fprintf(out_file, "%c%c%c",
      image->pixels[i].R,
      image->pixels[i].G,
      image->pixels[i].B
    );
  }
}

//write a file in p3 format
void write_p3(FILE* out_file, image_t* image) {
  header_t header = image->header;
  header.MAGIC_NUMBER[1] = '3';
  write_header(out_file, header);
  int num_pixels = header.HEIGHT * header.WIDTH;
  for(int i = 0; i < num_pixels; i++) {
    fprintf(out_file, "%u %u %u ",
      image->pixels[i].R,
      image->pixels[i].G,
      image->pixels[i].B
    );
  }
}
