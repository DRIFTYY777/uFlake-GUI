
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
ugui gui_instance;  // Global for easy access in callbacks and draw functions

void sdl_flush(ugui *gui)
{
	SDL_UpdateTexture(texture, nullptr, gui->buffer, gui->width * sizeof(uint16_t));
	SDL_RenderClear(renderer);
	SDL_RenderTexture(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

// SDL button polling - called by update_input()
void sdl_poll_input()
{
	int num_keys = 0;
	const bool* keys = SDL_GetKeyboardState(&num_keys);

	// Map numpad keys to button codes
	// Numpad: 8=UP, 2=DOWN, 4=LEFT, 6=RIGHT, 5=ENTER, 3=BACK
	set_button_state(KEY_UP, keys[SDL_SCANCODE_KP_8] ? BTN_PRESSED : BTN_RELEASED);
	set_button_state(KEY_DOWN, keys[SDL_SCANCODE_KP_2] ? BTN_PRESSED : BTN_RELEASED);
	set_button_state(KEY_LEFT, keys[SDL_SCANCODE_KP_4] ? BTN_PRESSED : BTN_RELEASED);
	set_button_state(KEY_RIGHT, keys[SDL_SCANCODE_KP_6] ? BTN_PRESSED : BTN_RELEASED);
	set_button_state(KEY_ENTER, keys[SDL_SCANCODE_KP_5] ? BTN_PRESSED : BTN_RELEASED);
	set_button_state(KEY_BACK, keys[SDL_SCANCODE_KP_3] ? BTN_PRESSED : BTN_RELEASED);
}

uint8_t init_sdl() {
	// Initialize SDL
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		return 1;
	}

	// Create window
	window = SDL_CreateWindow("uFlake GUI", WIDTH, HEIGHT, 0);
	if (!window) {
		SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// Create renderer
	renderer = SDL_CreateRenderer(window, nullptr);
	if (!renderer) {
		SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// Create texture for display buffer
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
	if (!texture) {
		SDL_Log("SDL_CreateTexture failed: %s", SDL_GetError());
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}
}

void draw_demo();


// Demo state - interactive cursor
struct {
	int16_t x;
	int16_t y;
	bool show_menu;
	int menu_select;
} demo_state = { 150, 110, true, 0 };

// Button callback handler - responds to button presses
void on_button_press(key_code_t key, button_state_t state)
{
	// Only handle button press events (ignore release)
	if (state != BTN_PRESSED) return;

	if (demo_state.show_menu) {
		// Menu mode
		switch (key) {
		case KEY_UP:
			if (demo_state.menu_select > 0) demo_state.menu_select--;
			break;
		case KEY_DOWN:
			if (demo_state.menu_select < 3) demo_state.menu_select++;
			break;
		case KEY_ENTER:
			// Exit menu and enter interactive mode
			demo_state.show_menu = false;
			break;
		case KEY_BACK:
			// Nothing to do on back in menu
			break;
		default:
			break;
		}
	} else {
		// Interactive cursor mode
		switch (key) {
		case KEY_UP:
			if (demo_state.y > 20) demo_state.y -= 10;
			break;
		case KEY_DOWN:
			if (demo_state.y < 220) demo_state.y += 10;
			break;
		case KEY_LEFT:
			if (demo_state.x > 20) demo_state.x -= 10;
			break;
		case KEY_RIGHT:
			if (demo_state.x < 300) demo_state.x += 10;
			break;
		case KEY_ENTER:
			// Return to menu
			demo_state.show_menu = true;
			demo_state.menu_select = 0;
			break;
		case KEY_BACK:
			// Exit application
			exit(0);
			break;
		default:
			break;
		}
	}

	// Redraw display after state change
	draw_demo();
}

void draw_demo()
{
	set_background_color(WHITE);

	if (demo_state.show_menu) {
		// Draw menu
		set_font(FONT_8X8);  // 8x8 font
		set_text_color(BLACK);
		draw_text("Menu:", 10, 20);

		// Menu options
		const char* options[] = { "Option 1", "Option 2", "Option 3", "Option 4" };
		for (int i = 0; i < 4; i++) {
			set_text_color(demo_state.menu_select == i ? RED : BLACK);
			draw_text(options[i], 20, 50 + i * 30);
		}

		set_text_color(GRAY);
		set_font(FONT_8X12);  // 7x12 font
		draw_text("Use 8/2/4/6 to move", 10, 170);
		draw_text("Press 5 to select", 10, 190);
		draw_text("Press 3 for back", 10, 210);
	} else {
		// Draw interactive cursor
		set_font(FONT_8X8);  // 8x8 font
		set_text_color(BLACK);
		draw_text("Moving Cursor:", 10, 20);

		// Draw cursor box
		draw_circle(demo_state.x, demo_state.y, 8);

		// Draw position info
		char pos_text[50];
		sprintf_s(pos_text, sizeof(pos_text), "Pos: %d,%d", demo_state.x, demo_state.y);
		set_text_color(BLUE);
		draw_text(pos_text, 10, 50);

		// Draw instructions
		set_text_color(GRAY);
		set_font(FONT_8X12);  // 7x12 font
		draw_text("Use 8/2/4/6 to move", 10, 170);
		draw_text("Press 5 to show menu", 10, 190);
		draw_text("Press 3 to exit", 10, 210);
	}

	// Flush display
	gui_instance.flush_func(&gui_instance);
}

int main()
{
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);

	init_sdl();

	// Allocate all memory controlled by user
	uint16_t* buffer1 = (uint16_t*)malloc(WIDTH * HEIGHT * sizeof(uint16_t));
	uint16_t* buffer2 = (uint16_t*)malloc(WIDTH * HEIGHT * sizeof(uint16_t));

	// Initialize GUI with config
	ugui_config config = {
		.width = WIDTH,
		.height = HEIGHT,
		.flush_func = sdl_flush,
		.buffer1 = buffer1,
		.buffer2 = buffer2
	};
	init_ugui(&config, &gui_instance);

	// Initialize input system with SDL polling
	init_input(sdl_poll_input);

	// Register button callback for interactive demo
	register_button_callback(on_button_press);

	// Draw initial demo
	draw_demo();

	// Event loop
	bool running = true;
	SDL_Event event;

	while (running) {
		// Poll input every frame
		update_input();

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				running = false;
			}
		}
	}

	// Cleanup
	//free(buffer1);
	//free(buffer2);
	deinit_ugui();
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}