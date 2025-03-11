#include "blackbox.h"

int pixels[8] = {0};
int sprite_x = 4;
int sprite_y = 0;

void setup() {
  pixels[sprite_y + 1] |= 1 << (7 - sprite_x);
  debug_print("%d: %d", sprite_y + 1, pixels[sprite_y + 1]);
  bb_matrix_set_arr(pixels);
}