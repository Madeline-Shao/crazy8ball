#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "body.h"
#include "color.h"
#include "list.h"
#include "scene.h"
#include "game_state.h"
#include "polygon.h"
#include "sdl_wrapper.h"
#include "vector.h"
#include "forces.h"
#include "force_params.h"
#include "force_wrapper.h"
#include <string.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "game_util.h"
#include "game_setup.h"
#include "power_effects.h"

void play_balls_colliding(int channel) {
    Mix_Chunk *balls_colliding = Mix_LoadWAV("sounds/balls_colliding.wav");
    Mix_PlayChannel(channel, balls_colliding, 0);
}

void play_cue_stick_ball(int channel) {
    Mix_Chunk *cue_stick_ball = Mix_LoadWAV("sounds/cue_stick_ball.wav");
    Mix_PlayChannel(channel, cue_stick_ball, 0);
}

void play_pocket(int channel) {
    Mix_Chunk *pocket = Mix_LoadWAV("sounds/pocket.wav");
    Mix_PlayChannel(channel, pocket, 0);
}

void balls_collision_handler(body_t *body1, body_t *body2, vector_t axis, void *aux){
    int channel = *(int *)aux;
    play_balls_colliding(channel);
    double u1 = vec_dot(axis, body_get_velocity(body1));
    double u2 = vec_dot(axis, body_get_velocity(body2));
    double reduced_mass;
    if (body_get_mass(body1) == INFINITY) {
        reduced_mass = body_get_mass(body2);
    }
    else if (body_get_mass(body2) == INFINITY) {
        reduced_mass = body_get_mass(body1);
    }
    else {
        reduced_mass = (body_get_mass(body1) * body_get_mass(body2) / (body_get_mass(body1) + body_get_mass(body2)));
    }
    vector_t impulse = vec_multiply((reduced_mass * (1 + BALL_ELASTICITY) * (u2 - u1)), axis);
    body_add_impulse(body1, impulse);
    body_add_impulse(body2, vec_multiply(-1, impulse));
}


// SHE DID INDEED SAY THAT
void ball_destroy(body_t *ball, body_t *hole, vector_t axis, void *aux) {
    play_pocket(2);
    list_add(game_state_get_balls_sunk(scene_get_game_state((scene_t *) aux)), ball);
    body_set_image(ball, NULL);
    body_set_velocity(ball, (vector_t) {0, 0});
}

void rotation_handler(double x, double y, double xrel, double yrel, void *aux) {
    // if on cue ball then move cue ball
    // if on force bar then change prepare to shoot stick
    body_t *ball = get_object((scene_t *)aux, "CUE_BALL");
    double angle = 2 * M_PI * (sqrt(pow(xrel, 2) + pow(yrel, 2))) / DRAG_DIST;
    body_t *cue_stick = get_object((scene_t *)aux, "CUE_STICK");
    // first quadrant
    if (x > body_get_centroid(ball).x && y < body_get_centroid(ball).y){
        // counterclockwise
        if (-1 * yrel >= xrel){
            body_set_rotation(cue_stick, body_get_angle(cue_stick) - angle);
            double angle1 = body_get_angle(cue_stick) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1), (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(ball));
            body_set_centroid(cue_stick, new_centroid);
        }
        else{
            body_set_rotation(cue_stick, body_get_angle(cue_stick) + angle);
            double angle1 = body_get_angle(cue_stick) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1), (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(ball));
            body_set_centroid(cue_stick, new_centroid);
        }
    }
    // second quadrant
    else if (x < body_get_centroid(ball).x && y < body_get_centroid(ball).y){
        // counterclockwise
        if (yrel >= xrel){
            body_set_rotation(cue_stick, body_get_angle(cue_stick) - angle);
            double angle1 = body_get_angle(cue_stick) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1), (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(ball));
            body_set_centroid(cue_stick, new_centroid);
        }
        else{
            body_set_rotation(cue_stick, body_get_angle(cue_stick) + angle);
            double angle1 = body_get_angle(cue_stick) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1), (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(ball));
            body_set_centroid(cue_stick, new_centroid);
        }
    }
    // third quadrant
    else if (x < body_get_centroid(ball).x && y > body_get_centroid(ball).y){
        // counterclockwise
        if (-1 * yrel <= xrel){
            body_set_rotation(cue_stick, body_get_angle(cue_stick) - angle);
            double angle1 = body_get_angle(cue_stick) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1), (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(ball));
            body_set_centroid(cue_stick, new_centroid);
        }
        else{
            body_set_rotation(cue_stick, body_get_angle(cue_stick) + angle);
            double angle1 = body_get_angle(cue_stick) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1), (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(ball));
            body_set_centroid(cue_stick, new_centroid);
        }
    }
    // fourth quandrant
    else if (x > body_get_centroid(ball).x && y > body_get_centroid(ball).y){
        // counterclockwise
        if (yrel <= xrel){
            body_set_rotation(cue_stick, body_get_angle(cue_stick) - angle);
            double angle1 = body_get_angle(cue_stick) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1), (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(ball));
            body_set_centroid(cue_stick, new_centroid);
        }
        else{
            body_set_rotation(cue_stick, body_get_angle(cue_stick) + angle);
            double angle1 = body_get_angle(cue_stick) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1), (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(ball));
            body_set_centroid(cue_stick, new_centroid);
        }
    }
    // printf("mouse motion - x: %f, y: %f, xrel: %f, yrel: %f\n", x, y, xrel, yrel);
}

