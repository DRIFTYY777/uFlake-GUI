#include "uGui.h"

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
	ugui_instance->active_buffer = 0;

	// Initialize default font (8x8)
	ugui_instance->font_width = 8;
	ugui_instance->font_height = 8;
	ugui_instance->font_data = font_8x8_data;

	// Set buffer pointer to active buffer (buffer1 initially)
	ugui_instance->buffer = ugui_instance->buffer1;
}

void deinit_ugui() {
	// free allocated memory
	free(ugui_instance->buffer1);
	free(ugui_instance->buffer2);

	ugui_instance = nullptr;
}
void gui_task() {
	update_input();
}


void set_background_color(ugui_color color) {
	if (!ugui_instance || !ugui_instance->buffer) return;
	for (size_t i = 0; i < ugui_instance->width * ugui_instance->height; i++)
	{
		ugui_instance->buffer[i] = color;
	}
}

void draw_circle(int x0, int y0, int radius)
{
	if (!ugui_instance || !ugui_instance->buffer) return;
	int x = radius;
	int y = 0;
	int err = 0;
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
}

void ugui_swap_buffer()
{
	if (!ugui_instance || !ugui_instance->buffer2) return;
	ugui_instance->active_buffer = !ugui_instance->active_buffer;
	// Update the buffer pointer to active buffer
	ugui_instance->buffer = ugui_instance->active_buffer ? ugui_instance->buffer2 : ugui_instance->buffer1;
}




