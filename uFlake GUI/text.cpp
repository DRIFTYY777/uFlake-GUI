#include "text.h"
#include "fonts.h"

// Font pointers from fonts.h
extern const uint8_t* font_8x8_data;
extern const uint8_t* font_7x12_data;
extern const uint8_t* font_8x12_data;
extern const uint8_t* font_8x14_data;

void set_font(font_size_t size)
{
	if (!ugui_instance) return;

	
	if (size == 0)  // FONT_8X8
	{
		ugui_instance->font_width = 8;
		ugui_instance->font_height = 8;
		ugui_instance->font_data = font_8x8_data;
	}
	else if (size == 1)  // FONT_7X12
	{
		ugui_instance->font_width = 7;
		ugui_instance->font_height = 12;
		ugui_instance->font_data = font_7x12_data;
	}
	else if (size == 2)  // FONT_8X12
	{
		ugui_instance->font_width = 8;
		ugui_instance->font_height = 12;
		ugui_instance->font_data = font_8x12_data;
	}
	else if (size == 3)  // FONT_8X14
	{
		ugui_instance->font_width = 8;
		ugui_instance->font_height = 14;
		ugui_instance->font_data = font_8x14_data;
	}
	else
	{
		ugui_instance->font_width = 8;
		ugui_instance->font_height = 8;
		ugui_instance->font_data = font_8x8_data;
	}
}

void draw_text(const char* text, int x, int y)
{
	if (!ugui_instance || !text || !ugui_instance->buffer) return;
	if (!ugui_instance->font_data) return;

	while (*text)
	{
		unsigned char c = *text++;
		if (c < 32 || c > 127) continue;

		const uint8_t* glyph = ugui_instance->font_data + (c - 32) * ugui_instance->font_height;

		for (int dy = 0; dy < ugui_instance->font_height; dy++)
		{
			uint8_t row = glyph[dy];
			for (int dx = 0; dx < ugui_instance->font_width; dx++)
			{
				if (row & (0x80 >> dx))
				{
					int px = x + dx;
					int py = y + dy;
					if (px >= 0 && px < ugui_instance->width &&
						py >= 0 && py < ugui_instance->height)
					{
						ugui_instance->buffer[py * ugui_instance->width + px] = ugui_instance->color;
					}
				}
			}
		}
		x += ugui_instance->font_width;
	}
}

void set_text_color(ugui_color color)
{
	if (ugui_instance)
		ugui_instance->color = color;
}

void set_text_bg_color(ugui_color color)
{
	// Optional: background color for text
}


