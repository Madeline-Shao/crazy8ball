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
const double CIRCLE_POINTS = 30;
const double BALL_RADIUS = 15;
const double TABLE_MASS = INFINITY;
const double BALL_MASS = 10;


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

list_t *circle_init(double radius){
    list_t *circle_list = list_init(CIRCLE_POINTS, (free_func_t) free);
    for (size_t i = 0; i < CIRCLE_POINTS - 1; i++) {
        double angle = 2 * M_PI * i / CIRCLE_POINTS;
        vector_t *v = malloc(sizeof(vector_t));
        *v = (vector_t) {radius * cos(angle), radius * sin(angle)};
        list_add(circle_list, (void *) v);
    }
    return circle_list;
}

void game_setup(scene_t *scene){
    list_t *rect = rect_init(1000, 600);
    SDL_Surface *table_image = IMG_Load("images/pool_table.png");
    body_t *pool_table = body_init_with_info(rect, TABLE_MASS, (rgb_color_t) {0,0,0}, table_image, 800, 480, "POOL_TABLE", (free_func_t) free);
    body_set_centroid(pool_table, (vector_t) {500, 300});
    scene_add_body(scene, pool_table);

    for(int i = 1; i <= 16; i++){
        char name[20];
        snprintf(name, 20, "images/ball_%d.png", i);
        SDL_Surface *ball_image = IMG_Load(name);
        body_t *pool_ball = body_init_with_info(circle_init(BALL_RADIUS), BALL_MASS, (rgb_color_t) {0,0,0}, ball_image, 
                                        2*BALL_RADIUS, 2*BALL_RADIUS, "POOL_BALL", (free_func_t) free);
        body_set_centroid(pool_ball, (vector_t) {i*10, i*15});
        scene_add_body(scene, pool_ball);
    }

}



int main(){
    scene_t *scene = scene_init();
    game_setup(scene);
    SDL_Renderer *renderer = sdl_init(LOW_LEFT_CORNER, HIGH_RIGHT_CORNER);
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 0);
    
    while (!sdl_is_done()){
        sdl_render_scene(scene);
        scene_tick(scene, time_since_last_tick());  
    }
    scene_free(scene);
}
