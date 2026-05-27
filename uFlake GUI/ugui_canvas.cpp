#include "ugui_canvas.h"
#include "uGui.h"

static ugui_canvas_t g_canvas = {0};

void ugui_canvas_init(uint16_t width, uint16_t height)
{
    g_canvas.display_width = width;
    g_canvas.display_height = height;
    g_canvas.buffer = nullptr;
    g_canvas.is_dirty = false;
    g_canvas.is_active = false;
    
    // Initialize dirty rect to nothing
    g_canvas.dirty_rect.x = 0;
    g_canvas.dirty_rect.y = 0;
    g_canvas.dirty_rect.width = 0;
    g_canvas.dirty_rect.height = 0;
}

ugui_canvas_t* ugui_canvas_begin(void)
{
    if (g_canvas.is_active) {
        return nullptr;  // Already acquired
    }
    
    if (!ugui_instance || !ugui_instance->buffer) {
        return nullptr;  // GUI not initialized
    }
    
    g_canvas.is_active = true;
    g_canvas.buffer = ugui_instance->buffer;
    g_canvas.is_dirty = false;
    
    // Reset dirty rect
    g_canvas.dirty_rect.x = 0;
    g_canvas.dirty_rect.y = 0;
    g_canvas.dirty_rect.width = 0;
    g_canvas.dirty_rect.height = 0;
    
    return &g_canvas;
}

void ugui_canvas_end(void)
{
    if (!g_canvas.is_active) return;
    
    g_canvas.is_active = false;
    g_canvas.buffer = nullptr;
    
    // is_dirty and dirty_rect persist for flush operations
}

ugui_canvas_t* ugui_canvas_get(void)
{
    return &g_canvas;
}

void ugui_canvas_mark_dirty(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
    if (w == 0 || h == 0) return;
    
    // Clamp to display bounds
    if (x < 0) {
        w += x;
        x = 0;
    }
    if (y < 0) {
        h += y;
        y = 0;
    }
    
    int16_t right = x + w;
    int16_t bottom = y + h;
    
    if (right > g_canvas.display_width) {
        right = g_canvas.display_width;
    }
    if (bottom > g_canvas.display_height) {
        bottom = g_canvas.display_height;
    }
    
    // If first dirty region, just set it
    if (!g_canvas.is_dirty) {
        g_canvas.dirty_rect.x = x;
        g_canvas.dirty_rect.y = y;
        g_canvas.dirty_rect.width = right - x;
        g_canvas.dirty_rect.height = bottom - y;
        g_canvas.is_dirty = true;
        return;
    }
    
    // Union with existing dirty rect
    int16_t dirty_right = g_canvas.dirty_rect.x + g_canvas.dirty_rect.width;
    int16_t dirty_bottom = g_canvas.dirty_rect.y + g_canvas.dirty_rect.height;
    
    // Expand dirty rect to include new region
    int16_t new_x = (x < g_canvas.dirty_rect.x) ? x : g_canvas.dirty_rect.x;
    int16_t new_y = (y < g_canvas.dirty_rect.y) ? y : g_canvas.dirty_rect.y;
    int16_t new_right = (right > dirty_right) ? right : dirty_right;
    int16_t new_bottom = (bottom > dirty_bottom) ? bottom : dirty_bottom;
    
    g_canvas.dirty_rect.x = new_x;
    g_canvas.dirty_rect.y = new_y;
    g_canvas.dirty_rect.width = new_right - new_x;
    g_canvas.dirty_rect.height = new_bottom - new_y;
}

void ugui_canvas_mark_all_dirty(void)
{
    g_canvas.dirty_rect.x = 0;
    g_canvas.dirty_rect.y = 0;
    g_canvas.dirty_rect.width = g_canvas.display_width;
    g_canvas.dirty_rect.height = g_canvas.display_height;
    g_canvas.is_dirty = true;
}

void ugui_canvas_clear_dirty(void)
{
    g_canvas.is_dirty = false;
    g_canvas.dirty_rect.x = 0;
    g_canvas.dirty_rect.y = 0;
    g_canvas.dirty_rect.width = 0;
    g_canvas.dirty_rect.height = 0;
}

ugui_rect_t ugui_canvas_get_dirty_rect(void)
{
    return g_canvas.dirty_rect;
}

bool ugui_canvas_is_dirty(void)
{
    return g_canvas.is_dirty;
}
