#pragma once

#include "types.h"

// buttons
typedef enum {
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_ENTER,
	KEY_BACK,
} key_code_t;

// Button event callback - platform calls this when a button is pressed/released
// key: which button (KEY_UP, KEY_DOWN, etc.)
// state: BTN_PRESSED or BTN_RELEASED
typedef void (*button_callback_fn)(key_code_t key, button_state_t state);

// Initialize input system with a platform-specific poll function
// poll_fn: pointer to platform-specific input polling (e.g., sdl_poll_input)
void init_input(void (*poll_fn)(void));

// Poll for button input (call this regularly in gui_task)
void update_input(void);

// Register a callback for button events (optional, for event-based handling)
void register_button_callback(button_callback_fn callback);

// Get current button state (simple polling API)
// Returns: BTN_PRESSED if button is currently pressed, BTN_RELEASED otherwise
button_state_t get_button_state(key_code_t key);

// Platform-specific helper: set button state (called by platform layer like SDL)
// This triggers edge detection and optional callbacks
void set_button_state(key_code_t key, button_state_t state);

