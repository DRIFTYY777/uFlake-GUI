#include "uGui.h"
#include "ugui_canvas.h"
#include "ugui_scene.h"
#include "ugui_task.h"

#include <cstdlib>

#include "colors.h"
#include "fonts.h"

ugui* ugui_instance = nullptr;

void init_ugui(ugui_config* config, ugui *instance)
{
	// User provides the ugui instance - no allocation here
	ugui_instance = instance;

	// Store both buffers
	ugui_instance->buffer1 = config->buffer1;
	ugui_instance->buffer2 = config->buffer2;
	ugui_instance->buffer_size = config->width * config->height * sizeof(uint16_t);
	ugui_instance->width = config->width;
	ugui_instance->height = config->height;
	ugui_instance->color = WHITE;  // Default text color is WHITE
	ugui_instance->flush_func = config->flush_func;
	ugui_instance->poll_func = config->poll_func;
	ugui_instance->active_buffer = 0;

	// Initialize default font (8x8)
	ugui_instance->font_width = 8;
	ugui_instance->font_height = 8;
	ugui_instance->font_data = font_8x8_data;

	// Set buffer pointer to active buffer (buffer1 initially)
	ugui_instance->buffer = ugui_instance->buffer1;

	// Initialize subsystems
	ugui_canvas_init(config->width, config->height);
	ugui_scene_manager_init();
	
	// Initialize task system with default config
	ugui_task_config_t task_config = {
		.input_poll_ms = 10,   // Poll input every 10ms
		.frame_time_ms = 16,   // 60fps target
		.max_frame_skip = 2
	};
	ugui_task_init(&task_config);
}

void deinit_ugui() {
	// free allocated memory
	free(ugui_instance->buffer1);
	free(ugui_instance->buffer2);

	ugui_instance = nullptr;
}
void gui_task() {
	update_input();

	// Flush display
	if (ugui_instance && ugui_instance->flush_func) {
		ugui_instance->flush_func(ugui_instance);
	}
}


void set_background_color(ugui_color color) {
	if (!ugui_instance || !ugui_instance->buffer) return;
	for (size_t i = 0; i < ugui_instance->width * ugui_instance->height; i++)
	{
		ugui_instance->buffer[i] = color;
	}
	// Mark entire display as dirty
	ugui_canvas_mark_all_dirty();
}

void draw_circle(int x0, int y0, int radius)
{
	if (!ugui_instance || !ugui_instance->buffer) return;
	int x = radius;
	int y = 0;
	int err = 0;
	
	int min_x = x0 - radius;
	int max_x = x0 + radius;
	int min_y = y0 - radius;
	int max_y = y0 + radius;
	
	while (x >= y)
	{
		// Draw the eight octants
		if (x0 + x < ugui_instance->width && y0 + y < ugui_instance->height)
			ugui_instance->buffer[(y0 + y) * ugui_instance->width + (x0 + x)] = ugui_instance->color;
		if (x0 + y < ugui_instance->width && y0 + x < ugui_instance->height)
			ugui_instance->buffer[(y0 + x) * ugui_instance->width + (x0 + y)] = ugui_instance->color;
		if (x0 - y >= 0 && y0 + x < ugui_instance->height)
			ugui_instance->buffer[(y0 + x) * ugui_instance->width + (x0 - y)] = ugui_instance->color;
		if (x0 - x >= 0 && y0 + y < ugui_instance->height)
			ugui_instance->buffer[(y0 + y) * ugui_instance->width + (x0 - x)] = ugui_instance->color;
		if (x0 - x >= 0 && y0 - y >= 0)
			ugui_instance->buffer[(y0 - y) * ugui_instance->width + (x0 - x)] = ugui_instance->color;
		if (x0 - y >= 0 && y0 - x >= 0)
			ugui_instance->buffer[(y0 - x) * ugui_instance->width + (x0 - y)] = ugui_instance->color;
		if (x0 + y < ugui_instance->width && y0 - x >= 0)
			ugui_instance->buffer[(y0 - x) * ugui_instance->width + (x0 + y)] = ugui_instance->color;
		if (x0 + x < ugui_instance->width && y0 - y >= 0)
			ugui_instance->buffer[(y0 - y) * ugui_instance->width + (x0 + x)] = ugui_instance->color;
		err += y;
		y++;
		if (2 * err >= x)
		{
			err -= x;
			x--;
		}
	}
	
	// Mark circle bounding box as dirty
	if (max_x < 0 || min_x >= (int)ugui_instance->width ||
		max_y < 0 || min_y >= (int)ugui_instance->height) {
		return;  // Circle completely off-screen
	}
	
	if (min_x < 0) min_x = 0;
	if (max_x >= (int)ugui_instance->width) max_x = ugui_instance->width - 1;
	if (min_y < 0) min_y = 0;
	if (max_y >= (int)ugui_instance->height) max_y = ugui_instance->height - 1;
	
	ugui_canvas_mark_dirty(min_x, min_y, max_x - min_x + 1, max_y - min_y + 1);
}

void ugui_swap_buffer()
{
	if (!ugui_instance || !ugui_instance->buffer2) return;
	ugui_instance->active_buffer = !ugui_instance->active_buffer;
	// Update the buffer pointer to active buffer
	ugui_instance->buffer = ugui_instance->active_buffer ? ugui_instance->buffer2 : ugui_instance->buffer1;
}