void slider_handler(double x, double y, double xrel, double yrel, void *aux) {
    body_t *button = get_object((scene_t *) aux, "BUTTON");
    body_t *cue_stick = get_object((scene_t *) aux, "CUE_STICK");
    body_t *cue_ball = get_object((scene_t *) aux, "CUE_BALL");
    double angle = body_get_angle(cue_stick);
    double y_total = HIGH_RIGHT_CORNER.y - BUTTON_Y;
    if (y >= BUTTON_Y && y <= y_total){
        body_set_centroid(button, (vector_t) {SLIDER_X, y});
        vector_t curr_centroid = body_get_centroid(cue_ball);
        double adjustment = (y - BUTTON_Y) / y_total * PULL_FACTOR_ADJUSTMENT_CONSTANT + (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2);
        body_set_centroid(cue_stick, (vector_t) {curr_centroid.x + adjustment * cos(angle),
                                                 curr_centroid.y + adjustment * sin(angle)});
    }
}

void cue_ball_up_down_handler(double x, double y, double xrel, double yrel, void *aux) {
    body_t *cue_ball = get_object((scene_t *) aux, "CUE_BALL");
    body_t *cue_stick = get_object((scene_t *) aux, "CUE_STICK");
    vector_t table_centroid = body_get_centroid(get_object((scene_t *) aux, "POOL_TABLE"));
    if (y < table_centroid.y + TABLE_HEIGHT / 2 - TABLE_WALL_THICKNESS - WALL_THICKNESS / 2 - BALL_RADIUS && y > table_centroid.y - TABLE_HEIGHT/ 2 + TABLE_WALL_THICKNESS + WALL_THICKNESS / 2 + BALL_RADIUS){
        body_set_centroid(cue_ball, (vector_t) {body_get_centroid(cue_ball).x, y});
        vector_t cue_centroid = vec_add(body_get_centroid(cue_ball), (vector_t) {BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2, 0});
        body_set_centroid(cue_stick, cue_centroid);
        body_set_origin(cue_stick, body_get_centroid(cue_ball));
    }
}

