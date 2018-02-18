#include "ppm_utils.h"
#include <stdio.h>


void ckws_comments(FILE *infileptr){
  int done = 0;
  while(done == 0){
    char currentChar = fgetc(infileptr);
    if((isspace(currentChar)) == 0 && currentChar != "#"){
      ungetc(infileptr);
      done = 1;
    };
    if(currentChar == "#"){
      char newLine = 0;
      while(newLine == 0){
        if((fgetc(infileptr)) == "\n"){
          newLine = 1;
          done = 1;
        };
      };
    };
  };
}

void openInputFiles(char* name, FILE* inPut[]){
  if(strcmp(name, "average") == 0){
    int i;
    char fileName[20];
    for(i = 0; i < 10; i++){
      int ext = i+1;
      sprintf(fileName, "average_%3d.ppm", &ext);
      inPut[i] = fopen(fileName, "r");
      if(inPut[i] == NULL){
        printf("Error: Could not open %s", fileName);
        return 1;
      };
    }
  };
  if(strcmp(name, "median") == 0){
    int i;
    char fileName[20];
    for(i = 0; i < 9; i++){
      int ext = i+1;
      sprintf(fileName, "median_%3d.ppm", &ext);
      inPut[i] = fopen(fileName, "r");
      if(inPut[i] == NULL){
        printf("Error: Could not open %s", fileName);
        return 1;
      };
    }
  };

}

image_t* removeNoiseAverage(image_t* img[]){
  image_t newImg;
  int sum = 0;
  int i;
  int r = 0;
  int totalPixels = (img[0]->header->HEIGHT)*(img[0]->header->WIDTH);
//delete this comment. replace with a while loop increasing the pixel that is averaged
  while(r < totalPixels){
  // Average Red values
    for(i = 0; i < 10; i++){
      int val = img[i]->pixels[r]->R;
      sum += val;
    };
    int avg = sum/10;
    newImg->pixels[r]->R = avg;

  // Average Green values
    for(i = 0; i < 10; i++){
      int val = img[i]->pixels[r]->G;
      sum += val;
    };
    int avg = sum/10;
    newImg->pixels[r]->G = avg;

  // Average Blue values
    for(i = 0; i < 10; i++){
      int val = img[i]->pixels[r]->B;
      sum += val;
    };
    int avg = sum/10;
    newImg->pixels[r]->B = avg;

    //move on to next pixel
    r++;
  };
};

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

void write_header(FILE* out_file, header_t header) {
  fprintf(out_file, "%s %u %u %u ",
    header.MAGIC_NUMBER, header.WIDTH, header.HEIGHT, header.MAX_COLOR);
}

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

image_t* copy_image(image_t* image) {
  // Size of a header_t and a pointer to pixel data somewhere else in memory
  image_t* copy = (image_t*) malloc(sizeof(image_t));
  // Copy assignment of header info
  copy->header = image->header;
  int num_pixels = copy->header.WIDTH * copy->header.HEIGHT;
  // Allocate memory for pixel data
  copy->pixels = (pixel_t*) malloc(sizeof(pixel_t)*num_pixels);
  // One line deep copy
  memcpy(copy->pixels, image->pixels, sizeof(pixel_t)*num_pixels);
  return copy;
}
