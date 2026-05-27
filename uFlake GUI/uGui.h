#ifndef U_GUI_H
#define U_GUI_H

#include <cstdint>
#include <cstddef>
#include "types.h"
#include "text.h"
#include "colors.h"
#include "input.h"
#include "ugui_canvas.h"
#include "ugui_scene.h"
#include "ugui_task.h"

#ifdef __cplusplus
extern "C"
{
#endif
    void init_ugui(ugui_config *config, ugui *instance);
	void deinit_ugui();
	void gui_task();
	void set_background_color(ugui_color color);
	void draw_circle(int x0, int y0, int radius);
    void ugui_swap_buffer();
#ifdef __cplusplus
}
#endif

#endif // U_GUI_H