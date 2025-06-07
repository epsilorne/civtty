#include <stdint.h>

#ifndef RENDER_H
#define RENDER_H

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} pixel;

typedef struct {
  int width;
  int height;
  pixel* pixels;
} image;

#endif

void do_rendering(image*);
