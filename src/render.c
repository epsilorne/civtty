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

// Buffers for image file + displayed stuff
pixel* PIX_BUFFER;
image* CURR_IMAGE;

// Related to actually displaying on screen
char ANSI_BUFFER[25];
const char* CLEAR_ANSI = "\033[0m";
const char* BLOCK = "▀";

int idx(int y, int x) {
  return y * COLS + x;
}

/**
 * Allocate memory for the pixel buffer.
 */
void init_buffer(image* img) {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  
  // We can double the no. of rows since we're using 'half blocks'
  ROWS = w.ws_row * 2;
  COLS = w.ws_col;

  // Allocate memory for the buffer
  PIX_BUFFER = calloc(COLS * ROWS, sizeof(pixel));
  CURR_IMAGE = img;
}

/**
 * Given a pixel, generate the ANSI code to render a BLOCK in the
 * pixel's colour.
 */
char* generate_ansi(pixel px, bool fg) {
  snprintf(ANSI_BUFFER, 25, "\033[%i;2;%i;%i;%im", fg == true ? 38 : 48,
                                                   px.r, px.g, px.b);
  return ANSI_BUFFER;
}

/**
 * Draw an individual pixel on the buffer. Assumes the coordinates to
 * be on-screen.
 */
void draw_pixel(int x, int y) {
  pixel px = PIX_BUFFER[idx(y, x)];

  char *ansi_fg = generate_ansi(px, 1);
  fputs(ansi_fg, stdout);
  
  // If not at the last row, we can also draw the pixel below 
  if(y != ROWS - 1) {
    pixel px_below = PIX_BUFFER[idx(y + 1, x)];
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

  for(int y = 0; y < CURR_IMAGE->height; ++y) {
    for(int x = 0; x < CURR_IMAGE->width; ++x) {
      int offset_idx = idx(y + offset_y, x + offset_x);
      bool within_bounds = x + offset_x >= 0 &&
         x + offset_x < COLS &&
         y + offset_y >= 0 &&
         y + offset_y < ROWS;

      if(within_bounds) {
        PIX_BUFFER[offset_idx] = CURR_IMAGE->pixels[pixel_idx]; }
      pixel_idx++;
    }
  }
}

/**
 * Display the contents of the buffer on screen.
 */
void draw_buffer(void) {
  memset(PIX_BUFFER, 0, sizeof(pixel) * COLS * ROWS);
  offset_image();

  // Iterate y-axis for every other row (since we draw two at a time)
  for(int y = 0; y < ROWS; y += 2) {
    for(int x = 0; x < COLS; ++x) {
      draw_pixel(x, y);
    }
  }
  fflush(stdout);
}

/**
 * Main functional loop; given an image, display it and allow for movement.
 */
void do_rendering(image* img) {
  init_buffer(img);

  // TODO: input loop to update positions or quit
  draw_buffer();

  // Once done, clean up all allocated memory
  free(img->pixels);
  free(img);
  free(PIX_BUFFER);
}
