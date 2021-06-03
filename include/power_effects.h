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

/**
 * Resizes the balls of the current type to have the given radius.
 * @param scene - The scene containing the balls.
 * @param radius - The new radius.
 */
void add_size_powerdown(scene_t *scene, double radius);

/**
 * Sets the balls of the opponent's type to the given mass. If the given mass is zero,
 * the ball color will be set to a transparent gray, otherwise, the color will be
 * set to white.
 * @param scene - The scene containing the balls.
 * @param mass - The new mass.
 */
void add_ghost_powerup(scene_t *scene, double mass);

/**
 * Gives the opponent two extra balls, placed in random spots on the table.
 * @param scene - The scene containing the balls.
 * @param balls_colliison_handler - A collision handler to be called when the balls collide
 * with other balls.
 * @param ball_destroy - A collision handler to be called when the balls collide with a hole.
 */
void add_balls_powerup(scene_t *scene, collision_handler_t balls_collision_handler, collision_handler_t ball_destroy);
