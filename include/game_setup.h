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

/**
 * Adds the cue stick to the specified scene.
 *
 * @param scene the scene object to which the stick is added.
 */
void game_setup_add_stick(scene_t * scene);

/**
 * Adds the pool table to the specified scene.
 *
 * @param scene the scene object to which the table is added.
 */
void game_setup_add_table(scene_t *scene);

/**
 * Adds the cue stick force slider to the specified scene.
 *
 * @param scene the scene object to which the slider is added.
 */
void game_setup_add_slider(scene_t *scene);

/**
 * Adds all the balls in a pool game
 *  to the specified scene; these include
 * the solid balls, striped balls, and the cue ball.
 *
 * @param scene the scene object to which the stick is added.
 */
void game_setup_add_balls(scene_t *scene);

/**
 * Adds barrier bodies to the pool table body in a specified scene.
 *
 * @param scene the scene object to which the walls.
 */
void game_setup_add_walls(scene_t *scene);

/**
 * Adds hole bodies to the pool table in a specified scene.
 *
 * @param scene the scene object to which the holes is added.
 */
void game_setup_add_holes(scene_t *scene);

/**
 * Adds a cosmetic guideline along which the user is allowed
 * to drag the cue ball during the first turn.
 *
 * @param scene the scene object to which the line is added.
 */
void game_setup_add_initial_line(scene_t *scene);

/**
 * Adds text objects to a pool game, including 
 * player turn, the win message, and the power
 * up/down messages.
 *
 * @param scene the scene object to which text is added.
 * @param font the font of the displayed text.
 */
void game_setup_add_text(scene_t *scene, TTF_Font *font);

/**
 * Adds play button, help button, and quit button
 * in their start screen positions and shapes.
 *
 * @param scene the scene object to which the buttons are added.
 * @param font the font of the text on the buttons.
 */
void game_setup_add_start_menu(scene_t *scene, TTF_Font *font);

/**
 * Adds the help button and quit button in their game
 * positions and shapes.
 *
 * @param scene the scene object to which the buttons are added.
 */
void game_setup_add_in_game_buttons(scene_t *scene);

/**
 * Adds the backgorund picture to the specified scene.
 *
 * @param scene the scene object to which the backgorund is added.
 */
void game_setup_add_background(scene_t *scene);

/**
 * Adds ball-ball collision, stick-ball collision, and ball sink
 * sound effects.
 */
void game_setup_sound_setup();

/**
 * Adds the instruction page image to the specified scene.
 *
 * @param scene the scene object to which the 
 *              instruction page is added.
 */
void game_setup_add_instructions(scene_t *scene);

/**
 * Adds all necessary components of a game of Crazy 8 Ball
 * to the given scene.
 *
 * @param scene the scene object in which the game is set up.
 * @param font the font of the displayed messages and button text
 *              in the game.
 */
void game_setup(scene_t *scene, TTF_Font *font);