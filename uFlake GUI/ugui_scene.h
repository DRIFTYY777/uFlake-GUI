#pragma once

#include <cstdint>
#include <cstddef>

#ifdef __cplusplus
extern "C"
{
#endif

#define UGUI_MAX_SCENES 16
#define UGUI_MAX_SCENE_STACK 8

// Forward declaration for widget
typedef struct ugui_widget ugui_widget_t;

// Scene - top-level container for UI state
typedef struct {
    uint32_t id;                    // Unique scene identifier
    ugui_widget_t* root_widget;     // Root of widget tree (optional)
    
    bool is_dirty;                  // True if scene needs redraw
    bool is_active;                 // True if scene is on top of stack
    
    void* user_data;                // App-specific data
} ugui_scene_t;

// Scene stack manager
typedef struct {
    ugui_scene_t* scenes[UGUI_MAX_SCENE_STACK];
    uint8_t stack_top;              // -1 if empty, 0-7 otherwise
} ugui_scene_stack_t;

// Initialize scene manager
void ugui_scene_manager_init(void);

// Create a new scene
ugui_scene_t* ugui_scene_create(uint32_t id);

// Destroy a scene (removes from stack if present)
void ugui_scene_destroy(ugui_scene_t* scene);

// Push scene onto stack (becomes active)
void ugui_scene_push(ugui_scene_t* scene);

// Pop scene from stack
ugui_scene_t* ugui_scene_pop(void);

// Get currently active scene
ugui_scene_t* ugui_scene_get_active(void);

// Get scene at specific stack index
ugui_scene_t* ugui_scene_get_at_index(uint8_t index);

// Get stack depth
uint8_t ugui_scene_get_stack_depth(void);

// Mark scene as needing redraw
void ugui_scene_mark_dirty(ugui_scene_t* scene);

// Mark all scenes as dirty
void ugui_scene_mark_all_dirty(void);

// Check if scene needs redraw
bool ugui_scene_is_dirty(ugui_scene_t* scene);

// Clear dirty flag on scene (call after rendering)
void ugui_scene_clear_dirty(ugui_scene_t* scene);

// Set root widget for scene (widget tree rendering)
void ugui_scene_set_root_widget(ugui_scene_t* scene, ugui_widget_t* widget);

#ifdef __cplusplus
}
#endif
