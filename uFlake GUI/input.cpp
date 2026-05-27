#include "input.h"
#include "types.h"

// Button states (track current state of each button)
static button_state_t button_states[6] = { BTN_RELEASED };

// Optional button event callback
static button_callback_fn button_event_callback = nullptr;



// Poll for button input
void update_input(void)
{
	if (ugui_instance && ugui_instance->poll_func) {
		ugui_instance->poll_func();
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
