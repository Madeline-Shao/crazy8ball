#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "sdl_wrapper.h"
#include "color.h"
#include "body.h"

const char WINDOW_TITLE[] = "CS 3";
const int WINDOW_WIDTH = 1500;
const int WINDOW_HEIGHT = 900;
const double MS_PER_S = 1e3;

/**
 * Auxiliary element for on_key.
 */
void * key_aux;

/**
 * Auxiliary element for on_mouse.
 */
void * mouse_aux;

/**
 * Auxiliary element for on_motion.
 */
void * motion_aux;

/**
 * The coordinate at the center of the screen.
 */
vector_t center;
/**
 * The coordinate difference from the center to the top right corner.
 */
vector_t max_diff;
/**
 * The SDL window where the scene is rendered.
 */
SDL_Window *window;
/**
 * The renderer used to draw the scene.
 */
SDL_Renderer *renderer;
/**
 * The keypress handler, or NULL if none has been configured.
 */
key_handler_t key_handler = NULL;
/**
 * The mouse press handler, or NULL if none has been configured.
 */
mouse_handler_t mouse_handler = NULL;

/**
 * The motion press handler, or NULL if none has been configured.
 */
motion_handler_t motion_handler = NULL;
/**
 * SDL's timestamp when a key was last pressed or released.
 * Used to mesasure how long a key has been held.
 */
uint32_t key_start_timestamp;
/**
 * The value of clock() when time_since_last_tick() was last called.
 * Initially 0.
 */
clock_t last_clock = 0;

/** Computes the center of the window in pixel coordinates */
vector_t get_window_center(void) {
    int *width = malloc(sizeof(*width)),
        *height = malloc(sizeof(*height));
    assert(width != NULL);
    assert(height != NULL);
    SDL_GetWindowSize(window, width, height);
    vector_t dimensions = {.x = *width, .y = *height};
    free(width);
    free(height);
    return vec_multiply(0.5, dimensions);
}

/**
 * Computes the scaling factor between scene coordinates and pixel coordinates.
 * The scene is scaled by the same factor in the x and y dimensions,
 * chosen to maximize the size of the scene while keeping it in the window.
 */
double get_scene_scale(vector_t window_center) {
    // Scale scene so it fits entirely in the window
    double x_scale = window_center.x / max_diff.x,
           y_scale = window_center.y / max_diff.y;
    return x_scale < y_scale ? x_scale : y_scale;
}

/** Maps a scene coordinate to a window coordinate */
vector_t get_window_position(vector_t scene_pos, vector_t window_center) {
    // Scale scene coordinates by the scaling factor
    // and map the center of the scene to the center of the window
    vector_t scene_center_offset = vec_subtract(scene_pos, center);
    double scale = get_scene_scale(window_center);
    vector_t pixel_center_offset = vec_multiply(scale, scene_center_offset);
    vector_t pixel = {
        .x = round(window_center.x + pixel_center_offset.x),
        // Flip y axis since positive y is down on the screen
        .y = round(window_center.y + pixel_center_offset.y)
    };
    return pixel;
}

/**
 * Converts an SDL key code to a char.
 * 7-bit ASCII characters are just returned
 * and arrow keys are given special character codes.
 */
char get_keycode(SDL_Keycode key) {
    switch (key) {
        case SDLK_LEFT:  return LEFT_ARROW;
        case SDLK_UP:    return UP_ARROW;
        case SDLK_RIGHT: return RIGHT_ARROW;
        case SDLK_DOWN:  return DOWN_ARROW;
        default:
            // Only process 7-bit ASCII characters
            return key == (SDL_Keycode) (char) key ? key : '\0';
    }
}

SDL_Renderer * sdl_init(vector_t min, vector_t max) {
    // Check parameters
    assert(min.x < max.x);
    assert(min.y < max.y);

    center = vec_multiply(0.5, vec_add(min, max));
    max_diff = vec_subtract(max, center);
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );
    renderer = SDL_CreateRenderer(window, -1, 0);
return renderer;
}

bool sdl_is_done(void) {
    SDL_Event *event = malloc(sizeof(*event));
    assert(event != NULL);
    while (SDL_PollEvent(event)) {
        switch (event->type) {
            case SDL_QUIT:
                free(event);
                return true;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                // Skip the keypress if no handler is configured
                // or an unrecognized key was pressed
                if (key_handler == NULL) break;
                char key = get_keycode(event->key.keysym.sym);
                if (key == '\0') break;

                uint32_t timestamp = event->key.timestamp;
                if (!event->key.repeat) {
                    key_start_timestamp = timestamp;
                }
                key_event_type_t type =
                    event->type == SDL_KEYDOWN ? KEY_PRESSED : KEY_RELEASED;
                double held_time = (timestamp - key_start_timestamp) / MS_PER_S;
                key_handler(key, type, held_time, key_aux);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                if (mouse_handler == NULL) break;
                Uint8 button = event->button.button;
                // if (key == '\0') break;

                // uint32_t timestamp = event->button.timestamp;
                // if (!event->key.repeat) {
                //     key_start_timestamp = timestamp;
                // }
                Sint32 x = event->button.x;
                Sint32 y = event->button.y;
                mouse_event_type_t m_type =
                    event->type == SDL_MOUSEBUTTONDOWN ? MOUSE_DOWN : MOUSE_UP;
                // double held_time = (timestamp - key_start_timestamp) / MS_PER_S;
                mouse_handler(button, m_type, x, y, mouse_aux);
                break;
            case SDL_MOUSEMOTION:
                if (motion_handler == NULL) break;
                motion_handler(event->motion.x, event->motion.y, event->motion.xrel, event->motion.yrel, motion_aux);

        }
    }
    free(event);
    return false;
}

