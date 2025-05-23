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
// 1 if the user has interacted (pressed any button) this round, 0 if the user's AFK
int has_interacted = 0;
// The Black Box will go to sleep (LEDs off, no gameplay) if there's no interaction for a whole round
int sleeping = 0;

// How often (in frames) the sprite should drop down
int drop_rate = 6;
// Wait a bit after a new piece is spawned before doing the first drop (in frames)
int initial_delay = 10;

int remaining_flashes = 0;
int flash_screen_state = 0;
int flash_screen_delay = 500;

void do_flash_screen(uint32_t task_handle) {
  // Recursive function to flash the screen on and off after a delay
  if (!remaining_flashes) {
    bb_matrix_all_off();
    flash_screen_state = 0;
    gaming = 1;
    // Reset the clock here because it's immediately before the round begins
    // (the `pixels` array has already been reset)
    clock = 0;
    if (!has_interacted) {
      // If the user hasn't touched the buttons all round, go to sleep
      gaming = 0;
      sleeping = 1;
    }
    // Ready for tracking the interactions next round
    has_interacted = 0;
    return;
  }
  if (flash_screen_state == 0) {
    bb_matrix_all_on();
    flash_screen_state = 1;
  }
  else if (flash_screen_state == 1) {
    bb_matrix_all_off();
    flash_screen_state = 0;
  }
  remaining_flashes--;
  task_create_timeout(do_flash_screen, flash_screen_delay);
}

void flash_screen(int delay, int repetitions) {
  // Set global variables
  remaining_flashes = repetitions;
  flash_screen_delay = delay;
  gaming = 0;
  // Start the screen-flashing loop
  do_flash_screen(0);
}

void you_just_died() {
  // Reset game
  for (int i = 0; i < 8; i++) {
    blocks[i] = 0;
  }

  // Play a sad sound
  bb_tone(350);
  tone_ttl = 8;

  // Show the animation
  flash_screen(500, 6);
}

void check_collision() {
  // Check if we've hit another block, or the bottom of the screen
  if (blocks[sprite_y + 2] & (1 << (7 - sprite_x)) || sprite_y + 1 == 7) {
    blocks[sprite_y] |= 1 << (7 - sprite_x);
    blocks[sprite_y + 1] |= 1 << (7 - sprite_x);
    // Respawn the sprite at the top
    sprite_x = 4;
    sprite_y = 0;
    // Reset the clock to give the player a bit of time before the first movement of the block
    clock = 0;
    if (blocks[sprite_y + 1] & (1 << (7 - sprite_x))) {
      gaming = 0;
      you_just_died();
    } else {
      bb_tone(400);
      tone_ttl = 2;
    }
  }
}

void sprite_down() {
  // Drop down one pixel
  sprite_y += 1;
  sprite_boredom = 0;
  check_collision();
}

void on_interaction() {
  has_interacted = 1;
  if (sleeping) {
    // Time to wake up!
    // If we're sleeping, we're already ready to start a new round
    // i.e. the board is already clear
    // So we just tell the tick loop to start ticking again
    sleeping = 0;
    gaming = 1;
    clock = 0;
  }
}

void on_up(task_handle self) {
  on_interaction();
}
void on_down(task_handle self) {
  if (gaming) {
    sprite_down();
  }
  on_interaction();
}
void on_left(task_handle self) {
  if (gaming) {
    if (sprite_x <= 0) {
      sprite_x = 8;
    }
    sprite_x -= 1;
    check_collision();
  }
  on_interaction();
}
void on_right(task_handle self) {
  if (gaming) {
    if (sprite_x >= 7) {
      sprite_x = -1;
    }
    sprite_x += 1;
    check_collision();
  }
  on_interaction();
}
void on_select(task_handle self) {
  on_interaction();
}
void tick(task_handle self) {
  // Update clock
  clock++;
  
  // Handle stopping the tone
  tone_ttl--;
  if (tone_ttl == 0) {
    bb_tone_off();
  }

  if (sleeping) {
    // Blink the status LED to show we're alive
    const int INTERVAL = 50;
    const int DURATION = 2;
    if (clock % INTERVAL == 0) {
      bb_matrix_set_pos(7, 7, 1);
    }
    else if (clock % INTERVAL == DURATION) {
      bb_matrix_set_pos(7, 7, 0);
    }
  }

  if (!gaming) {
    return;
  }

  // Update data
  sprite_boredom++;
  if (clock >= initial_delay && sprite_boredom >= drop_rate) {
    sprite_down();
  }

  // Draw to screen
  for (int i = 0; i < 8; i++) {
    pixels[i] = blocks[i];
  }
  pixels[sprite_y] |= 1 << (7 - sprite_x);
  pixels[sprite_y + 1] |= 1 << (7 - sprite_x);
  //debug_print("%d: %d", sprite_y + 1, pixels[sprite_y + 1]);
  bb_matrix_set_arr(pixels);
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