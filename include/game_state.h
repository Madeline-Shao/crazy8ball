#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include "body.h"
#include "list.h"

typedef struct game_state game_state_t;

game_state_t *game_state_init(void);

bool game_state_get_balls_powerup(game_state_t *game_state);

bool game_state_get_ghost_powerup(game_state_t *game_state);

bool game_state_get_size_powerdown(game_state_t *game_state);

bool game_state_get_turn_powerdown(game_state_t *game_state);

char *game_state_get_current_type(game_state_t *game_state);

size_t game_state_get_curr_player_turn(game_state_t *game_state);

bool game_state_get_first_turn(game_state_t *game_state);

char *game_state_get_player_1_type(game_state_t *game_state);

char *game_state_get_player_2_type(game_state_t *game_state);

bool game_state_get_end_of_turn(game_state_t *game_state);

list_t *game_state_get_balls_sunk(game_state_t *game_state);

bool game_state_get_cue_ball_sunk(game_state_t *game_state);

char *game_state_get_winner(game_state_t *game_state);

bool game_state_get_game_start(game_state_t *game_state);

bool game_state_get_game_instructions(game_state_t *game_state);

bool game_state_get_game_quit(game_state_t *game_state);

list_t *game_state_get_keys(game_state_t *game_state);

bool game_state_get_konami(game_state_t *game_state);

void game_state_set_konami(game_state_t *game_state, bool konami);

void game_state_set_winner(game_state_t *game_state, char *winner);

void game_state_set_cue_ball_sunk(game_state_t *game_state, bool cue_ball_sunk);

void game_state_set_curr_player_turn(game_state_t *game_state, size_t turn);

void game_state_set_first_turn(game_state_t *game_state, bool first_turn);

void game_state_set_end_of_turn(game_state_t *game_state, bool end_of_turn);

void game_state_set_player_1_type(game_state_t *game_state, char *player_1_type);

void game_state_set_player_2_type(game_state_t *game_state, char *player_2_type);

void game_state_set_balls_powerup(game_state_t *game_state, bool balls_powerup);

void game_state_set_ghost_powerup(game_state_t *game_state, bool ghost_powerup);

void game_state_set_size_powerdown(game_state_t *game_state, bool size_powerdown);

void game_state_set_turn_powerdown(game_state_t *game_state, bool turn_powerdown);

void game_state_set_game_start(game_state_t *game_state, bool game_start);

void game_state_set_game_instructions(game_state_t *game_state, bool val);

void game_state_set_game_quit(game_state_t *game_state, bool game_quit);
