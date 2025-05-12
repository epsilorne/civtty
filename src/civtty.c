#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "civtty.h"

const char* BLOCK = "▀";

char* generate_ansi(uint8_t r, uint8_t g, uint8_t b, short fg) {
  // The sequences we generate are fixed length so it's okay?
  char* seq = malloc(25);
  snprintf(seq, 25, "\033[%i;2;%i;%i;%im", fg == 1 ? 38 : 48, r, g, b);
  return seq;
}

void print_pixel(int x, int y) {
  pixel px = IMAGE_BUFFER[y][x];

  // We print two pixels in the buffer at the same time 
  if(y < BUFF_HEIGHT - 2) {
    pixel px_below = IMAGE_BUFFER[y + 1][x];
    char* ansi_fg = generate_ansi(px.r, px.g, px.b, 1);
    char* ansi_bg = generate_ansi(px_below.r, px_below.g, px_below.b, 0);
    printf("%s%s%s\033[0m\033[0m", ansi_fg, ansi_bg, BLOCK);
    // TODO: don't call malloc a billion times
    free(ansi_fg);
    free(ansi_bg);
  }
  // But for the last row, only print one pixel
  else {
    char* ansi_fg = generate_ansi(px.r, px.g, px.b, 1);
    printf("%s%s\033[0m\033[0m", ansi_fg, BLOCK);
    free(ansi_fg);
  }
}

void display_buffer(void) {
  // The y-axis in image buffer is iterated for every other row
  for(int y = 0; y < BUFF_HEIGHT; y += 2) {
    for(int x = 0; x < BUFF_WIDTH; ++x) {
      print_pixel(x, y);
    }
    printf("\n");
  }
}

int main(int argc, char *argv[]) {
  for(int j = 0; j < BUFF_HEIGHT; ++j) {
    for(int i = 0; i < BUFF_WIDTH; ++i) {
      pixel px = {i * 3, i * 3, j * 3};
      IMAGE_BUFFER[j][i] = px;
    }
  }
  display_buffer();

  return 0;
}
