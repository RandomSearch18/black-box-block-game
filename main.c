#include "blackbox.h"

int clock = 0;
uint8_t pixels[8] = {0};
uint8_t blocks[8] = {0}; 
int sprite_x = 4;
int sprite_y = 0;
int tone_ttl = -1;
// Increases by 1 each frame the sprite stays at the same vertical level
int sprite_boredom = 0;
// 1 while in-game, 0 when in an animation (i.e. death screen)
int gaming = 1;

// How often (in frames) the sprite should drop down
int drop_rate = 200;
// Wait a bit after the game starts before doing the first drop
int initial_delay = 0; // 300

void toggle_pixel(int y, int x) {
    // pixels[y] ^= 1;
}

void you_just_died() {
  int delay = 500;
  gaming = 0;
  bb_tone(700);
  bb_matrix_all_on();
  // blackbox.sleep(delay);
  bb_tone(600);
  bb_matrix_all_off();
  // blackbox.sleep(delay);
  bb_tone(500);
  bb_matrix_all_on();
  // blackbox.sleep(delay);
  bb_tone(350);
  tone_ttl = 200;

  // Reset game
  for (int i = 0; i < 8; i++) {
      blocks[i] = 0;
  }
  gaming = 1;
}

void sprite_down() {
  //debyg
  toggle_pixel(0,0);
  
  // Drop down one pixel
  sprite_y += 1;
  sprite_boredom = 0;
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
      bb_tone(400);
      tone_ttl = 40;
    }
  }
}

void on_up(task_handle self) {}
void on_down(task_handle self) {
  if (gaming) {
   sprite_down(); 
  }
}
void on_left(task_handle self) {
  if (sprite_x <= 0) {
    sprite_x = 8;
  }
  sprite_x -= 1;
}
void on_right(task_handle self) {
  if (sprite_x >= 7) {
    sprite_x = -1;
  }
  sprite_x += 1;
}
void on_select(task_handle self) {}

void tick(task_handle self) {
  // Update data
  sprite_boredom++;
  if (clock >= initial_delay && sprite_boredom >= drop_rate) {
    sprite_down();
  }

  // Handle stopping the tone
  tone_ttl--;
  if (tone_ttl == 0) {
    bb_tone_off();
  }
  
  // Draw to screen
  for (int i = 0; i < 8; i++) {
    pixels[i] = blocks[i];
  }
  pixels[sprite_y] |= 1 << (7 - sprite_x);
  pixels[sprite_y + 1] |= 1 << (7 - sprite_x);
  bb_matrix_set_arr(pixels);
  clock++;
  //debug_print("sprite_x=%d, y=%d", sprite_x, sprite_y);
}

void setup() {
  task_create_interval(tick, 100);
  task_create_event(on_up, EVENT_PRESS_UP);
  task_create_event(on_down, EVENT_PRESS_DOWN);
  task_create_event(on_left, EVENT_PRESS_LEFT);
  task_create_event(on_right, EVENT_PRESS_RIGHT);
  task_create_event(on_select, EVENT_PRESS_SELECT);
}