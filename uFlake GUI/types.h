#pragma once

#include <cstdint>
#include <cstddef>

typedef uint16_t ugui_color;

// Font sizes enum
typedef enum {
	FONT_8X8 = 0,
	FONT_7X12 = 1,
	FONT_8X12 = 2,
	FONT_8X14 = 3
} font_size_t;

// Forward declare ugui for the function pointer
typedef struct ugui ugui;

// Flush function pointer - user implements this
typedef void (*flush_fn)(ugui *gui);

// Main GUI structure
typedef struct ugui
{
	uint16_t* buffer;      // Always points to active buffer
	uint16_t* buffer1;
	uint16_t* buffer2;
	size_t buffer_size;
	uint16_t width;
	uint16_t height;
	ugui_color color;
	flush_fn flush_func;
	bool active_buffer;
	uint8_t font_width;    // Current font width
	uint8_t font_height;   // Current font height
	const uint8_t* font_data;  // Pointer to current font data
} ugui;

extern ugui* ugui_instance;

// Configuration passed by user
typedef struct {
	uint16_t width;
	uint16_t height;
	uint8_t rotation;
	flush_fn flush_func;
	uint16_t* buffer1;
	uint16_t* buffer2;
} ugui_config;

// Button states
typedef enum {
	BTN_RELEASED = 0,
	BTN_PRESSED = 1
} button_state_t;

