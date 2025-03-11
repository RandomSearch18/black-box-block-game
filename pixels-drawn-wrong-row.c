#include "blackbox.h"

int pixels[8] = {0};
// Coords of the pixel
int x = 4;
int y = 1;

void setup() {
  pixels[y] |= 1 << (7 - x);
  bb_matrix_set_arr(pixels);
}