#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include "body.h"
#include "list.h"

typedef struct game_state game_state_t;

game_state_t *game_state_init(void);

char *game_state_get_current_type(game_state_t *game_state);

size_t game_state_get_curr_player_turn(game_state_t *game_state);

bool game_state_get_first_turn(game_state_t *game_state);

char *game_state_get_player_1_type(game_state_t *game_state);

char *game_state_get_player_2_type(game_state_t *game_state);

bool game_state_get_end_of_turn(game_state_t *game_state);

list_t *game_state_get_balls_sunk(game_state_t *game_state);

void game_state_set_curr_player_turn(game_state_t *game_state, size_t turn);

void game_state_set_first_turn(game_state_t *game_state, bool first_turn);

void game_state_set_end_of_turn(game_state_t *game_state, bool end_of_turn);

void game_state_set_player_1_type(game_state_t *game_state, char *player_1_type);

void game_state_set_player_2_type(game_state_t *game_state, char *player_2_type);