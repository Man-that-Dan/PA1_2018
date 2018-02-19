#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppm_utils.h"

int main(int argc, char * argv[]){
  if(argc < 3){
    printf("Arguments not provided. Usage: ./a.out <noise removal type> <output file name>");
    return 1;
  };

  if(strcmp(argv[1], "median") != 0 && strcmp(argv[1], "average") != 0){
    printf("Incorrect usage");
    return 1;
  };
  char name[20];
  strcpy(name, argv[1]);
  char* namePtr = name;
  char outFile[50];
  strcpy(outFile, argv[2]);

  FILE * optr;
  optr = fopen(outFile, "w");
  if(optr == NULL){
    printf("Could not open output file");
    return 1;
  };

  FILE* medFiles[9];
  FILE* avgFiles[10];

  image_t** medImgs = (image_t**)malloc(sizeof(image_t)*9);
  image_t** avgImgs = (image_t**)malloc(sizeof(image_t)*10);

  int o;
  image_t* outputImg;
  if(strcmp(argv[1], "average") == 0){
      int i;
      openInputFiles(namePtr, avgFiles);
      for(i = 0; i < 10; i++){
        avgImgs[i] = read_ppm(avgFiles[i]);

      };
      outputImg = removeNoiseAverage(avgImgs);
      write_p6(optr, outputImg);

      for(o = 0; o < 10; o++){
        fclose(avgFiles[o]);
      };
  };

  if(strcmp(argv[1], "median") == 0){
      int i;
      openInputFiles(namePtr, medFiles);
      for(i = 0; i < 9; i++){
        medImgs[i] = read_ppm(medFiles[i]);

      };
      outputImg = removeNoiseMedian(medImgs);
      write_p3(optr, outputImg);

      for(o = 0; o < 9; o++){
        fclose(medFiles[o]);
      };
  };

  free(medImgs);
  free(avgImgs);
  free(outputImg->pixels);
  free(outputImg);
  fclose(optr);

}
