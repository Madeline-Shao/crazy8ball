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
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

void game_setup_add_stick(scene_t * scene);

void game_setup_add_table(scene_t *scene);

void game_setup_add_slider(scene_t *scene);

void game_setup_add_balls(scene_t *scene);

void game_setup_add_walls(scene_t *scene);

void game_setup_add_holes(scene_t *scene);

void game_setup_add_initial_line(scene_t *scene);

void game_setup_add_text(scene_t *scene, TTF_Font *font);

void game_setup_add_start_menu(scene_t *scene, TTF_Font *font);

void game_setup_add_in_game_buttons(scene_t *scene);

void game_setup_add_background(scene_t *scene);

void game_setup_sound_setup();

void game_setup_add_instructions(scene_t *scene);

void game_setup(scene_t *scene, TTF_Font *font);