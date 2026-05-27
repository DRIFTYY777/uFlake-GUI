#pragma once

#include <cstdint>
#include <cstddef>

#ifdef __cplusplus
extern "C"
{
#endif

// Task configuration
typedef struct {
    uint16_t input_poll_ms;      // Poll input every N milliseconds
    uint16_t frame_time_ms;      // Target frame time (60fps = 16ms)
    uint16_t max_frame_skip;     // Max frames to skip if behind
} ugui_task_config_t;

// Initialize task system with config
void ugui_task_init(const ugui_task_config_t* config);

// Main event loop - call this as your app's main loop
// Returns false if requested to quit
bool ugui_task_tick(void);

// Request quit from task loop
void ugui_task_quit(void);

// Get elapsed time since last frame (ms)
uint16_t ugui_task_get_delta_time(void);

// Get current frame count
uint64_t ugui_task_get_frame_count(void);

// Pause task loop (stop rendering)
void ugui_task_pause(void);

// Resume task loop
void ugui_task_resume(void);

// Set whether to skip rendering if no dirty regions
void ugui_task_set_force_render(bool force);

#ifdef __cplusplus
}
#endif
