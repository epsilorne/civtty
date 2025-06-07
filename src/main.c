#include "render.h"
#include "ppm.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if(argc != 2) {
    printf("Usage: %s [file]\n", argv[0]);
    return 1;
  }

  if(strcmp(strrchr(argv[1], '\0') - 4, ".ppm")) {
    printf("Only .ppm files are supported.\n");
    return 1;
  }

  image* img = NULL;
  if((img = parse_ppm(argv[1])) == NULL) {
    return 1;
  }
  init_buffer(img);
  update_buffer();

  cleanup_buffer();
  clear_image(img);
  return 0;
}
