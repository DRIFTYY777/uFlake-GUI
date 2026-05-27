#pragma once

#include <cstdint>
#include <cstddef>
#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Dirty rectangle tracking
typedef struct {
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
} ugui_rect_t;

// Canvas - scoped display access with automatic dirty-rect tracking
typedef struct {
    uint16_t* buffer;        // Points to active framebuffer
    uint16_t display_width;
    uint16_t display_height;
    
    // Dirty rect tracking
    ugui_rect_t dirty_rect;  // Bounding box of all changes
    bool is_dirty;           // True if any changes occurred
    
    // State
    bool is_active;          // True if canvas is currently acquired
} ugui_canvas_t;

// Initialize canvas (call once at startup)
void ugui_canvas_init(uint16_t width, uint16_t height);

// Acquire exclusive canvas access - must call before drawing
// Returns pointer to canvas, or NULL if already acquired
ugui_canvas_t* ugui_canvas_begin(void);

// Release canvas and mark region for flushing
// Call after all drawing operations complete
void ugui_canvas_end(void);

// Get current active canvas (safe to call anytime)
ugui_canvas_t* ugui_canvas_get(void);

// Mark a rectangular region as dirty (unions with existing dirty-rect)
void ugui_canvas_mark_dirty(int16_t x, int16_t y, uint16_t w, uint16_t h);

// Mark entire display as dirty
void ugui_canvas_mark_all_dirty(void);

// Clear dirty-rect tracking (call after flush)
void ugui_canvas_clear_dirty(void);

// Get current dirty rect
ugui_rect_t ugui_canvas_get_dirty_rect(void);

// Check if canvas has pending dirty regions
bool ugui_canvas_is_dirty(void);

#ifdef __cplusplus
}
#endif
