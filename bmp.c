#include "bmp.h"

#include <stdio.h>
#include <stdlib.h>

// correct values for the header
#define MAGIC_VALUE 0X4D42
#define BITS_PER_PIXEL 24
#define NUM_PLANE 1
#define COMPRESSION 0

// return 0 if the header is invalid
// return 1 if the header is valid
static int checkHeader(BMP_Header* hdr) {
  if ((hdr->type) != MAGIC_VALUE) {
    return 0;
  }
  if ((hdr->bits) != BITS_PER_PIXEL) {
    return 0;
  }
  if ((hdr->planes) != NUM_PLANE) {
    return 0;
  }
  if ((hdr->compression) != COMPRESSION) {
    return 0;
  }
  return 1;
}

// close opened file and release memory
BMP_Image* cleanUp(FILE* fptr, BMP_Image* img) {
  if (fptr != NULL) {
    fclose(fptr);
  }
  free(img->data);
  free(img);
  return NULL;
}

BMP_Image* BMP_open(const char* filename) {
  FILE* fptr = NULL;
  BMP_Image* img = NULL;
  fptr = fopen(filename, "r");  // "rb" unnecessary in Linux
  if (fptr == NULL) {
    return cleanUp(fptr, img);
  }
  img = malloc(sizeof(BMP_Image));
  if (img == NULL) {
    return cleanUp(fptr, img);
  }
  // read the header
  if (fread(&(img->header), sizeof(BMP_Header), 1, fptr) != 1) {
    return cleanUp(fptr, img);
  }
  if (checkHeader(&(img->header)) == 0) {
    return cleanUp(fptr, img);
  }
  img->data_size = (img->header).size - sizeof(BMP_Header);
  img->width = (img->header).width;
  img->height = (img->header).height;
  img->bytes_per_pixel = (img->header).bits / 8;  // Bits per byte
  img->data = malloc(sizeof(unsigned char) * (img->data_size));
  if ((img->data) == NULL) {
    return cleanUp(fptr, img);
  }
  if (fread(img->data, sizeof(char), img->data_size, fptr) != (img->data_size)) {
    return cleanUp(fptr, img);
  }
  char onebyte;
  if (fread(&onebyte, sizeof(char), 1, fptr) != 0) {
    // not at the of the file but the file still has data
    return cleanUp(fptr, img);
  }
  fclose(fptr);
  return img;
}

int BMP_save(const BMP_Image* img, const char* filename) {
  FILE* fptr = NULL;
  fptr = fopen(filename, "w");
  if (fptr == NULL) {
    return 0;
  }
  // write the header first
  if (fwrite(&(img->header), sizeof(BMP_Header), 1, fptr) != 1) {
    fclose(fptr);
    return 0;
  }
  if (fwrite(img->data, sizeof(char), img->data_size, fptr) != (img->data_size)) {
    fclose(fptr);
    return 0;
  }
  fclose(fptr);
  return 1;
}

void BMP_destroy(BMP_Image* img) {
  free(img->data);
  free(img);
}
