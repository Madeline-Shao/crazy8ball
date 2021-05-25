#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include "body.h"
#include "list.h"
#include "game_state.h"

typedef struct game_state{
    size_t curr_player_turn;
    bool first_turn;
    list_t *balls_sunk;
    char *player_1_type;
    char *player_2_type;
    bool end_of_turn;
    bool cue_ball_sunk;
    char *winner;
} game_state_t;

game_state_t *game_state_init(void){
    game_state_t *new_game_state = malloc(sizeof(game_state_t));
    assert(new_game_state != NULL);
    new_game_state->curr_player_turn = 1;
    new_game_state->first_turn = true;
    new_game_state->player_1_type = NULL;
    new_game_state->player_2_type = NULL;
    new_game_state->end_of_turn = false;
    list_t *balls_sunk = list_init(1, (free_func_t) body_free);
    new_game_state->balls_sunk = balls_sunk;
    new_game_state->cue_ball_sunk = false;
    new_game_state->winner = NULL;
    return new_game_state;
}

char *game_state_get_current_type(game_state_t *game_state){
    if (game_state->curr_player_turn == 1) {
        return game_state->player_1_type;
    }
    else {
        return game_state->player_2_type;
    }
}

size_t game_state_get_curr_player_turn(game_state_t *game_state){
    return game_state->curr_player_turn;
}

bool game_state_get_first_turn(game_state_t *game_state){
    return game_state->first_turn;
}

char *game_state_get_player_1_type(game_state_t *game_state){
    return game_state->player_1_type;
}

char *game_state_get_player_2_type(game_state_t *game_state){
    return game_state->player_2_type;
}

bool game_state_get_end_of_turn(game_state_t *game_state){
    return game_state->end_of_turn;
}

list_t *game_state_get_balls_sunk(game_state_t *game_state){
    return game_state->balls_sunk;
}

bool game_state_get_cue_ball_sunk(game_state_t *game_state) {
    return game_state->cue_ball_sunk;
}

char *game_state_get_winner(game_state_t *game_state) {
    return game_state->winner;
}

void game_state_set_winner(game_state_t *game_state, char *winner) {
    game_state->winner = winner;
}

void game_state_set_cue_ball_sunk(game_state_t *game_state, bool cue_ball_sunk) {
    game_state->cue_ball_sunk = cue_ball_sunk;
}

void game_state_set_curr_player_turn(game_state_t *game_state, size_t turn){
    game_state->curr_player_turn = turn;
}

void game_state_set_first_turn(game_state_t *game_state, bool first_turn){
    game_state->first_turn = first_turn;
}

void game_state_set_end_of_turn(game_state_t *game_state, bool end_of_turn){
    game_state->end_of_turn = end_of_turn;
}

void game_state_set_player_1_type(game_state_t *game_state, char *player_1_type){
    game_state->player_1_type = player_1_type;
}

void game_state_set_player_2_type(game_state_t *game_state, char *player_2_type){
    game_state->player_2_type = player_2_type;
}
