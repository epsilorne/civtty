#include "render.h"

int main(int argc, char *argv[]) {
  init_buffer(); 
  render_buffer(); 
  cleanup_buffer();

  return 0;
}
