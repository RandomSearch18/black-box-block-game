#include "blackbox.h"

#define BLACKBOX_TIMEOUT_1 1
#define BLACKBOX_TIMEOUT_2 125

BlackBox* blackbox;
int pixels[8] = {0};
int blocks[8] = {0}; 
int sprite_x = 4;
int sprite_y = 0;
int tone_ttl = -1;

// These functions are called when the buttons are pressed
void on_up() {}
void on_down() {}
void on_left() {
  if (sprite_x <= 0) {
    sprite_x = 8;
  }
  sprite_x -= 1;
}
void on_right() {
  if (sprite_x >= 7) {
    sprite_x = -1;
  }
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


void main() {
  int clock = 0;
  while (1) {
    // Update data
    int drop_rate = 200;
    // wait a bit after the game starts before doing the first drop
    int initial_delay = 0; // 300
    if (clock >= initial_delay && clock % drop_rate == drop_rate - 1) {
      // Attempt to drop down one pixel
      int new_y = sprite_y + 1;
      // Check if the bottom pixel of the sprite would be in a block
      // or if we've hit the bottom of the screen
      if (blocks[new_y + 1] & (1 << (7 - sprite_x)) || new_y + 1 == 8) {
        blocks[sprite_y] |= 1 << (7 - sprite_x);
        blocks[sprite_y + 1] |= 1 << (7 - sprite_x);
        sprite_x = 4;
        sprite_y = 0;
        blackbox.piezo.tone(400);
        tone_ttl = 40;
      } else {
        sprite_y += 1;
      }
    }

    // Handle stopping the tone
    tone_ttl--;
    if (tone_ttl == 0) {
      blackbox.piezo.no_tone();
    }
    
    // Draw to screen
    for (int i = 0; i < 8; i++) {
      pixels[i] = blocks[i];
    }
    pixels[sprite_y] |= 1 << (7 - sprite_x);
    pixels[sprite_y + 1] |= 1 << (7 - sprite_x);    
    blackbox.matrix.set_from_integers(pixels);
    clock++;
  }
}