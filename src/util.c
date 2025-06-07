#include "util.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

loaded_file* load_file(char* path) {
  FILE* fptr;

  if((fptr = fopen(path, "rb")) == NULL) {
    perror("fopen");
    return NULL;
  }

  fseek(fptr, 0L, SEEK_END);
  size_t image_size = ftell(fptr);
  unsigned char* BUFFER = malloc((sizeof(unsigned char) * image_size) + 1);
  rewind(fptr);

  if(fread(BUFFER, sizeof(unsigned char), image_size, fptr) == 0) {
    perror("fread");
    fclose(fptr);
    return NULL;
  }
  fclose(fptr);
  BUFFER[image_size] = '\0';

  loaded_file* file = malloc(sizeof(loaded_file));
  file->size = image_size;
  file->contents = BUFFER;

  return file;
}

void clear_file(loaded_file* file) {
  free(file->contents);
  free(file);
}
