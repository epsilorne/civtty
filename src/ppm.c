#include "ppm.h"
#include "util.h"

#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

bool ascii_format = false;
char* DELIMS = " \r\n\t";

image* parse_ppm(char* path) {
  loaded_file* file = load_file(path);
  if(file == NULL || file->size <= 2) {
    printf("Could not parse .ppm file!");
    return NULL;
  }

  int width, height, maxval;

  // Get the format (magic number) of the .ppm first
  // P3 => ASCII format, P6 => byte format. Other types are currently unsupported
  if(strncmp((char*) file->contents, "P3", 2) == 0) {
    ascii_format = true;
  }
  else if(strncmp((char*) file->contents, "P6", 2) != 0) {
    printf("Unsupported .ppm file!\n");
    return NULL;
  }

  // Position pointers for parsing
  char* pos = (char*) &file->contents[2];
  char* end;

  // Populate the width, height and max pixel values
  if((width = (int) strtol(pos, &end, 10)) == 0L) {
    printf("Could not parse 'width': %s\n", pos);
    return NULL;
  }
  pos = end;
  if((height = (int) strtol(pos, &end, 10)) == 0L) {
    printf("Could not parse 'height': %s\n", pos);
    return NULL;
  }
  pos = end;
  if((maxval = (int) strtol(pos, &end, 10)) == 0L) {
    printf("Could not parse 'maxval': %s\n", pos);
    return NULL;
  }
  pos = end;

  image* img = malloc(sizeof(image));
  img->width = width;
  img->height = height;
  img->pixels = malloc(sizeof(pixel) * width * height);

  // The index of the pixel we're parsing
  int px_idx = 0;

  // For an ASCII image, we continually read three white-spaced numbers
  if(ascii_format) {
    while(px_idx < (width * height) - 1) {
      unsigned char r = (unsigned char) strtol(pos, &end, 10);
      pos = end;
      
      unsigned char g = (unsigned char) strtol(pos, &end, 10);
      pos = end;

      unsigned char b = (unsigned char) strtol(pos, &end, 10);
      pos = end;

      img->pixels[px_idx] = (pixel) {r, g, b};
      px_idx++;
    }
  }
  // For a byte image, we just read the next three bytes
  else {
    int file_idx = 0;
    while(isspace(pos[file_idx])) {
      file_idx++;
    }

    while(px_idx < (width * height) - 1) {
      unsigned char r = pos[file_idx];
      unsigned char g = pos[file_idx + 1];
      unsigned char b = pos[file_idx + 2];
      img->pixels[px_idx] = (pixel) {r, g, b};
      px_idx++;

      file_idx += 3;
    }
  }

  clear_file(file);
  return img;
}
