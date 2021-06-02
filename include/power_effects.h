#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "polygon.h"
#include "body.h"
#include "color.h"
#include "vector.h"
#include "list.h"
#include "scene.h"
#include "game_util.h"
#include "forces.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

void add_size_powerdown(scene_t *scene, double radius);

void add_ghost_powerup(scene_t *scene, double mass);

void add_balls_powerup(scene_t *scene, collision_handler_t balls_collision_handler, collision_handler_t ball_destroy){
