#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include "body.h"
#include "list.h"

/**
 * An encapsulation of the necessary variables keeping track
 * of Crazy 8 Ball game progression.
 */
typedef struct game_state game_state_t;

/**
 * Initializes a game state object with values corresponding
 * to the beginning of a game.
 */
game_state_t *game_state_init(void);

/**
 * Frees the game state.
 * @param game_state - The game state to free
 */
void game_state_free(game_state_t *game_state);

/**
 * Gives the status of the additional balls powerup.
 *
 * @param game_state the game state whose status is returned.
 *
 * @return the status of the powerup.
 */
bool game_state_get_balls_powerup(game_state_t *game_state);

/**
 * Gives the status of the ghost balls powerup.
 *
 * @param game_state the game state whose status is returned.
 *
 * @return the status of the powerup.
 */
bool game_state_get_ghost_powerup(game_state_t *game_state);

/**
 * Gives the status of the increase size powerdown.
 *
 * @param game_state the game state whose status is returned.
 *
 * @return the status of the powerdown.
 */
bool game_state_get_size_powerdown(game_state_t *game_state);

/**
 * Gives the status of the end turn powerdown.
 *
 * @param game_state the game state whose status is returned.
 *
 * @return the status of the powerdown.
 */
bool game_state_get_turn_powerdown(game_state_t *game_state);

/**
 * Gives the player whose turn it is in the current game.
 *
 * @param game_state the game state whose turn is returned.
 *
 * @return a string corresponding to the player whose turn it is.
 */
char *game_state_get_current_type(game_state_t *game_state);

/**
 * Gives the player whose turn it is in the current game.
 *
 * @param game_state the game state whose turn is returned.
 *
 * @return the number corresponding to the player whose turn it is.
 */
size_t game_state_get_curr_player_turn(game_state_t *game_state);

/**
 * Gives whether or not the game is on the first turn.
 *
 * @param game_state the game state whose status is returned.
 *
 * @return a boolean for if the first turn is happening.
 */
bool game_state_get_first_turn(game_state_t *game_state);

/**
 * Gives the ball type of player 1.
 *
 * @param game_state the game state whose player 1 status is returned.
 *
 * @return a string corresponding to the ball type of player 1.
 */
char *game_state_get_player_1_type(game_state_t *game_state);

/**
 * Gives the ball type of player 2.
 *
 * @param game_state the game state whose player 2 status is returned.
 *
 * @return a string corresponding to the ball type of player 2.
 */
char *game_state_get_player_2_type(game_state_t *game_state);

/**
 * Gives whether or not it is the end of the current player's turn.
 *
 * @param game_state the game state whose status is returned.
 *
 * @return a boolean for whether or not the end of the turn has been reached.
 */
bool game_state_get_end_of_turn(game_state_t *game_state);

/**
 * Gives all the balls sunk during this shot.
 *
 * @param game_state the game state whose balls sunk are returned.
 *
 * @return a list containing all the sunk balls.
 */
list_t *game_state_get_balls_sunk(game_state_t *game_state);

/**
 * Gives whether or not the cue ball has been sunk this shot.
 *
 * @param game_state the game state whose cue ball status is returned.
 *
 * @return a boolean for the cue ball status.
 */
bool game_state_get_cue_ball_sunk(game_state_t *game_state);

/**
 * Gives the winner of the game.
 *
 * @param game_state the game state whose winner is returned.
 *
 * @return a string corresponding to the winner of the
 *              game; NULL if the game is ongoing.
 */
char *game_state_get_winner(game_state_t *game_state);

/**
 * Gives whether or not the game has started.
 *
 * @param game_state the game state whose start status is returned.
 *
 * @return boolean for if the game has started.
 */
bool game_state_get_game_start(game_state_t *game_state);

/**
 * Gives whether or not the instructinon page has been called.
 *
 * @param game_state the game state whose instruction status is returned.
 *
 * @return boolean for if the instruction page is open.
 */
bool game_state_get_game_instructions(game_state_t *game_state);

/**
 * Gives whether the game has been quit using one of the quit buttons.
 *
 * @param game_state the game state whose quit status is returned.
 *
 * @return boolean for if the game has been quit.
 */
bool game_state_get_game_quit(game_state_t *game_state);

