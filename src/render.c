#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include "render.h"

uint8_t ROWS;
uint8_t COLS;

pixel** IMAGE_BUFFER;

const char* CLEAR_ANSI = "\033[0m";
const char* BLOCK = "▀";
char BLOCK_BUFFER[25];

int init_buffer(void) {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  
  // We can double the no. of rows since we're using 'half blocks'
  ROWS = w.ws_row * 2;
  COLS = w.ws_col;

  // Allocate memory for the buffer
  IMAGE_BUFFER = (pixel**) malloc(ROWS * sizeof(IMAGE_BUFFER));
  for(int i = 0; i < ROWS; ++i) {
    IMAGE_BUFFER[i] = (pixel*) malloc(COLS * sizeof(*IMAGE_BUFFER[i]));
  }

  // Populate with dummy values (TODO: remove)
  for(int x = 0; x < COLS; ++x) {
    for(int y = 0; y < ROWS; ++y) {
      pixel px = {rand() % 255, rand() % 255, rand() % 255};
      IMAGE_BUFFER[y][x] = px;
    }
  }

  return 0;
}

/**
 * Given a pixel, generate the ANSI code to render a BLOCK.
 */
char* generate_ansi(pixel px, short fg) {
  snprintf(BLOCK_BUFFER, 25, "\033[%i;2;%i;%i;%im", fg == 1 ? 38 : 48, px.r, px.g, px.b);
  return BLOCK_BUFFER;
}

void draw_pixel(int x, int y) {
  pixel px = IMAGE_BUFFER[y][x];

  char *ansi_fg = generate_ansi(px, 1);
  fputs(ansi_fg, stdout);
  
  // If not at the last row, we can also draw the pixel below 
  if(y != ROWS - 1) {
    pixel px_below = IMAGE_BUFFER[y + 1][x];
    char *ansi_bg = generate_ansi(px_below, 0);
    fputs(ansi_bg, stdout);
  }
  fputs(BLOCK, stdout);
  fputs(CLEAR_ANSI, stdout);
  fputs(CLEAR_ANSI, stdout);
}

void render_buffer(void) {
  init_buffer();

  // Iterate y-axis for every other row (since we draw two at a time)
  for(int y = 0; y < ROWS; y += 2) {
    for(int x = 0; x < COLS; ++x) {
      draw_pixel(x, y);
    }
    putchar('\n');
  }
  fflush(stdout);
  printf("IMG WIDTH: %i, IMG HEIGHT: %i\n", COLS, ROWS);
}

void cleanup_buffer(void) {
  for(int i = 0; i < ROWS; ++i) {
    free(IMAGE_BUFFER[i]); 
  }
  free(IMAGE_BUFFER);
}