void sdl_clear(void) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
}

void sdl_draw_polygon(list_t *points, rgb_color_t color) {
    // Check parameters
    size_t n = list_size(points);
    assert(n >= 3);
    assert(0 <= color.r && color.r <= 1);
    assert(0 <= color.g && color.g <= 1);
    assert(0 <= color.b && color.b <= 1);
    assert(0 <= color.a && color.a <= 1);

    vector_t window_center = get_window_center();

    // Convert each vertex to a point on screen
    int16_t *x_points = malloc(sizeof(*x_points) * n),
            *y_points = malloc(sizeof(*y_points) * n);
    assert(x_points != NULL);
    assert(y_points != NULL);
    for (size_t i = 0; i < n; i++) {
        vector_t *vertex = list_get(points, i);
        vector_t pixel = get_window_position(*vertex, window_center);
        x_points[i] = pixel.x;
        y_points[i] = pixel.y;
    }

    // Draw polygon with the given color
    filledPolygonRGBA(
        renderer,
        x_points, y_points, n,
        color.r * 255, color.g * 255, color.b * 255, color.a * 255
    );
    free(x_points);
    free(y_points);
    list_free(points);
}

void sdl_set_icon(SDL_Surface *surface){
    SDL_SetWindowIcon(window, surface);
}

void sdl_set_title(const char *title){
    SDL_SetWindowTitle(window, title);
}

void sdl_show(void) {
    // Draw boundary lines
    vector_t window_center = get_window_center();
    vector_t max = vec_add(center, max_diff),
             min = vec_subtract(center, max_diff);
    vector_t max_pixel = get_window_position(max, window_center),
             min_pixel = get_window_position(min, window_center);
    SDL_Rect *boundary = malloc(sizeof(*boundary));
    boundary->x = min_pixel.x;
    boundary->y = max_pixel.y;
    boundary->w = max_pixel.x - min_pixel.x;
    boundary->h = min_pixel.y - max_pixel.y;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, boundary);
    free(boundary);

    SDL_RenderPresent(renderer);
}

void sdl_draw_sprite(body_t *body, scene_t *scene){
    if (body_get_image(body) != NULL) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, body_get_image(body));
        SDL_Rect *boundary = malloc(sizeof(SDL_Rect));
        boundary->x = body_get_centroid(body).x - (body_get_width(body) / 2.0);
        boundary->y = body_get_centroid(body).y - (body_get_height(body) / 2.0);
        boundary->w = body_get_width(body);
        boundary->h = body_get_height(body);
        if (!strcmp(body_get_info(body), "CUE_STICK")){
            SDL_RenderCopyEx(renderer, texture, NULL, boundary, body_get_angle(body) * 180 / M_PI, NULL, SDL_FLIP_NONE);
        }
        else if(body_get_mass(body) == 0){
            sdl_draw_polygon(body_get_shape(body), body_get_color(body));
        }
        else{
            SDL_RenderCopy(renderer, texture, NULL, boundary);
        }
        SDL_DestroyTexture(texture);
        free(boundary);
    }
    if (!strcmp(body_get_info(body), "INITIAL_LINE")){
        sdl_draw_polygon(body_get_shape(body), body_get_color(body));
    }

}

void sdl_render_scene(scene_t *scene) {
    sdl_clear();
    size_t body_count = scene_bodies(scene);
    for (size_t i = 0; i < body_count; i++) {
        body_t *body = scene_get_body(scene, i);
        sdl_draw_sprite(body, scene);
    }
    sdl_show();
}

void sdl_on_key(key_handler_t k_handler, void * aux_arg) {
    key_aux = aux_arg;
    key_handler = k_handler;

}

void sdl_on_mouse(mouse_handler_t m_handler, void * aux_arg) {
    mouse_handler = m_handler;
    mouse_aux = aux_arg;
}

void sdl_on_motion(motion_handler_t m_handler, void * aux_arg) {
    motion_handler = m_handler;
    motion_aux = aux_arg;
}

double time_since_last_tick(void) {
    clock_t now = clock();
    double difference = last_clock
        ? (double) (now - last_clock) / CLOCKS_PER_SEC
        : 0.0; // return 0 the first time this is called
    last_clock = now;
    return difference;
}
