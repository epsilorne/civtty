#include <stdint.h>

#define BUFF_HEIGHT 70
#define BUFF_WIDTH 70

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} pixel;

pixel IMAGE_BUFFER[BUFF_WIDTH][BUFF_HEIGHT];
