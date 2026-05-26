#include "input.h"

// Button states (track current state of each button)
static button_state_t button_states[6] = { BTN_RELEASED };

// Platform-specific poll function pointer
static void (*platform_poll_fn)(void) = nullptr;

// Optional button event callback
static button_callback_fn button_event_callback = nullptr;

// Initialize input system
void init_input(void (*poll_fn)(void))
{
	platform_poll_fn = poll_fn;
	// Initialize all button states to released
	for (int i = 0; i < 6; i++) {
		button_states[i] = BTN_RELEASED;
	}
}

// Poll for button input
void update_input(void)
{
	if (platform_poll_fn) {
		platform_poll_fn();
	}
}

// Register a callback for button events
void register_button_callback(button_callback_fn callback)
{
	button_event_callback = callback;
}

// Get current button state
button_state_t get_button_state(key_code_t key)
{
	if (key >= 0 && key < 6) {
		return button_states[key];
	}
	return BTN_RELEASED;
}

// Helper function for platform layer to set button state
// This is called by platform-specific code (e.g., SDL polling)
void set_button_state(key_code_t key, button_state_t state)
{
	if (key >= 0 && key < 6) {
		// Check if state changed (rising edge detection for press, falling for release)
		if (button_states[key] != state) {
			button_states[key] = state;
			// Trigger callback if registered
			if (button_event_callback) {
				button_event_callback(key, state);
			}
		}
	}
}