/**
 * Gives the keys that have been pressed during the game.
 *
 * @param game_state the game state whose keys are returned.
 *
 * @return a list of the keys pressed during the game.
 */
list_t *game_state_get_keys(game_state_t *game_state);

/**
 * Gives whether or not the konami code has been completed
 * during this game.
 *
 * @param game_state the game state konami status is returned.
 *
 * @return a boolean for if the konami code is completed.
 */
bool game_state_get_konami(game_state_t *game_state);

/**
 * Sets the status of konami code completion
 * during this game.
 *
 * @param game_state the game state konami status is changed.
 * @param konami a boolean for the konami status
 */
void game_state_set_konami(game_state_t *game_state, bool konami);

/**
 * Sets the winner of the game.
 *
 * @param game_state the game state whose winner is set.
 * @param winner a string corresponding to the winner of the game.
 */
void game_state_set_winner(game_state_t *game_state, char *winner);

/**
 * Sets whether or not the cue ball has been sunk this shot.
 *
 * @param game_state the game state whose cue ball status is changed.
 * @param cue_ball_sunk a boolean for the cue ball status.
 */
void game_state_set_cue_ball_sunk(game_state_t *game_state, bool cue_ball_sunk);

/**
 * Sets the player whose turn it is in the current game.
 *
 * @param game_state the game state whose turn is changed.
 * @param turn the number corresponding to the player whose turn it is.
 */
void game_state_set_curr_player_turn(game_state_t *game_state, size_t turn);

/**
 * Sets whether the game is on the first turn.
 *
 * @param game_state the game state whose first turn status is changed.
 * @param first_turn whether or not it is the first turn.
 */
void game_state_set_first_turn(game_state_t *game_state, bool first_turn);

/**
 * Sets whether or not it is the end of the current player's turn.
 *
 * @param game_state the game state whose status is changed.
 * @param end_of_turn a boolean for whether or not the end of the turn has been reached.
 */
void game_state_set_end_of_turn(game_state_t *game_state, bool end_of_turn);

/**
 * Sets the ball type of player 1.
 *
 * @param game_state the game state whose player 1 status is changed.
 * @param player_1_type a string corresponding to the ball type of player 1.
 */
void game_state_set_player_1_type(game_state_t *game_state, char *player_1_type);

/**
 * Sets the ball type of player 2.
 *
 * @param game_state the game state whose player 2 status is changed.
 * @param player_1_type a string corresponding to the ball type of player 2.
 */
void game_state_set_player_2_type(game_state_t *game_state, char *player_2_type);

/**
 * Sets the status of the additional balls powerup.
 *
 * @param game_state the game state whose status is changed.
 * @param balls_powerup the status of the powerup.
 */
void game_state_set_balls_powerup(game_state_t *game_state, bool balls_powerup);

/**
 * Sets the status of the ghost balls powerup.
 *
 * @param game_state the game state whose status is changed.
 * @param balls_powerup the status of the powerup.
 */
void game_state_set_ghost_powerup(game_state_t *game_state, bool ghost_powerup);

/**
 * Sets the status of the bigger balls powerdown.
 *
 * @param game_state the game state whose status is changed.
 * @param balls_powerup the status of the powerdown.
 */
void game_state_set_size_powerdown(game_state_t *game_state, bool size_powerdown);

/**
 * Sets the status of the end turn powerdown.
 *
 * @param game_state the game state whose status is changed.
 * @param balls_powerup the status of the powerdown.
 */
void game_state_set_turn_powerdown(game_state_t *game_state, bool turn_powerdown);

/**
 * Sets whether or not the game has started.
 *
 * @param game_state the game state whose start status is changed.
 * @param game_start a boolean for if the game has started.
 */
void game_state_set_game_start(game_state_t *game_state, bool game_start);

/**
 * Sets whether or not the instructinon page has been called.
 *
 * @param game_state the game state whose instruction status is changed.
 * @param val a boolean for if the instruction page is open.
 */
void game_state_set_game_instructions(game_state_t *game_state, bool val);

/**
 * Sets whether the game has been quit using one of the quit buttons.
 *
 * @param game_state the game state whose quit status is changed.
 * @param game_quit a boolean for if the game has been quit.
 */
void game_state_set_game_quit(game_state_t *game_state, bool game_quit);
