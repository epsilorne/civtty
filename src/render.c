#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include "render.h"

int offset_x = 0;
int offset_y = 0;

uint8_t ROWS;
uint8_t COLS;

pixel* IMAGE_BUFFER;
image* IMAGE;

const char* CLEAR_ANSI = "\033[0m";
const char* BLOCK = "▀";
char BLOCK_BUFFER[25];

int idx(int y, int x) {
  return y * COLS + x;
}

int init_buffer(image* img) {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  
  // We can double the no. of rows since we're using 'half blocks'
  ROWS = w.ws_row * 2;
  COLS = w.ws_col;

  // Allocate memory for the buffer
  IMAGE_BUFFER = calloc(COLS * ROWS, sizeof(pixel));
  IMAGE = img;

  // Populate with values from image
  int pixel_idx = 0;
  for(int y = 0; y < ROWS; ++y) {
    for(int x = 0; x < COLS; ++x) {
      if(x < IMAGE->width && y < IMAGE->height) {
        IMAGE_BUFFER[idx(y, x)] = IMAGE->pixels[pixel_idx];
        pixel_idx++;
      }
    }
  }

  return 0;
}

/**
 * Given a pixel, generate the ANSI code to render a BLOCK.
 */
char* generate_ansi(pixel px, bool fg) {
  snprintf(BLOCK_BUFFER, 25, "\033[%i;2;%i;%i;%im", fg == true ? 38 : 48, px.r, px.g, px.b);
  return BLOCK_BUFFER;
}

void draw_pixel(int x, int y) {
  pixel px = IMAGE_BUFFER[idx(y, x)];

  char *ansi_fg = generate_ansi(px, 1);
  fputs(ansi_fg, stdout);
  
  // If not at the last row, we can also draw the pixel below 
  if(y != ROWS - 1) {
    pixel px_below = IMAGE_BUFFER[idx(y + 1, x)];
    char *ansi_bg = generate_ansi(px_below, 0);
    fputs(ansi_bg, stdout);
  }
  fputs(BLOCK, stdout);
  fputs(CLEAR_ANSI, stdout);
  fputs(CLEAR_ANSI, stdout);
}

/*
 * Apply positional offsets to the image within the buffer.
 */
void offset_image() {
  int pixel_idx = 0;

  for(int y = 0; y < IMAGE->height; ++y) {
    for(int x = 0; x < IMAGE->width; ++x) {
      int offset_idx = idx(y + offset_y, x + offset_x);
      // TODO: refactor this is disguisting
      if(x + offset_x < 0 || x + offset_x >= COLS || y + offset_y < 0 || y + offset_y >= ROWS) {
      }
      else if(offset_idx >= 0 && offset_idx < COLS * ROWS) {
        IMAGE_BUFFER[offset_idx] = IMAGE->pixels[pixel_idx];
      }
      pixel_idx++;
    }
  }
}

void update_buffer(void) {
  memset(IMAGE_BUFFER, 0, sizeof(pixel) * COLS * ROWS);
  offset_image();

  // Iterate y-axis for every other row (since we draw two at a time)
  for(int y = 0; y < ROWS; y += 2) {
    for(int x = 0; x < COLS; ++x) {
      draw_pixel(x, y);
    }
  }
  fflush(stdout);
}

void cleanup_buffer(void) {
  free(IMAGE_BUFFER);
}
