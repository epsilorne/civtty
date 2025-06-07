#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <termios.h>

typedef struct {
  size_t size;
  unsigned char* contents;
} loaded_file;

loaded_file* load_file(char* path);
void clear_file(loaded_file* file);
void exit_raw_mode(struct termios original);
void enter_raw_mode(struct termios original);
