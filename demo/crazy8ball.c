#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "body.h"
#include "color.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "vector.h"
#include "forces.h"
#include "force_params.h"
#include "force_wrapper.h"
#include <string.h>
#include <SDL2/SDL_image.h>

const vector_t LOW_LEFT_CORNER = {0, 0};
const vector_t HIGH_RIGHT_CORNER = {1000, 600};


// body_t *init_stick(int pts, double radius, double mass, rgb_color_t rgb_color) {

// }

// body_t *init_balls(int pts, double radius, double mass, rgb_color_t rgb_color){

// }

// // stick force buildup, animation, and ball collision
// body_t *shoot_stick(vector_t initial_position, int direction, double width,

// }

// // stick rotation
// void player_key_handler(char key, key_event_type_t type, double held_time)

// }

list_t *rect_init(double width, double height) {
    vector_t half_width  = {.x = width / 2, .y = 0.0},
             half_height = {.x = 0.0, .y = height / 2};
    list_t *rect = list_init(4, free);
    vector_t *v = malloc(sizeof(*v));
    *v = vec_add(half_width, half_height);
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = vec_subtract(half_height, half_width);
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = vec_negate(*(vector_t *) list_get(rect, 0));
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = vec_subtract(half_width, half_height);
    list_add(rect, v);
    return rect;
}

int main(){
    scene_t *scene = scene_init();
    SDL_Renderer *renderer = sdl_init(LOW_LEFT_CORNER, HIGH_RIGHT_CORNER);
    list_t *rect = rect_init(1000, 600);
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 0);
    SDL_Surface *image = IMG_Load("demo/small.png");
    SDL_Surface *image2 = IMG_Load("demo/pool_table.png");
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_Texture *texture2 = SDL_CreateTextureFromSurface(renderer, image2);
    
    body_t *ditto_bod = body_init_with_info(rect, 10, (rgb_color_t) {0,0,0}, image2, 800, 480, NULL, free);
    body_set_centroid(ditto_bod, (vector_t) {500, 300});
    scene_add_body(scene, ditto_bod);
    
    while (!sdl_is_done()){
        sdl_render_scene(scene);
        scene_tick(scene, time_since_last_tick());
        //SDL_RenderCopy(renderer, texture2, NULL, NULL);  
        //SDL_RenderCopy(renderer, texture, NULL, NULL);
        //SDL_RenderPresent(renderer);
        // free(boundary);   
    }
    scene_free(scene);
}
