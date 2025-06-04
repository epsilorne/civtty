#include <stdint.h>

int init_buffer(void);
void render_buffer(void);
void cleanup_buffer(void);

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} pixel;
