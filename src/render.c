#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include "render.h"

uint8_t ROWS;
uint8_t COLS;

pixel* IMAGE_BUFFER;

const char* CLEAR_ANSI = "\033[0m";
const char* BLOCK = "▀";
char BLOCK_BUFFER[25];

int idx(int y, int x) {
  return y * COLS + x;
}

int init_buffer(image* IMAGE) {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  
  // We can double the no. of rows since we're using 'half blocks'
  ROWS = w.ws_row * 2;
  COLS = w.ws_col;

  // Allocate memory for the buffer
  IMAGE_BUFFER = malloc(sizeof(pixel) * COLS * ROWS);

  // Populate with values from image
  // TODO: allow for movement
  int pixel_idx = 0;
  for(int y = 0; y < ROWS; ++y) {
    for(int x = 0; x < COLS; ++x) {
      if(x < IMAGE->width && y < IMAGE->height) {
        IMAGE_BUFFER[idx(y, x)] = IMAGE->pixels[pixel_idx];
        pixel_idx++;
      }
      else {
        IMAGE_BUFFER[idx(y, x)] = (pixel) {0, 0, 0};
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

void render_buffer(void) {
  // Iterate y-axis for every other row (since we draw two at a time)
  for(int y = 0; y < ROWS; y += 2) {
    for(int x = 0; x < COLS; ++x) {
      draw_pixel(x, y);
    }
    putchar('\n');
  }
  fflush(stdout);
}

void cleanup_buffer(void) {
  free(IMAGE_BUFFER);
}
