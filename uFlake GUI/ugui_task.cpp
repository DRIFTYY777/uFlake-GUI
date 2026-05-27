#include "ugui_task.h"
#include "ugui_canvas.h"
#include "ugui_scene.h"
#include "uGui.h"

#include <chrono>

typedef struct {
    bool running;
    bool paused;
    bool force_render;
    
    uint16_t input_poll_ms;
    uint16_t frame_time_ms;
    uint16_t max_frame_skip;
    
    uint16_t delta_time;
    uint64_t frame_count;
    
    std::chrono::steady_clock::time_point last_input_time;
    std::chrono::steady_clock::time_point last_frame_time;
    std::chrono::steady_clock::time_point frame_start_time;
} ugui_task_state_t;

static ugui_task_state_t g_task = {0};

void ugui_task_init(const ugui_task_config_t* config)
{
    g_task.running = true;
    g_task.paused = false;
    g_task.force_render = true;  // Force first render
    
    if (config) {
        g_task.input_poll_ms = config->input_poll_ms > 0 ? config->input_poll_ms : 10;
        g_task.frame_time_ms = config->frame_time_ms > 0 ? config->frame_time_ms : 16;  // 60fps
        g_task.max_frame_skip = config->max_frame_skip > 0 ? config->max_frame_skip : 2;
    } else {
        g_task.input_poll_ms = 10;
        g_task.frame_time_ms = 16;
        g_task.max_frame_skip = 2;
    }
    
    g_task.delta_time = 0;
    g_task.frame_count = 0;
    g_task.last_input_time = std::chrono::steady_clock::now();
    g_task.last_frame_time = std::chrono::steady_clock::now();
}

static void render_scene(ugui_scene_t* scene)
{
    if (!scene) return;
    
    // Acquire canvas for drawing
    ugui_canvas_t* canvas = ugui_canvas_begin();
    if (!canvas) return;
    
    // TODO: When widgets are implemented, render widget tree here
    // For now, scene just holds data - app-level rendering happens in on_scene_render callbacks
    
    ugui_canvas_end();
}

bool ugui_task_tick(void)
{
    if (!g_task.running) {
        return false;
    }
    
    auto now = std::chrono::steady_clock::now();
    
    // === INPUT POLLING ===
    auto input_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - g_task.last_input_time).count();
    
    if (input_elapsed >= g_task.input_poll_ms) {
        if (ugui_instance && ugui_instance->poll_func) {
            ugui_instance->poll_func();  // Call platform-specific poll
        }
        update_input();  // Process button state changes
        g_task.last_input_time = now;
    }
    
    // === RENDER TIMING ===
    auto frame_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - g_task.last_frame_time).count();
    
    if (frame_elapsed >= g_task.frame_time_ms) {
        g_task.frame_start_time = now;
        g_task.delta_time = (uint16_t)frame_elapsed;
        g_task.frame_count++;
        
        // === RENDER ===
        ugui_scene_t* active_scene = ugui_scene_get_active();
        
        if (active_scene && (active_scene->is_dirty || g_task.force_render)) {
            render_scene(active_scene);
            ugui_scene_clear_dirty(active_scene);
        }
        
        // === FLUSH ===
        if (ugui_canvas_is_dirty() || g_task.force_render) {
            if (ugui_instance && ugui_instance->flush_func) {
                ugui_instance->flush_func(ugui_instance);
            }
            ugui_canvas_clear_dirty();
        }
        
        g_task.force_render = false;
        g_task.last_frame_time = now;
    }
    
    return true;
}

void ugui_task_quit(void)
{
    g_task.running = false;
}

uint16_t ugui_task_get_delta_time(void)
{
    return g_task.delta_time;
}

uint64_t ugui_task_get_frame_count(void)
{
    return g_task.frame_count;
}

void ugui_task_pause(void)
{
    g_task.paused = true;
}

void ugui_task_resume(void)
{
    g_task.paused = false;
    g_task.force_render = true;
}

void ugui_task_set_force_render(bool force)
{
    g_task.force_render = force;
}