void cue_ball_handler(double x, double y, double xrel, double yrel, void *aux) {
    body_t *cue_ball = get_object((scene_t *) aux, "CUE_BALL");
    body_t *cue_stick = get_object((scene_t *) aux, "CUE_STICK");
    vector_t table_centroid = body_get_centroid(get_object((scene_t *) aux, "POOL_TABLE"));
    // making sure not dragging over another ball
    if (x < table_centroid.x + TABLE_WIDTH / 2 - TABLE_WALL_THICKNESS - WALL_THICKNESS / 2 - BALL_RADIUS && x > table_centroid.x - TABLE_WIDTH / 2 + TABLE_WALL_THICKNESS  + WALL_THICKNESS / 2 + BALL_RADIUS
        && y < table_centroid.y + TABLE_HEIGHT / 2 - TABLE_WALL_THICKNESS - WALL_THICKNESS / 2 - BALL_RADIUS && y > table_centroid.y - TABLE_HEIGHT/ 2 + TABLE_WALL_THICKNESS + WALL_THICKNESS / 2 + BALL_RADIUS){
        scene_t *scene = (scene_t *)aux;
        for (int i = 0; i < scene_bodies(scene); i++) {
            body_t *body = scene_get_body(scene, i);
            if (!strcmp(body_get_info(body), "STRIPED_BALL") || !strcmp(body_get_info(body), "SOLID_BALL") || !strcmp(body_get_info(body), "8_BALL")) {
                vector_t ball_centroid = body_get_centroid(body);
                if (overlaps(x, y, ball_centroid)) {
                    return;
                }
            }
        }
        body_set_centroid(cue_ball, (vector_t) {x, y});
        vector_t cue_centroid = vec_add(body_get_centroid(cue_ball),
                                    (vector_t) {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(body_get_angle(cue_stick)), (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(body_get_angle(cue_stick))});
        body_set_centroid(cue_stick, cue_centroid);
        body_set_origin(cue_stick, body_get_centroid(cue_ball));
    }
}

void shoot_handler(double y, void *aux){
    body_t *button = get_object((scene_t *) aux, "BUTTON");
    body_t *cue_ball = get_object((scene_t *) aux, "CUE_BALL");
    body_t *cue_stick = get_object((scene_t *) aux, "CUE_STICK");
    if (body_get_centroid(button).y != BUTTON_Y){
        if (game_state_get_first_turn(scene_get_game_state((scene_t *) aux))){
            body_remove(get_object((scene_t *) aux, "INITIAL_LINE"));
        }
        play_cue_stick_ball(1);
        // body_set_centroid(cue_stick, (vector_t){HIGH_RIGHT_CORNER.x / 2, CUE_STICK_DEFAULT_Y});
        // body_set_origin(cue_stick, (vector_t){HIGH_RIGHT_CORNER.x / 2, CUE_STICK_DEFAULT_Y});
        body_set_centroid(cue_stick, (vector_t){1200, HIGH_RIGHT_CORNER.y / 2});
        body_set_origin(cue_stick, (vector_t){1200, HIGH_RIGHT_CORNER.y / 2});
        double impulse_factor = y - BUTTON_Y;
        vector_t impulse = {impulse_factor * DEFAULT_IMPULSE * -cos(body_get_angle(cue_stick)), impulse_factor * DEFAULT_IMPULSE * -sin(body_get_angle(cue_stick))};
        body_add_impulse(cue_ball, impulse);
        body_set_rotation(cue_stick, M_PI / 2);
        // set end of turn to TRUE
        game_state_set_end_of_turn(scene_get_game_state((scene_t *)aux), true);
    }
    body_set_centroid(button, (vector_t) {SLIDER_X, BUTTON_Y});
}

bool ball_overlap(scene_t *scene, body_t *body){
    for(int i = 0; i < scene_bodies(scene); i++){
        body_t *body1 = scene_get_body(scene, i);
        if(strcmp(body_get_info(body), body_get_info(body1)) && (!strcmp(body_get_info(body1), "SOLID_BALL") || !strcmp(body_get_info(body1), "STRIPED_BALL")
        || !strcmp(body_get_info(body1), "8_BALL") || !strcmp(body_get_info(body1), "CUE_BALL"))){
            if(fabs(body_get_centroid(body).x - body_get_centroid(body1).x) < BALL_RADIUS
            && fabs(body_get_centroid(body).y - body_get_centroid(body1).y) < BALL_RADIUS)
                return true;
        }
    }
    return false;
}

bool is_balls_stopped(scene_t *scene) {
    for (int i = 0; i < scene_bodies(scene); i++) {
        body_t *body = scene_get_body(scene, i);
        vector_t velocity = body_get_velocity(body);
        if (!vec_equal(velocity, VEC_ZERO)){
            return false;
        }
    }
    return true;
}

bool self_balls_done(scene_t *scene) {
    for (int i = 0; i < scene_bodies(scene);i++) {
        body_t *ball = scene_get_body(scene, i);
        // printf("self balls done: %s\n", game_state_get_current_type(scene_get_game_state(scene)));
        if (!strcmp(body_get_info(ball), game_state_get_current_type(scene_get_game_state(scene)))){
            return false;
        }
    }
    return true;
}

void clear_scene(scene_t *scene) {
    for (int i = 0; i < scene_bodies(scene); i++) {
        body_t *body = scene_get_body(scene, i);
        if (!strcmp(body_get_info(body), "STRIPED_BALL") || !strcmp(body_get_info(body), "SOLID_BALL") || !strcmp(body_get_info(body), "8_BALL")
            || !strcmp(body_get_info(body), "CUE_BALL") || !strcmp(body_get_info(body), "CUE_STICK") || !strcmp(body_get_info(body), "TURN_TEXT")
            || !strcmp(body_get_info(body), "TYPE_INDICATOR")) {
            body_remove(body);
        }
    }
}

void change_text(scene_t *scene, char *info, char *text, TTF_Font *font){
    // TTF_Font* inspace_font = TTF_OpenFont("fonts/InspaceDemoRegular.ttf", 100);
    SDL_Surface *new_text = TTF_RenderText_Solid(font, text, WHITE_COLOR_SDL);
    body_set_image(get_object(scene, info), new_text);
}

void gameplay_handler(scene_t *scene, TTF_Font *font) {
    game_state_t *game_state = scene_get_game_state(scene);

    bool switch_turn = false;
    bool self_balls_sunk = false;
    bool cue_ball_sunk = false;

    list_t *balls_sunk = game_state_get_balls_sunk(game_state);
    bool applied_power = false;
    for (int i = 0; i < list_size(balls_sunk); i++) {
        if (game_state_get_current_type(scene_get_game_state(scene)) != NULL && !strcmp(body_get_info(list_get(balls_sunk, i)), game_state_get_current_type(scene_get_game_state(scene))) && !applied_power){
            float power_rand = rand() / (float) RAND_MAX;
            // float power_rand = 0;
            // printf("rand: %f\n", power_rand);
            // if (power_rand > 0.8 && power_rand < 0.85){
            if (power_rand > 0 && power_rand <= 0.05){
                // printf("add\n");
                add_balls_powerup(scene, (collision_handler_t) balls_collision_handler, (collision_handler_t) ball_destroy);
                applied_power = true;
                game_state_set_balls_powerup(game_state, true);
                change_text(scene, "POWER_TEXT", "POWER UP: 2 extra balls are forced upon your opponent!", font);
            }
            else if (power_rand > 0.05 && power_rand <= 0.1){
                //powerup 2
                // printf("ghost\n");
                add_ghost_powerup(scene, 0.0);
                game_state_set_ghost_powerup(game_state, true);
                applied_power = true;
                change_text(scene, "POWER_TEXT", "POWER UP: You don't have to touch your opponent's balls!", font);
            }
            else if (power_rand > 0 && power_rand <= 1){
                // printf("size\n");
                // powerdown 1
                add_size_powerdown(scene, SIZE_POWERDOWN_ADJUSTMENT_SCALE_FACTOR * BALL_RADIUS);
                game_state_set_size_powerdown(game_state, true);
                applied_power = true;
                change_text(scene, "POWER_TEXT", "POWER DOWN: Unfortunately, you now have gigantic balls!", font);
            }
            else if (power_rand > 0.15 && power_rand < 0.20){
                // powerdown 2
                // printf("switch\n");
                game_state_set_turn_powerdown(game_state, true);
                applied_power = true;
                change_text(scene, "POWER_TEXT", "POWER DOWN: Sorry, you may no longer play with your balls!", font);
                switch_turn = true;
            }
        }

        body_t *ball = list_get(balls_sunk, i);
        // cue ball is sunk
        if (!strcmp(body_get_info(ball), "CUE_BALL")) {
            switch_turn = true;
            cue_ball_sunk = true;
            game_state_set_cue_ball_sunk(game_state, true);
        }
        // 8 ball is sunk and all of your own balls are already sunk
        else if (!strcmp(body_get_info(ball), "8_BALL") && game_state_get_current_type(scene_get_game_state(scene)) != NULL && self_balls_done(scene)) {
            printf("1\n");
            char winner[9];
            snprintf(winner, 9, "Player %d", game_state_get_curr_player_turn(game_state));
            printf("2\n");
            game_state_set_winner(game_state, winner);
            printf("3\n");
            char win_message[17];
            snprintf(win_message, 17, "Winner: %s!", game_state_get_winner(game_state));
            printf("4\n");
            change_text(scene, "WIN_TEXT", win_message, font);
            printf("5\n");
            clear_scene(scene);
            printf("6\n");
        }
        // 8 ball is sunk prematurely
        else if (!strcmp(body_get_info(ball), "8_BALL")) {
            char winner[9];
            snprintf(winner, 9, "Player %d", 3 - game_state_get_curr_player_turn(game_state));
            game_state_set_winner(game_state, winner);

            char win_message[17];
            snprintf(win_message, 17, "Winner: %s!", game_state_get_winner(game_state));
            change_text(scene, "WIN_TEXT", win_message, font);
            clear_scene(scene);
        }
        // sink one of your own balls
        else if (game_state_get_current_type(game_state) != NULL && !strcmp(body_get_info(ball), game_state_get_current_type(game_state))) {
            self_balls_sunk = true;
        }
    }
    if (!cue_ball_sunk) {
        game_state_set_cue_ball_sunk(game_state, false);
    }
    else {
        body_t *cue_ball = get_object(scene, "CUE_BALL");
        body_t *cue_stick = get_object(scene, "CUE_STICK");
        SDL_Surface *ball_image = IMG_Load("images/ball_16.png");
        body_set_image(cue_ball, ball_image);
        body_set_centroid(cue_ball, (vector_t){HIGH_RIGHT_CORNER.x * 4 / 7, HIGH_RIGHT_CORNER.y / 2});
        vector_t cue_centroid = vec_add(body_get_centroid(cue_ball), (vector_t) {BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2, 0});
        body_set_centroid(cue_stick, cue_centroid);
        body_set_origin(cue_stick, body_get_centroid(cue_ball));
    }

    // setting the type for each player
    if (!game_state_get_first_turn(game_state) && game_state_get_player_1_type(game_state) == NULL) {
        // printf("never gonna give you up");
        if ((cue_ball_sunk && list_size(balls_sunk) > 1) || (!cue_ball_sunk && list_size(balls_sunk) > 0)) {
            // printf("never gonna give let you down");
            for (int i = 0; i < list_size(balls_sunk); i++) {
                body_t *ball = list_get(balls_sunk, i);
                if (strcmp((char *) body_get_info(ball), "CUE_BALL")) {
                    // printf("never gonna give run around");
                    if (game_state_get_curr_player_turn(game_state) == 1) {
                       // printf("and desert you");
                        game_state_set_player_1_type(game_state, (char *) body_get_info(ball));
                        if (!strcmp((char *) body_get_info(ball), "SOLID_BALL")) {
                           // printf("never gonna make you cry");
                            SDL_Surface *solid_img = IMG_Load("images/solid_type_ball.png");
                            body_set_image(get_object(scene, "TYPE_INDICATOR"), solid_img);
                            game_state_set_player_2_type(game_state, "STRIPED_BALL");
                        }
                        else {
                            // printf("never gonna say goodbye");
                            SDL_Surface *striped_img = IMG_Load("images/striped_type_ball.png");
                            body_set_image(get_object(scene, "TYPE_INDICATOR"), striped_img);
                            game_state_set_player_2_type(game_state, "SOLID_BALL");
                        }
                    }
                    else {
                        game_state_set_player_2_type(game_state, (char *) body_get_info(ball));
                        if (!strcmp((char *) body_get_info(ball), "SOLID_BALL")) {
                            // printf("never gonna tell a lie");
                            SDL_Surface *solid_img = IMG_Load("images/solid_type_ball.png");
                            body_set_image(get_object(scene, "TYPE_INDICATOR"), solid_img);
                            game_state_set_player_1_type(game_state, "STRIPED_BALL");
                        }
                        else {
                           // printf("and hurt you");
                            SDL_Surface *striped_img = IMG_Load("images/striped_type_ball.png");
                            body_set_image(get_object(scene, "TYPE_INDICATOR"), striped_img);
                            game_state_set_player_1_type(game_state, "SOLID_BALL");
                        }
                    }
                    self_balls_sunk = true;

                    // char type[45];

                    // if (!strcmp(game_state_get_player_1_type(game_state), "SOLID_BALL")){
                    //     body_set_centroid(solid_ball, (vector_t) {LOW_LEFT_CORNER.x + 200, 72.5});
                    //     body_set_centroid(striped_ball, (vector_t) {LOW_LEFT_CORNER.x + 600, 72.5});
                    //     scene_add_body(scene, solid_ball);
                    //     scene_add_body(scene, striped_ball);
                    //     snprintf(type, 45, "Player 1: %s | Player 2: %s", "SOLID", "STRIPES");
                    // }
                    // else {
                    //     body_set_centroid(striped_ball, (vector_t) {LOW_LEFT_CORNER.x + 200, 72.5});
                    //     body_set_centroid(solid_ball, (vector_t) {LOW_LEFT_CORNER.x + 600, 72.5});
                    //     scene_add_body(scene, solid_ball);
                    //     scene_add_body(scene, striped_ball);
                    //     snprintf(type, 45, "Player 1: %s | Player 2: %s", "STRIPES", "SOLID");
                    // }
                    // change_text(scene, "TYPE_TEXT", "PLAYER 1       PLAYER 2", font);
                    break;
                }
            }
        }
        else {
            switch_turn = true;
        }
    }
    if (!self_balls_sunk) {
        switch_turn = true;
    }
    if (switch_turn) {
        if (game_state_get_balls_powerup(game_state)){
            game_state_set_balls_powerup(game_state, false);
            change_text(scene, "POWER_TEXT", "", font);
        }
        else if (game_state_get_ghost_powerup(game_state)){
            add_ghost_powerup(scene, BALL_MASS);
            game_state_set_ghost_powerup(game_state, false);
            change_text(scene, "POWER_TEXT", "", font);
        }
        else if (game_state_get_size_powerdown(game_state)){
            add_size_powerdown(scene, BALL_RADIUS);
            game_state_set_size_powerdown(game_state, false);
            change_text(scene, "POWER_TEXT", "", font);
        }
        else if (game_state_get_turn_powerdown(game_state)){
            game_state_set_turn_powerdown(game_state, false);
            change_text(scene, "POWER_TEXT", "POWER DOWN: Sorry, you may no longer play with your balls!", font);
        }
        else {
            change_text(scene, "POWER_TEXT", "", font);
        }
        add_size_powerdown(scene, BALL_RADIUS);
        game_state_set_curr_player_turn(game_state, 3 - game_state_get_curr_player_turn(game_state));
        if (game_state_get_curr_player_turn(game_state) == 1){
            change_text(scene, "TURN_TEXT", "Player 1", font);
            if (game_state_get_player_1_type(game_state) != NULL) {
                if (!strcmp((char *) game_state_get_player_1_type(game_state), "SOLID_BALL")) {
                    // printf("never gonna make you cry");
                    SDL_Surface *solid_img = IMG_Load("images/solid_type_ball.png");
                    body_set_image(get_object(scene, "TYPE_INDICATOR"), solid_img);
                }
                else {
                    // printf("never gonna say goodbye");
                    SDL_Surface *striped_img = IMG_Load("images/striped_type_ball.png");
                    body_set_image(get_object(scene, "TYPE_INDICATOR"), striped_img);
                }
            }
        }
        else{
            change_text(scene, "TURN_TEXT", "Player 2", font);
            if (game_state_get_player_2_type(game_state) != NULL) {
                if (!strcmp((char *) game_state_get_player_2_type(game_state), "SOLID_BALL")) {
                    // printf("never gonna make you cry");
                    SDL_Surface *solid_img = IMG_Load("images/solid_type_ball.png");
                    body_set_image(get_object(scene, "TYPE_INDICATOR"), solid_img);
                }
                else {
                    // printf("never gonna say goodbye");
                    SDL_Surface *striped_img = IMG_Load("images/striped_type_ball.png");
                    body_set_image(get_object(scene, "TYPE_INDICATOR"), striped_img);
                }
            }
        }
    }

    while (list_size(balls_sunk) != 0) {
        body_t *ball = list_remove(balls_sunk, 0);
        // if not a cue ball
        if (strcmp(body_get_info(ball), "CUE_BALL")) {
            body_remove(ball);
        }
    }

    // set to true later in shoot handler
    game_state_set_end_of_turn(game_state, false);
    if (game_state_get_first_turn(game_state)){
        game_state_set_first_turn(game_state, false);
    }
    // if (game_state_get_player_1_type(game_state) != NULL && game_state_get_player_2_type(game_state) != NULL) {
    //     printf("current turn: %d, 1_type: %s, 2_type: %s\n", game_state_get_curr_player_turn(game_state), game_state_get_player_1_type(game_state), game_state_get_player_2_type(game_state));
    // }
    // else {
    //     printf("current turn: %d\n", game_state_get_curr_player_turn(game_state));
    // }
}

void player_mouse_handler(int key, mouse_event_type_t type, double x, double y, void *aux) {
    // do this if the game is not over yet
    if(key == SDL_BUTTON_LEFT && game_state_get_winner(scene_get_game_state((scene_t *)aux)) != NULL){
        if(type == MOUSE_DOWN){
        if (x >= QUIT_BUTTON_CENTROID.x - QUIT_BUTTON_SIDE_LENGTH / 2
                    && x <= QUIT_BUTTON_CENTROID.x + QUIT_BUTTON_SIDE_LENGTH / 2
                    && y >= QUIT_BUTTON_CENTROID.y - QUIT_BUTTON_SIDE_LENGTH / 2
                    && y <= QUIT_BUTTON_CENTROID.y + QUIT_BUTTON_SIDE_LENGTH / 2){
                    game_state_set_game_quit(scene_get_game_state((scene_t *)aux), true);
                }
        }
    }
    else if (key == SDL_BUTTON_LEFT && game_state_get_winner(scene_get_game_state((scene_t *)aux)) == NULL) {
         if (type == MOUSE_DOWN){
             // within instruction menu code; should work regardless of game_start status
             if (game_state_get_game_instructions(scene_get_game_state((scene_t *)aux))){
                 // close instruction menu if clicking on quit instructions button
                 if (x >= QUIT_BUTTON_CENTROID.x - START_MENU_BUTTON_SIDE_LENGTH / 2
                 && x <= QUIT_BUTTON_CENTROID.x + START_MENU_BUTTON_SIDE_LENGTH / 2
                 && y >= QUIT_BUTTON_CENTROID.y - START_MENU_BUTTON_SIDE_LENGTH / 2
                 && y <= QUIT_BUTTON_CENTROID.y + START_MENU_BUTTON_SIDE_LENGTH / 2){
                     body_remove(get_object((scene_t *) aux, "INSTRUCTIONS"));
                     body_remove(get_object((scene_t *) aux, "INSTR_QUIT"));
                     game_state_set_game_instructions(scene_get_game_state((scene_t *)aux), false);
                 }
             }
             // start menu handling
             else if (!game_state_get_game_start(scene_get_game_state((scene_t *)aux))){
                 // clicking on play button
                 if (x >= START_PLAY_BUTTON_CENTROID.x - RECTANGULAR_BUTTON_WIDTH / 2
                    && x <= START_PLAY_BUTTON_CENTROID.x + RECTANGULAR_BUTTON_WIDTH / 2
                    && y >= START_PLAY_BUTTON_CENTROID.y - RECTANGULAR_BUTTON_HEIGHT / 2
                    && y <= START_PLAY_BUTTON_CENTROID.y + RECTANGULAR_BUTTON_HEIGHT / 2){
                        body_remove(get_object((scene_t *) aux, "START_MENU"));
                        body_remove(get_object((scene_t *) aux, "INFO_BUTTON"));
                        body_remove(get_object((scene_t *) aux, "PLAY_BUTTON"));
                        body_remove(get_object((scene_t *) aux, "TITLE_TEXT"));
                        body_remove(get_object((scene_t *) aux, "START_QUIT_BUTTON"));
                        game_state_set_game_start(scene_get_game_state((scene_t *)aux), true);
                    }
                 // clicking help button
                 else if (x >= START_INSTRUCTIONS_BUTTON_CENTROID.x - RECTANGULAR_BUTTON_WIDTH / 2
                    && x <= START_INSTRUCTIONS_BUTTON_CENTROID.x + RECTANGULAR_BUTTON_WIDTH / 2
                    && y >= START_INSTRUCTIONS_BUTTON_CENTROID.y - RECTANGULAR_BUTTON_HEIGHT / 2
                    && y <= START_INSTRUCTIONS_BUTTON_CENTROID.y + RECTANGULAR_BUTTON_HEIGHT / 2){
                        game_setup_add_instructions((scene_t *)aux);
                        game_state_set_game_instructions(scene_get_game_state((scene_t *)aux), true);
                    }
                 // clicking quit button
                 else if (x >= START_QUIT_BUTTON_CENTROID.x - RECTANGULAR_BUTTON_WIDTH / 2
                    && x <= START_QUIT_BUTTON_CENTROID.x + RECTANGULAR_BUTTON_WIDTH / 2
                    && y >= START_QUIT_BUTTON_CENTROID.y - RECTANGULAR_BUTTON_HEIGHT / 2
                    && y <= START_QUIT_BUTTON_CENTROID.y + RECTANGULAR_BUTTON_HEIGHT / 2){
                        game_state_set_game_quit(scene_get_game_state((scene_t *)aux), true);
                    }
                }
             // main game screen handling
             else if (game_state_get_game_start(scene_get_game_state((scene_t *)aux))){
                if (x >= QUIT_BUTTON_CENTROID.x - QUIT_BUTTON_SIDE_LENGTH / 2
                    && x <= QUIT_BUTTON_CENTROID.x + QUIT_BUTTON_SIDE_LENGTH / 2
                    && y >= QUIT_BUTTON_CENTROID.y - QUIT_BUTTON_SIDE_LENGTH / 2
                    && y <= QUIT_BUTTON_CENTROID.y + QUIT_BUTTON_SIDE_LENGTH / 2){
                    game_state_set_game_quit(scene_get_game_state((scene_t *)aux), true);
                }
                else if (x >= HELP_BUTTON_CENTROID.x - QUIT_BUTTON_SIDE_LENGTH / 2
                    && x <= HELP_BUTTON_CENTROID.x + QUIT_BUTTON_SIDE_LENGTH / 2
                    && y >= HELP_BUTTON_CENTROID.y - QUIT_BUTTON_SIDE_LENGTH / 2
                    && y <= HELP_BUTTON_CENTROID.y + QUIT_BUTTON_SIDE_LENGTH / 2){
                    game_setup_add_instructions((scene_t *)aux);
                    game_state_set_game_instructions(scene_get_game_state((scene_t *)aux), true);
                }


                if(is_balls_stopped((scene_t *) aux)){
                    body_t *button = get_object((scene_t *) aux, "BUTTON");
                    body_t *cue_ball = get_object((scene_t *)aux, "CUE_BALL");
                    game_state_t *game_state = scene_get_game_state((scene_t *) aux);
                    if (x >= body_get_centroid(button).x - BUTTON_WIDTH / 2
                        && x <= body_get_centroid(button).x + BUTTON_WIDTH / 2
                        && y >= body_get_centroid(button).y - BUTTON_WIDTH / 2
                        && y <= body_get_centroid(button).y + BUTTON_WIDTH / 2){
                        sdl_on_motion((motion_handler_t)slider_handler, aux);
                    }
                    // click on cue ball
                    else if (x >= body_get_centroid(cue_ball).x - BALL_RADIUS
                            && x <= body_get_centroid(cue_ball).x + BALL_RADIUS
                            && y >= body_get_centroid(cue_ball).y - BALL_RADIUS
                            && y <= body_get_centroid(cue_ball).y + BALL_RADIUS) {
                            //&& game_state_get_cue_ball_sunk(game_state) ADD BACK LATER
                        if (game_state_get_first_turn(game_state)){
                            sdl_on_motion((motion_handler_t)cue_ball_up_down_handler, aux);
                        }
                        else {
                            sdl_on_motion((motion_handler_t)cue_ball_handler, aux);
                        }
                    }
                    else{
                        sdl_on_motion((motion_handler_t)rotation_handler, aux);
                    }
                }
            }
        }
         else if (type == MOUSE_UP){
             sdl_on_motion((NULL), NULL);
             shoot_handler(y, aux);
         }
    }
}

// KONAMI
void player_key_handler(char key, key_event_type_t type, double held_time, void * aux) {
    if (type == MOUSE_UP){
        // printf("%c", key);
        game_state_t *game_state = scene_get_game_state((scene_t *)aux);
        list_t *keys = game_state_get_keys(game_state);
        if (list_size(keys) == 10) {
            list_remove(keys, 0);
        }
        if (key == LEFT_ARROW) {
            list_add(keys, (void *)'!');
        }
        else if (key == RIGHT_ARROW) {
            list_add(keys, (void *)'@');
        }
        else if (key == UP_ARROW) {
            list_add(keys, (void *)'#');
        }
        else if (key == DOWN_ARROW) {
            list_add(keys, (void *)'$');
        }
        else {
            list_add(keys, (void *)key);
        }
    }
}

void add_forces(scene_t *scene){
    int channel_num = 4;
    for(int i = 0; i < scene_bodies(scene) - 1; i++){
        body_t *body1 = scene_get_body(scene, i);
        if(!strcmp(body_get_info(body1), "SOLID_BALL") || !strcmp(body_get_info(body1), "STRIPED_BALL") || !strcmp(body_get_info(body1), "8_BALL") || !strcmp(body_get_info(body1), "CUE_BALL")){
            create_friction(scene, MU, G, body1);
            for (int j = i + 1; j < scene_bodies(scene); j++) {
                body_t *body2 = scene_get_body(scene, j);
                if(!strcmp(body_get_info(body2), "SOLID_BALL") || !strcmp(body_get_info(body2), "STRIPED_BALL") || !strcmp(body_get_info(body2), "8_BALL") || !strcmp(body_get_info(body2), "CUE_BALL") || !strcmp(body_get_info(body2), "WALL")){
                    int *aux = malloc(sizeof(int));
                    *aux = channel_num;
                    create_collision(scene, body1, body2, (collision_handler_t) balls_collision_handler, aux, NULL);
                    channel_num++;
                }
                else if(!strcmp(body_get_info(body2), "HOLE")){
                    // int *aux = malloc(sizeof(int));
                    // *aux = channel_num;
                    create_collision(scene, body1, body2, (collision_handler_t) ball_destroy, scene, NULL);
                    // channel_num++;
                }
                if(!strcmp(body_get_info(body1), "CUE_BALL") && !strcmp(body_get_info(body2), "CUE_STICK")){
                    create_physics_collision(scene, BALL_ELASTICITY, body1, body2);
                }
            }
        }
    }
}

void stop_balls(scene_t *scene){
    for (int i = 0; i < scene_bodies(scene); i++) {
        body_t *body = scene_get_body(scene, i);
        vector_t velocity = body_get_velocity(body);
        if (fabs(velocity.x) < VELOCITY_THRESHOLD.x && fabs(velocity.y) < VELOCITY_THRESHOLD.y){
            // add backlog force
            // if(ball_overlap(scene, body)){
            //     vector_t backlog = {BACKLOG_FORCE_CONSTANT_TO_SATISFY_PATRICKS_DESIRES * cos(body_get_angle(body)),
            //                         BACKLOG_FORCE_CONSTANT_TO_SATISFY_PATRICKS_DESIRES * sin(body_get_angle(body))};
            //     body_add_force(body, backlog);
            // }
            body_set_velocity(body, (vector_t) {0, 0});

        }
        /*if (!strcmp(body_get_info(body), "CUE_BALL")){
            printf("%f %f\n", velocity.x, velocity.y);
        }*/
    }
}

void konami_code(scene_t *scene) {
    game_state_t *game_state = scene_get_game_state(scene);
    if (!game_state_get_konami(game_state)) {
        list_t *code = list_init(10, NULL);
        list_add(code, (void *)'#');
        list_add(code, (void *)'#');
        list_add(code, (void *)'$');
        list_add(code, (void *)'$');
        list_add(code, (void *)'!');
        list_add(code, (void *)'@');
        list_add(code, (void *)'!');
        list_add(code, (void *)'@');
        list_add(code, (void *)'b');
        list_add(code, (void *)'a');
        list_t *keys = game_state_get_keys(game_state);
        if (list_size(keys) >= 10) {
            for (int i = 0; i < list_size(code); i ++ ){
                if (list_get(code, i) != list_get(keys, i)) {
                    return;
                }
            }
            SDL_Surface *bg_image = IMG_Load("images/background.jpg");
            body_set_image(get_object(scene, "BACKGROUND"), bg_image);
            if (get_object(scene, "START_MENU") != NULL) {
                body_set_image(get_object(scene, "START_MENU"), bg_image);
            }
            Mix_Chunk *background = Mix_LoadWAV("sounds/konami.wav");
            Mix_PlayChannel(3, background, -1);
            game_state_set_konami(game_state, true);
        }
    }
}

int main(){
    SDL_Renderer *renderer = sdl_init(LOW_LEFT_CORNER, HIGH_RIGHT_CORNER);
    scene_t *scene = scene_init();
    game_state_t *game_state = game_state_init();
    scene_set_game_state(scene, game_state);

    TTF_Init();
    TTF_Font* font = TTF_OpenFont("fonts/BebasNeue-Regular.TTF", 100);
    game_setup(scene, font);

    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 0);
    add_forces(scene);
    sdl_on_mouse((mouse_handler_t)player_mouse_handler, scene);
    sdl_on_key((key_handler_t)player_key_handler, scene);

    SDL_Surface *icon = IMG_Load("images/ball_8.png");
    sdl_set_icon(icon);

    sdl_set_title("Crazy 8 Ball");

    while (!sdl_is_done()){
        if(game_state_get_game_quit(scene_get_game_state(scene))){
            break;
        }
        sdl_render_scene(scene);
        scene_tick(scene, time_since_last_tick());
        stop_balls(scene);
        konami_code(scene);
        if (game_state_get_end_of_turn(scene_get_game_state(scene)) && is_balls_stopped(scene) && game_state_get_winner(scene_get_game_state(scene)) == NULL){
            vector_t cue_centroid = vec_add(body_get_centroid(get_object(scene, "CUE_BALL")), (vector_t) {BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2, 0});
            body_set_rotation(get_object(scene, "CUE_STICK"), 0);
            body_set_centroid(get_object(scene, "CUE_STICK"), cue_centroid);
            body_set_origin(get_object(scene, "CUE_STICK"), body_get_centroid(get_object(scene, "CUE_BALL")));
            gameplay_handler(scene, font);
        }
        if (game_state_get_winner(scene_get_game_state(scene)) != NULL) {
            // printf("winner: %s: \n", game_state_get_winner(scene_get_game_state(scene)));
        }
    }
    scene_free(scene);
}
