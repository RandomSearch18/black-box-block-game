#include "blackbox.h"

#define BLACKBOX_TIMEOUT_1 1
#define BLACKBOX_TIMEOUT_2 125

BlackBox* blackbox;
int pixels[8] = {0};
int blocks[8] = {0}; 
int sprite_x = 4;
int sprite_y = 0;
int tone_ttl = -1;

void sprite_down() {
  // Drop down one pixel
  sprite_y += 1;
  // Check if we've hit another block, or the bottom of the screen
  if (blocks[sprite_y + 2] & (1 << (7 - sprite_x)) || sprite_y + 1 == 7) {
    blocks[sprite_y] |= 1 << (7 - sprite_x);
    blocks[sprite_y + 1] |= 1 << (7 - sprite_x);
    // Respawn the sprite at the top
    sprite_x = 4;
    sprite_y = 0;
    if (blocks[sprite_y + 1] & (1 << (7 - sprite_x))) {
      you_just_died();
    } else {
      blackbox.piezo.tone(400);
      tone_ttl = 40;
    }
  }
}

void on_up() {}
void on_down() {
  sprite_down();
}
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

void you_just_died() {
  int delay = 500;
  blackbox.piezo.tone(800);
  blackbox.matrix.turn_all_on();
  blackbox.sleep(delay);
  blackbox.piezo.tone(700);
  blackbox.matrix.turn_all_off();
  blackbox.sleep(delay);
  blackbox.piezo.tone(600);
  blackbox.matrix.turn_all_on();
  blackbox.sleep(delay);
  blackbox.piezo.tone(400);
  tone_ttl = 200;

  // Reset game
  for (int i = 0; i < 8; i++) {
      blocks[i] = 0;
  }
}

void main() {
  int clock = 0;
  while (1) {
    // Update data
    int drop_rate = 200;
    // Wait a bit after the game starts before doing the first drop
    int initial_delay = 0; // 300
    if (clock >= initial_delay && clock % drop_rate == drop_rate - 1) {
      sprite_down();
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