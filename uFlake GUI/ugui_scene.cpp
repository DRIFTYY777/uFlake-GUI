#include "ugui_scene.h"
#include "ugui_canvas.h"

static ugui_scene_stack_t g_scene_stack = {0};

void ugui_scene_manager_init(void)
{
    g_scene_stack.stack_top = 0;
    for (int i = 0; i < UGUI_MAX_SCENE_STACK; i++) {
        g_scene_stack.scenes[i] = nullptr;
    }
}

ugui_scene_t* ugui_scene_create(uint32_t id)
{
    ugui_scene_t* scene = (ugui_scene_t*)malloc(sizeof(ugui_scene_t));
    if (!scene) return nullptr;
    
    scene->id = id;
    scene->root_widget = nullptr;
    scene->is_dirty = true;  // New scenes are dirty
    scene->is_active = false;
    scene->user_data = nullptr;
    
    return scene;
}

void ugui_scene_destroy(ugui_scene_t* scene)
{
    if (!scene) return;
    
    // Remove from stack if present
    for (int i = 0; i < UGUI_MAX_SCENE_STACK; i++) {
        if (g_scene_stack.scenes[i] == scene) {
            // Shift remaining scenes down
            for (int j = i; j < g_scene_stack.stack_top - 1; j++) {
                g_scene_stack.scenes[j] = g_scene_stack.scenes[j + 1];
            }
            g_scene_stack.scenes[g_scene_stack.stack_top - 1] = nullptr;
            if (g_scene_stack.stack_top > 0) {
                g_scene_stack.stack_top--;
            }
            break;
        }
    }
    
    free(scene);
}

void ugui_scene_push(ugui_scene_t* scene)
{
    if (!scene || g_scene_stack.stack_top >= UGUI_MAX_SCENE_STACK) {
        return;  // Stack full or invalid scene
    }
    
    // Deactivate current top scene
    if (g_scene_stack.stack_top > 0) {
        ugui_scene_t* prev = g_scene_stack.scenes[g_scene_stack.stack_top - 1];
        if (prev) prev->is_active = false;
    }
    
    // Push new scene
    g_scene_stack.scenes[g_scene_stack.stack_top] = scene;
    scene->is_active = true;
    scene->is_dirty = true;  // Mark as dirty when activated
    
    // Mark display dirty to force redraw on scene change
    ugui_canvas_mark_all_dirty();
    
    g_scene_stack.stack_top++;
}

ugui_scene_t* ugui_scene_pop(void)
{
    if (g_scene_stack.stack_top == 0) {
        return nullptr;  // Stack empty
    }
    
    ugui_scene_t* popped = g_scene_stack.scenes[g_scene_stack.stack_top - 1];
    g_scene_stack.scenes[g_scene_stack.stack_top - 1] = nullptr;
    g_scene_stack.stack_top--;
    
    if (popped) {
        popped->is_active = false;
    }
    
    // Reactivate previous scene if any
    if (g_scene_stack.stack_top > 0) {
        ugui_scene_t* prev = g_scene_stack.scenes[g_scene_stack.stack_top - 1];
        if (prev) {
            prev->is_active = true;
            prev->is_dirty = true;  // Mark dirty on reactivation
        }
    }
    
    // Mark display dirty for transition
    ugui_canvas_mark_all_dirty();
    
    return popped;
}

ugui_scene_t* ugui_scene_get_active(void)
{
    if (g_scene_stack.stack_top == 0) {
        return nullptr;
    }
    return g_scene_stack.scenes[g_scene_stack.stack_top - 1];
}

ugui_scene_t* ugui_scene_get_at_index(uint8_t index)
{
    if (index >= g_scene_stack.stack_top) {
        return nullptr;
    }
    return g_scene_stack.scenes[index];
}

uint8_t ugui_scene_get_stack_depth(void)
{
    return g_scene_stack.stack_top;
}

void ugui_scene_mark_dirty(ugui_scene_t* scene)
{
    if (scene) {
        scene->is_dirty = true;
        ugui_canvas_mark_all_dirty();  // Full redraw needed
    }
}

void ugui_scene_mark_all_dirty(void)
{
    for (int i = 0; i < g_scene_stack.stack_top; i++) {
        if (g_scene_stack.scenes[i]) {
            g_scene_stack.scenes[i]->is_dirty = true;
        }
    }
    ugui_canvas_mark_all_dirty();
}

bool ugui_scene_is_dirty(ugui_scene_t* scene)
{
    return scene ? scene->is_dirty : false;
}

void ugui_scene_clear_dirty(ugui_scene_t* scene)
{
    if (scene) {
        scene->is_dirty = false;
    }
}

void ugui_scene_set_root_widget(ugui_scene_t* scene, ugui_widget_t* widget)
{
    if (scene) {
        scene->root_widget = widget;
        scene->is_dirty = true;
    }
}
