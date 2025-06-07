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

int init_buffer(image*);
void update_buffer(void);
void cleanup_buffer(void);
