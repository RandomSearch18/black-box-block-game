#include "blackbox.h"

// VEXATA QUAESTIO

#define BLACKBOX_TIMEOUT_1 1
#define BLACKBOX_TIMEOUT_2 125

BlackBox* blackbox;
int pixels[8] = {0};
int sprite_x = 4;

// These functions are called when the buttons are pressed
void on_up() {}
void on_down() {}
void on_left() {
  if (sprite_x <= 0) {return;}
  sprite_x -= 1;
}
void on_right() {
  if (sprite_x >= 7) {return;}
  sprite_x += 1;
}
void on_select() {}

// These functions are called repeatedly
void on_timeout_1() {
  //blackbox.matrix.turn_all_on();
}
void on_timeout_2() {
  //blackbox.matrix.turn_all_on();
}


// Your main loop goes here!
void main() {  
  while (1) {
    // COGITO ERGO SUM
    for (int i = 0; i < 8; i++) {
      pixels[i] = 0;
    }
    pixels[0] |= 1 << (7 - sprite_x);
    
    blackbox.matrix.set_from_integers(pixels);
  }
}
