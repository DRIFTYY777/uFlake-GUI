
#include <SDL3/SDL.h>

#include "uGui.h"
#include "input.h"

#include <windows.h>
#include <cstdio>

#define WIDTH 320
#define HEIGHT 240

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* texture = nullptr;
ugui gui_instance;

// Test scene data
struct {
	int16_t rect_x;
	int16_t rect_y;
	uint16_t rect_w;
	uint16_t rect_h;
	ugui_color rect_color;
	bool show_info;
} test_scene = { 50, 30, 100, 80, BLUE, true };

void sdl_flush(ugui *gui)
{
	// Only flush dirty region for efficiency
	ugui_rect_t dirty = ugui_canvas_get_dirty_rect();
	
	if (!ugui_canvas_is_dirty()) {
		return;  // Nothing changed, skip flush
	}

	// For simplicity, flush entire buffer (in real esp32, would use dirty rect)
	SDL_UpdateTexture(texture, nullptr, gui->buffer, gui->width * sizeof(uint16_t));
	SDL_RenderClear(renderer);
	SDL_RenderTexture(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
	
	// Debug: show dirty rect info
	// printf("Flushed dirty rect: (%d, %d) %dx%d\n", dirty.x, dirty.y, dirty.width, dirty.height);
}

void sdl_poll_input()
{
	int num_keys = 0;
	const bool* keys = SDL_GetKeyboardState(&num_keys);

	set_button_state(KEY_UP, keys[SDL_SCANCODE_KP_8] ? BTN_PRESSED : BTN_RELEASED);
	set_button_state(KEY_DOWN, keys[SDL_SCANCODE_KP_2] ? BTN_PRESSED : BTN_RELEASED);
	set_button_state(KEY_LEFT, keys[SDL_SCANCODE_KP_4] ? BTN_PRESSED : BTN_RELEASED);
	set_button_state(KEY_RIGHT, keys[SDL_SCANCODE_KP_6] ? BTN_PRESSED : BTN_RELEASED);
	set_button_state(KEY_ENTER, keys[SDL_SCANCODE_KP_5] ? BTN_PRESSED : BTN_RELEASED);
	set_button_state(KEY_BACK, keys[SDL_SCANCODE_KP_3] ? BTN_PRESSED : BTN_RELEASED);
}

uint8_t init_sdl() {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow("uFlake GUI - Retained Mode", WIDTH, HEIGHT, 0);
	if (!window) {
		SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	renderer = SDL_CreateRenderer(window, nullptr);
	if (!renderer) {
		SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
	if (!texture) {
		SDL_Log("SDL_CreateTexture failed: %s", SDL_GetError());
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}
	
	return 0;
}

// Draw a filled rectangle (simple demo primitive)
void draw_rect_filled(int x, int y, int w, int h, ugui_color color)
{
	if (!gui_instance.buffer) return;
	
	for (int dy = 0; dy < h; dy++) {
		for (int dx = 0; dx < w; dx++) {
			int px = x + dx;
			int py = y + dy;
			if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
				gui_instance.buffer[py * WIDTH + px] = color;
			}
		}
	}
	
	ugui_canvas_mark_dirty(x, y, w, h);
}

// Input handler
void on_button_press(key_code_t key, button_state_t state)
{
	if (state != BTN_PRESSED) return;
	
	switch (key) {
	case KEY_UP:
		if (test_scene.rect_y > 0) test_scene.rect_y -= 5;
		ugui_scene_mark_dirty(ugui_scene_get_active());
		break;
	case KEY_DOWN:
		if (test_scene.rect_y < HEIGHT - test_scene.rect_h) test_scene.rect_y += 5;
		ugui_scene_mark_dirty(ugui_scene_get_active());
		break;
	case KEY_LEFT:
		if (test_scene.rect_x > 0) test_scene.rect_x -= 5;
		ugui_scene_mark_dirty(ugui_scene_get_active());
		break;
	case KEY_RIGHT:
		if (test_scene.rect_x < WIDTH - test_scene.rect_w) test_scene.rect_x += 5;
		ugui_scene_mark_dirty(ugui_scene_get_active());
		break;
	case KEY_ENTER:
		// Cycle color
		if (test_scene.rect_color == BLUE) test_scene.rect_color = RED;
		else if (test_scene.rect_color == RED) test_scene.rect_color = GREEN;
		else test_scene.rect_color = BLUE;
		ugui_scene_mark_dirty(ugui_scene_get_active());
		break;
	case KEY_BACK:
		test_scene.show_info = !test_scene.show_info;
		ugui_scene_mark_dirty(ugui_scene_get_active());
		break;
	default:
		break;
	}
}

// Render function - called when scene is dirty
void render_test_scene()
{
	// Get canvas
	ugui_canvas_t* canvas = ugui_canvas_begin();
	if (!canvas) return;
	
	// Clear background
	set_background_color(WHITE);
	
	// Draw moving rectangle
	set_text_color(test_scene.rect_color);
	draw_rect_filled(test_scene.rect_x, test_scene.rect_y, 
	                 test_scene.rect_w, test_scene.rect_h, 
	                 test_scene.rect_color);
	
	// Draw info text
	if (test_scene.show_info) {
		set_font(FONT_8X8);
		set_text_color(BLACK);
		
		draw_text("Retained Mode Test", 10, 10);
		
		char pos_text[60];
		sprintf_s(pos_text, sizeof(pos_text), "Rect: %d,%d Size: %dx%d", 
		          test_scene.rect_x, test_scene.rect_y,
		          test_scene.rect_w, test_scene.rect_h);
		draw_text(pos_text, 10, 200);
		
		set_text_color(GRAY);
		draw_text("8/2/4/6=Move 5=Color 3=Help", 10, 220);
	}
	
	ugui_canvas_end();
}

int main()
{
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);

	if (init_sdl() != 0) {
		return 1;
	}

	// Allocate display buffers
	uint16_t* buffer1 = (uint16_t*)malloc(WIDTH * HEIGHT * sizeof(uint16_t));
	uint16_t* buffer2 = (uint16_t*)malloc(WIDTH * HEIGHT * sizeof(uint16_t));

	// Initialize GUI
	ugui_config config = {
		.width = WIDTH,
		.height = HEIGHT,
		.flush_func = sdl_flush,
		.poll_func = sdl_poll_input,
		.buffer1 = buffer1,
		.buffer2 = buffer2
	};
	init_ugui(&config, &gui_instance);

	// Create and push test scene
	ugui_scene_t* test_scene_obj = ugui_scene_create(1);
	ugui_scene_push(test_scene_obj);

	// Register input callback
	register_button_callback(on_button_press);

	// Event loop - using retained mode
	bool running = true;
	SDL_Event event;

	// Mark initial dirty to force first render
	ugui_task_set_force_render(true);

	while (running) {
		// Check if active scene is dirty, render if needed
		ugui_scene_t* active = ugui_scene_get_active();
		if (active && active->is_dirty) {
			render_test_scene();
		}

		// Run task tick (handles input polling and flushing)
		if (!ugui_task_tick()) {
			running = false;
		}

		// SDL event handling
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				running = false;
			}
		}
	}

	// Cleanup
	free(buffer1);
	free(buffer2);
	deinit_ugui();
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
