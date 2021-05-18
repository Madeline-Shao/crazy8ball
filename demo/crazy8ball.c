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
const vector_t HIGH_RIGHT_CORNER = {1000, 700};


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

int main(){
    scene_t *scene = scene_init();

    SDL_Renderer *renderer = sdl_init(LOW_LEFT_CORNER, HIGH_RIGHT_CORNER);
    while (!sdl_is_done()){
        sdl_render_scene(scene);
        scene_tick(scene, time_since_last_tick());
        // SDL_Surface *image = IMG_Load("ditto.jpg");
        SDL_Texture *texture = IMG_LoadTexture(renderer, "ditto.jpg");
        // SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    scene_free(scene);
}
