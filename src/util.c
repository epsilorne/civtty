#include "util.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <termios.h>
#include <unistd.h>

/**
 * Load a file and store it in memory as a loaded_file.
 */
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

/**
 * Clear up a loaded_file from memory.
 */
void clear_file(loaded_file* file) {
  free(file->contents);
  free(file);
}

/**
 * Exit raw mode and return to cooked (regular) terminal mode.
 */
void exit_raw_mode(struct termios original) {
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &original) == -1) {
    perror("tcsetattr");
  }

  // Disable the temp buffer and restore screen/cursor
  printf("\033[?1049l");
  printf("\033[?47l");
  printf("\033[u");
  printf("\033[?25h");
}

/**
 * Enter raw mode so we can directly handle stdin.
 */
void enter_raw_mode(struct termios original) {
  struct termios raw;
  if(tcgetattr(STDIN_FILENO, &raw) == -1){
    perror("tcgetattr");
  }

  // Set flags for raw mode
  raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
  raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag &= ~(CS8);

  // Hide the cursor, save cursor & screen, and use a temp buffer
  printf("\033[?25l");
  printf("\033[s");
  printf("\033[?47h");
  printf("\033[?1049h");

  // Finally set terminal mode to raw
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    perror("tcsetattr");
  }
}
