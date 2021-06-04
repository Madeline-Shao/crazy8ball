#include <stdio.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
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

/**
 * Associates the arrow keys with a character.
 */
typedef enum {
    LEFT_ARROW_CHAR = '!',
    UP_ARROW_CHAR = '#',
    RIGHT_ARROW_CHAR = '@',
    DOWN_ARROW_CHAR = '$'
} arrow_key_char_t;

/**
 * Handles collisions between two balls and plays a sound of balls colliding.
 * @param body1 - The first ball
 * @param body2 - The second ball
 * @param axis - The axis of collision
 * @param aux - An auxiliary value
 */
void balls_collision_handler(body_t *body1, body_t *body2, vector_t axis, void *aux){
    play_sound(-1, "sounds/balls_colliding.wav");

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

/**
 * Handles the collision when the ball goes into a hole. Plays a sound and makes the ball
 * disappear.
 * @param ball - The ball
 * @param hole - The hole
 * @param axis - The axis of collision
 * @param aux - An auxiliary value
 */
void ball_destroy(body_t *ball, body_t *hole, vector_t axis, void *aux) {
    play_sound(POCKET_CHANNEL, "sounds/pocket.wav");
    list_add(game_state_get_balls_sunk(scene_get_game_state((scene_t *) aux)), ball);
    body_set_image(ball, NULL);
    body_set_velocity(ball, VEC_ZERO);
}

/**
 * Handles the rotation of the cue stick according to the mouse motion.
 * @param x - The x position of the mouse
 * @param y - The y position of the mouse
 * @param xrel - The x velocity of the mouse
 * @param yrel - The y velocity of the mouse
 * @param aux - An auxiliary value
 */
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
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1),
                                     (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(ball));
            body_set_centroid(cue_stick, new_centroid);
        }
        else{
            body_set_rotation(cue_stick, body_get_angle(cue_stick) + angle);
            double angle1 = body_get_angle(cue_stick) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1),
                                     (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
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
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1),
                                     (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(ball));
            body_set_centroid(cue_stick, new_centroid);
        }
        else{
            body_set_rotation(cue_stick, body_get_angle(cue_stick) + angle);
            double angle1 = body_get_angle(cue_stick) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1),
                                     (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
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
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1),
                                     (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(ball));
            body_set_centroid(cue_stick, new_centroid);
        }
        else{
            body_set_rotation(cue_stick, body_get_angle(cue_stick) + angle);
            double angle1 = body_get_angle(cue_stick) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1),
                                     (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
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
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1),
                                     (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(ball));
            body_set_centroid(cue_stick, new_centroid);
        }
        else{
            body_set_rotation(cue_stick, body_get_angle(cue_stick) + angle);
            double angle1 = body_get_angle(cue_stick) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1),
                                     (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(ball));
            body_set_centroid(cue_stick, new_centroid);
        }
    }
}

/**
 * Handles the movement of the slider and the cue stick according to the mouse motion.
 * @param x - The x position of the mouse
 * @param y - The y position of the mouse
 * @param xrel - The x velocity of the mouse
 * @param yrel - The y velocity of the mouse
 * @param aux - An auxiliary value
 */
void slider_handler(double x, double y, double xrel, double yrel, void *aux) {
    body_t *button = get_object((scene_t *) aux, "BUTTON");
    body_t *cue_stick = get_object((scene_t *) aux, "CUE_STICK");
    body_t *cue_ball = get_object((scene_t *) aux, "CUE_BALL");
    double angle = body_get_angle(cue_stick);
    double y_total = HIGH_RIGHT_CORNER.y - BUTTON_Y;
    if (y >= BUTTON_Y && y <= y_total){
        body_set_centroid(button, (vector_t) {SLIDER_X, y});
        vector_t curr_centroid = body_get_centroid(cue_ball);
        double adjustment = (y - BUTTON_Y) / y_total * PULL_FACTOR_ADJUSTMENT_CONSTANT +
                            (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2);
        body_set_centroid(cue_stick, (vector_t) {curr_centroid.x + adjustment * cos(angle),
                                                 curr_centroid.y + adjustment * sin(angle)});
    }
}

/**
 * Handles the movement of the cue ball up and down according to the mouse motion.
 * @param x - The x position of the mouse
 * @param y - The y position of the mouse
 * @param xrel - The x velocity of the mouse
 * @param yrel - The y velocity of the mouse
 * @param aux - An auxiliary value
 */
void cue_ball_up_down_handler(double x, double y, double xrel, double yrel, void *aux) {
    body_t *cue_ball = get_object((scene_t *) aux, "CUE_BALL");
    body_t *cue_stick = get_object((scene_t *) aux, "CUE_STICK");
    vector_t table_centroid = body_get_centroid(get_object((scene_t *) aux, "POOL_TABLE"));
    if (y < table_centroid.y + TABLE_HEIGHT / 2 - TABLE_WALL_THICKNESS - WALL_THICKNESS / 2 -
        BALL_RADIUS && y > table_centroid.y - TABLE_HEIGHT/ 2 + TABLE_WALL_THICKNESS + WALL_THICKNESS / 2 + BALL_RADIUS){
        body_set_centroid(cue_ball, (vector_t) {body_get_centroid(cue_ball).x, y});
        vector_t cue_centroid = vec_add(body_get_centroid(cue_ball),
                                    (vector_t) {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(body_get_angle(cue_stick)),
                                                (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(body_get_angle(cue_stick))});
        body_set_centroid(cue_stick, cue_centroid);
        body_set_origin(cue_stick, body_get_centroid(cue_ball));
    }
}

/**
 * Handles the movement of the cue ball according to the mouse motion.
 * @param x - The x position of the mouse
 * @param y - The y position of the mouse
 * @param xrel - The x velocity of the mouse
 * @param yrel - The y velocity of the mouse
 * @param aux - An auxiliary value
 */
void cue_ball_handler(double x, double y, double xrel, double yrel, void *aux) {
    body_t *cue_ball = get_object((scene_t *) aux, "CUE_BALL");
    body_t *cue_stick = get_object((scene_t *) aux, "CUE_STICK");
    vector_t table_centroid = body_get_centroid(get_object((scene_t *) aux, "POOL_TABLE"));
    // making sure not dragging over another ball
    if (x < table_centroid.x + TABLE_WIDTH / 2 - TABLE_WALL_THICKNESS - WALL_THICKNESS / 2 - BALL_RADIUS
        && x > table_centroid.x - TABLE_WIDTH / 2 + TABLE_WALL_THICKNESS  + WALL_THICKNESS / 2 + BALL_RADIUS
        && y < table_centroid.y + TABLE_HEIGHT / 2 - TABLE_WALL_THICKNESS - WALL_THICKNESS / 2 - BALL_RADIUS
        && y > table_centroid.y - TABLE_HEIGHT/ 2 + TABLE_WALL_THICKNESS + WALL_THICKNESS / 2 + BALL_RADIUS){
        scene_t *scene = (scene_t *)aux;
        for (int i = 0; i < scene_bodies(scene); i++) {
            body_t *body = scene_get_body(scene, i);
            if (!strcmp(body_get_info(body), "STRIPED_BALL") || !strcmp(body_get_info(body), "SOLID_BALL")
                || !strcmp(body_get_info(body), "8_BALL")) {
                vector_t ball_centroid = body_get_centroid(body);
                if (overlaps(x, y, ball_centroid)) {
                    return;
                }
            }
        }
        body_set_centroid(cue_ball, (vector_t) {x, y});
        vector_t cue_centroid = vec_add(body_get_centroid(cue_ball),
                                    (vector_t) {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(body_get_angle(cue_stick)),
                                                (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(body_get_angle(cue_stick))});
        body_set_centroid(cue_stick, cue_centroid);
        body_set_origin(cue_stick, body_get_centroid(cue_ball));
    }
}

/**
 * Handles the shooting of the stick, adding an impulse to the cue ball, playing a sound,
 * resetting the positions of the slider and cue stick.
 * @param y - The y position of the mouse
 * @param aux - An auxiliary value
 */
void shoot_handler(double y, void *aux){
    body_t *button = get_object((scene_t *) aux, "BUTTON");
    body_t *cue_ball = get_object((scene_t *) aux, "CUE_BALL");
    body_t *cue_stick = get_object((scene_t *) aux, "CUE_STICK");
    if (body_get_centroid(button).y != BUTTON_Y){
        if (game_state_get_first_turn(scene_get_game_state((scene_t *) aux))){
            body_remove(get_object((scene_t *) aux, "INITIAL_LINE"));
        }
        play_sound(CUE_STICK_BALL_CHANNEL, "sounds/cue_stick_ball.wav");
        body_set_centroid(cue_stick, (vector_t){CUE_STICK_DEFAULT_X, HIGH_RIGHT_CORNER.y / 2});
        body_set_origin(cue_stick, (vector_t){CUE_STICK_DEFAULT_X, HIGH_RIGHT_CORNER.y / 2});
        double impulse_factor = y - BUTTON_Y;
        vector_t impulse = {impulse_factor * DEFAULT_IMPULSE * -cos(body_get_angle(cue_stick)),
                            impulse_factor * DEFAULT_IMPULSE * -sin(body_get_angle(cue_stick))};
        body_add_impulse(cue_ball, impulse);
        body_set_rotation(cue_stick, M_PI / 2);
        // set end of turn to TRUE
        game_state_set_end_of_turn(scene_get_game_state((scene_t *)aux), true);
    }
    body_set_centroid(button, (vector_t) {SLIDER_X, BUTTON_Y});
}

/**
 * Checks if given body overlaps with any of the balls in the scene
 * @param scene - The scene containing the balls
 * @param body - The given body
 * @return True if the body overlaps with any of the balls, false otherwise
 */
bool ball_overlap(scene_t *scene, body_t *body){
    for(int i = 0; i < scene_bodies(scene); i++){
        body_t *body1 = scene_get_body(scene, i);
        if(strcmp(body_get_info(body), body_get_info(body1)) && (!strcmp(body_get_info(body1), "SOLID_BALL")
           || !strcmp(body_get_info(body1), "STRIPED_BALL")
        || !strcmp(body_get_info(body1), "8_BALL") || !strcmp(body_get_info(body1), "CUE_BALL"))){
            if(fabs(body_get_centroid(body).x - body_get_centroid(body1).x) < BALL_RADIUS
            && fabs(body_get_centroid(body).y - body_get_centroid(body1).y) < BALL_RADIUS)
                return true;
        }
    }
    return false;
}

/**
 * Checks if the bodies in the scene are stopped (zero velocity).
 * @param scene - The scene containing the bodies
 * @return True if all the bodies are stopped, false otherwise.
 */
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

/**
 * Checks if the current player has sunk all their balls.
 * @param scene - The scene containing the bodies
 * @return True if the current player has sunk all their balls, false otherwise
 */
bool self_balls_done(scene_t *scene) {
    for (int i = 0; i < scene_bodies(scene);i++) {
        body_t *ball = scene_get_body(scene, i);
        if (!strcmp(body_get_info(ball), game_state_get_current_type(scene_get_game_state(scene)))){
            return false;
        }
    }
    return true;
}

/**
 * Clears the scene of all the balls, the cue stick, the turn text, and the type indicator.
 * @param scene - The scene containing the bodies.
 */
void clear_scene(scene_t *scene) {
    for (int i = 0; i < scene_bodies(scene); i++) {
        body_t *body = scene_get_body(scene, i);
        if (!strcmp(body_get_info(body), "STRIPED_BALL") || !strcmp(body_get_info(body), "SOLID_BALL")
            || !strcmp(body_get_info(body), "8_BALL") || !strcmp(body_get_info(body), "CUE_BALL")
            || !strcmp(body_get_info(body), "CUE_STICK") || !strcmp(body_get_info(body), "TURN_TEXT")
            || !strcmp(body_get_info(body), "TYPE_INDICATOR")) {
            body_remove(body);
        }
    }
}

void win_handling(scene_t *scene, TTF_Font *font, int winner_num) {
    game_state_t * game_state = scene_get_game_state(scene);
    char winner[9];
    snprintf(winner, WINNER_TEXT_LENGTH, "Player %d", winner_num);
    game_state_set_winner(game_state, winner);
    char win_message[17];
    snprintf(win_message, WIN_MESSAGE_LENGTH, "Winner: %s!", game_state_get_winner(game_state));
    change_text(scene, "WIN_TEXT", win_message, font, WHITE_COLOR_SDL);
    clear_scene(scene);
}

/**
 * Handles the gameplay, including turns, win conditions, power ups/downs, etc.
 * Updates bodies as necessary.
 * @param scene - The scene containing the bodies
 * @param font - The font of the text
 */
void gameplay_handler(scene_t *scene, TTF_Font *font) {
    game_state_t *game_state = scene_get_game_state(scene);

    bool switch_turn = false;
    bool self_balls_sunk = false;
    bool cue_ball_sunk = false;
    int winner_num = -1;

    change_text(scene, "POWER_TEXT", "", font, GOLD_COLOR_SDL);

    list_t *balls_sunk = game_state_get_balls_sunk(game_state);
    bool applied_power = false;
    for (int i = 0; i < list_size(balls_sunk); i++) {
        if (game_state_get_current_type(scene_get_game_state(scene)) != NULL
            && !strcmp(body_get_info(list_get(balls_sunk, i)), game_state_get_current_type(scene_get_game_state(scene)))
            && !applied_power){
            float power_rand = rand() / (float) RAND_MAX;
            if (power_rand > 0.0 && power_rand <= POWER_PROBABILITY_SPACING){
                add_balls_powerup(scene, (collision_handler_t) balls_collision_handler, (collision_handler_t) ball_destroy);
                applied_power = true;
                game_state_set_balls_powerup(game_state, true);
                change_text(scene, "POWER_TEXT", "POWER UP: 2 extra balls are forced upon your opponent!",
                            font, GOLD_COLOR_SDL);
            }
            else if (power_rand > POWER_PROBABILITY_SPACING && power_rand <= 2 * POWER_PROBABILITY_SPACING){
                add_ghost_powerup(scene, 0.0);
                game_state_set_ghost_powerup(game_state, true);
                applied_power = true;
                change_text(scene, "POWER_TEXT", "POWER UP: You can shoot through your opponent's balls!",
                            font, GOLD_COLOR_SDL);
            }
            else if (power_rand > 2 * POWER_PROBABILITY_SPACING && power_rand <= 3 * POWER_PROBABILITY_SPACING){
                add_size_powerdown(scene, SIZE_POWERDOWN_ADJUSTMENT_SCALE_FACTOR * BALL_RADIUS);
                game_state_set_size_powerdown(game_state, true);
                applied_power = true;
                change_text(scene, "POWER_TEXT", "POWER DOWN: Unfortunately, you now have gigantic balls!",
                            font, GOLD_COLOR_SDL);
            }
            else if (power_rand > 3 * POWER_PROBABILITY_SPACING && power_rand < 4 * POWER_PROBABILITY_SPACING){
                game_state_set_turn_powerdown(game_state, true);
                applied_power = true;
                change_text(scene, "POWER_TEXT", "POWER DOWN: Sorry, your turn has ended!",
                            font, GOLD_COLOR_SDL);
                switch_turn = true;
            }
            // SHE DID INDEED SAY THAT
        }

        body_t *ball = list_get(balls_sunk, i);
        // cue ball is sunk
        if (!strcmp(body_get_info(ball), "CUE_BALL")) {
            switch_turn = true;
            cue_ball_sunk = true;
            game_state_set_cue_ball_sunk(game_state, true);
        }
        // 8 ball is sunk and all of your own balls are already sunk
        else if (!strcmp(body_get_info(ball), "8_BALL") && game_state_get_current_type(scene_get_game_state(scene)) != NULL
                 && self_balls_done(scene)) {
            winner_num = game_state_get_curr_player_turn(game_state);

        }
        // 8 ball is sunk prematurely
        else if (!strcmp(body_get_info(ball), "8_BALL")) {
            winner_num = 3 - game_state_get_curr_player_turn(game_state);
        }
        // sink one of your own balls
        else if (game_state_get_current_type(game_state) != NULL && !strcmp(body_get_info(ball),
                 game_state_get_current_type(game_state))) {
            self_balls_sunk = true;
        }
        //sink opponent ball
        else if (game_state_get_current_type(game_state) != NULL && strcmp(body_get_info(ball),
                 game_state_get_current_type(game_state))) {
            switch_turn = true;
        }
    }
    if (winner_num != -1) {
      if (cue_ball_sunk) {
        winner_num = 3 - winner_num;
      }
      win_handling(scene, font, winner_num);
      return;
    }
    if (!cue_ball_sunk) {
        game_state_set_cue_ball_sunk(game_state, false);
    }
    else {
        body_t *cue_ball = get_object(scene, "CUE_BALL");
        body_t *cue_stick = get_object(scene, "CUE_STICK");
        SDL_Surface *ball_image = IMG_Load("images/ball_16.png");
        body_set_image(cue_ball, ball_image);
        body_set_centroid(cue_ball, (vector_t){HIGH_RIGHT_CORNER.x * CUE_BALL_DEFAULT_X_FACTOR, HIGH_RIGHT_CORNER.y / 2});
        vector_t cue_centroid = vec_add(body_get_centroid(cue_ball), (vector_t) {BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2, 0});
        body_set_centroid(cue_stick, cue_centroid);
        body_set_origin(cue_stick, body_get_centroid(cue_ball));
    }

    // setting the type for each player
    if (!game_state_get_first_turn(game_state) && game_state_get_player_1_type(game_state) == NULL) {
        if ((cue_ball_sunk && list_size(balls_sunk) > 1) || (!cue_ball_sunk && list_size(balls_sunk) > 0)) {
            for (int i = 0; i < list_size(balls_sunk); i++) {
                body_t *ball = list_get(balls_sunk, i);
                if (strcmp((char *) body_get_info(ball), "CUE_BALL")) {
                    if (game_state_get_curr_player_turn(game_state) == 1) {
                        game_state_set_player_1_type(game_state, (char *) body_get_info(ball));
                        if (!strcmp((char *) body_get_info(ball), "SOLID_BALL")) {
                            SDL_Surface *solid_img = IMG_Load("images/solid_type_ball.png");
                            body_set_image(get_object(scene, "TYPE_INDICATOR"), solid_img);
                            game_state_set_player_2_type(game_state, "STRIPED_BALL");
                        }
                        else {
                            SDL_Surface *striped_img = IMG_Load("images/striped_type_ball.png");
                            body_set_image(get_object(scene, "TYPE_INDICATOR"), striped_img);
                            game_state_set_player_2_type(game_state, "SOLID_BALL");
                        }
                    }
                    else {
                        game_state_set_player_2_type(game_state, (char *) body_get_info(ball));
                        if (!strcmp((char *) body_get_info(ball), "SOLID_BALL")) {
                            SDL_Surface *solid_img = IMG_Load("images/solid_type_ball.png");
                            body_set_image(get_object(scene, "TYPE_INDICATOR"), solid_img);
                            game_state_set_player_1_type(game_state, "STRIPED_BALL");
                        }
                        else {
                            SDL_Surface *striped_img = IMG_Load("images/striped_type_ball.png");
                            body_set_image(get_object(scene, "TYPE_INDICATOR"), striped_img);
                            game_state_set_player_1_type(game_state, "SOLID_BALL");
                        }
                    }
                    self_balls_sunk = true;
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
            change_text(scene, "POWER_TEXT", "", font, GOLD_COLOR_SDL);
        }
        else if (game_state_get_ghost_powerup(game_state)){
            add_ghost_powerup(scene, BALL_MASS);
            game_state_set_ghost_powerup(game_state, false);
            change_text(scene, "POWER_TEXT", "", font, GOLD_COLOR_SDL);
        }
        else if (game_state_get_size_powerdown(game_state)){
            add_size_powerdown(scene, BALL_RADIUS);
            game_state_set_size_powerdown(game_state, false);
            change_text(scene, "POWER_TEXT", "", font, GOLD_COLOR_SDL);
        }
        else if (game_state_get_turn_powerdown(game_state)){
            game_state_set_turn_powerdown(game_state, false);
            change_text(scene, "POWER_TEXT", "POWER DOWN: Sorry, you may no longer play with your balls!",
                        font, GOLD_COLOR_SDL);
        }
        else {
            change_text(scene, "POWER_TEXT", "", font, GOLD_COLOR_SDL);
        }
        add_size_powerdown(scene, BALL_RADIUS);
        game_state_set_curr_player_turn(game_state, 3 - game_state_get_curr_player_turn(game_state));
        if (game_state_get_curr_player_turn(game_state) == 1){
            change_text(scene, "TURN_TEXT", "Player 1", font, WHITE_COLOR_SDL);
            if (game_state_get_player_1_type(game_state) != NULL) {
                if (!strcmp((char *) game_state_get_player_1_type(game_state), "SOLID_BALL")) {
                    SDL_Surface *solid_img = IMG_Load("images/solid_type_ball.png");
                    body_set_image(get_object(scene, "TYPE_INDICATOR"), solid_img);
                }
                else {
                    SDL_Surface *striped_img = IMG_Load("images/striped_type_ball.png");
                    body_set_image(get_object(scene, "TYPE_INDICATOR"), striped_img);
                }
            }
        }
        else{
            change_text(scene, "TURN_TEXT", "Player 2", font, WHITE_COLOR_SDL);
            if (game_state_get_player_2_type(game_state) != NULL) {
                if (!strcmp((char *) game_state_get_player_2_type(game_state), "SOLID_BALL")) {
                    SDL_Surface *solid_img = IMG_Load("images/solid_type_ball.png");
                    body_set_image(get_object(scene, "TYPE_INDICATOR"), solid_img);
                }
                else {
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
}

/**
 * Handles the mouse click actions of the player, such as clicking buttons or moving the
 * cue ball/cue stick/slider.
 * @param key - The mouse button pressed
 * @param type - The mouse event type
 * @param x - The x coordinate of the mouse
 * @param y - The y coordinate of the mouse
 * @param aux - The auxiliary value
 */
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
                 if (x >= QUIT_BUTTON_CENTROID.x - QUIT_BUTTON_SIDE_LENGTH / 2
                 && x <= QUIT_BUTTON_CENTROID.x + QUIT_BUTTON_SIDE_LENGTH / 2
                 && y >= QUIT_BUTTON_CENTROID.y - QUIT_BUTTON_SIDE_LENGTH / 2
                 && y <= QUIT_BUTTON_CENTROID.y + QUIT_BUTTON_SIDE_LENGTH / 2){
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
                        &&
                        y >= body_get_centroid(button).y - BUTTON_WIDTH / 2
                        && y <= body_get_centroid(button).y + BUTTON_WIDTH / 2){
                        sdl_on_motion((motion_handler_t)slider_handler, aux);
                    }
                    // click on cue ball
                    else if (x >= body_get_centroid(cue_ball).x - BALL_RADIUS
                            && x <= body_get_centroid(cue_ball).x + BALL_RADIUS
                            && y >= body_get_centroid(cue_ball).y - BALL_RADIUS
                            && y <= body_get_centroid(cue_ball).y + BALL_RADIUS
                            && (game_state_get_cue_ball_sunk(game_state)
                            || game_state_get_first_turn(game_state))
                            ) {
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
/**
 * Handles keyboard events for the Konami Code.
 * @param key - The key pressed
 * @param type - The type of key event
 * @param held_time - The amount of time the key has been held down
 * @param aux - An auxiliary value
 */
void player_key_handler(char key, key_event_type_t type, double held_time, void * aux) {
    if (type == MOUSE_UP){
        game_state_t *game_state = scene_get_game_state((scene_t *)aux);
        list_t *keys = game_state_get_keys(game_state);
        if (list_size(keys) == KONAMI_CODE_LENGTH) {
            list_remove(keys, 0);
        }
        if (key == LEFT_ARROW) {
            list_add(keys, (void *)LEFT_ARROW_CHAR);
        }
        else if (key == RIGHT_ARROW) {
            list_add(keys, (void *)RIGHT_ARROW_CHAR);
        }
        else if (key == UP_ARROW) {
            list_add(keys, (void *)UP_ARROW_CHAR);
        }
        else if (key == DOWN_ARROW) {
            list_add(keys, (void *)DOWN_ARROW_CHAR);
        }
        else {
            list_add(keys, (void *)key);
        }
    }
}

/**
 * Adds forces between the bodies in the scene.
 * @param scene - The scene containing the bodies
 */
void add_forces(scene_t *scene){
    for(int i = 0; i < scene_bodies(scene) - 1; i++){
        body_t *body1 = scene_get_body(scene, i);
        if(!strcmp(body_get_info(body1), "SOLID_BALL") || !strcmp(body_get_info(body1), "STRIPED_BALL")
           || !strcmp(body_get_info(body1), "8_BALL") || !strcmp(body_get_info(body1), "CUE_BALL")){
            create_friction(scene, MU, G, body1);
            for (int j = i + 1; j < scene_bodies(scene); j++) {
                body_t *body2 = scene_get_body(scene, j);
                if(!strcmp(body_get_info(body2), "SOLID_BALL") || !strcmp(body_get_info(body2), "STRIPED_BALL")
                   || !strcmp(body_get_info(body2), "8_BALL") || !strcmp(body_get_info(body2), "CUE_BALL")
                   || !strcmp(body_get_info(body2), "WALL")){
                    create_collision(scene, body1, body2, (collision_handler_t) balls_collision_handler, NULL, NULL);
                }
                else if(!strcmp(body_get_info(body2), "HOLE")){
                    create_collision(scene, body1, body2, (collision_handler_t) ball_destroy, scene, NULL);
                }
                if(!strcmp(body_get_info(body1), "CUE_BALL") && !strcmp(body_get_info(body2), "CUE_STICK")){
                    create_physics_collision(scene, BALL_ELASTICITY, body1, body2);
                }
            }
        }
    }
}

/**
 * Sets the velocity of bodies in the scene to zero if their velocity is below a certain threshold.
 * @param scene - The scene containing the bodies
 */
void stop_balls(scene_t *scene){
    for (int i = 0; i < scene_bodies(scene); i++) {
        body_t *body = scene_get_body(scene, i);
        vector_t velocity = body_get_velocity(body);
        if (fabs(velocity.x) < VELOCITY_THRESHOLD.x && fabs(velocity.y) < VELOCITY_THRESHOLD.y){
            body_set_velocity(body, VEC_ZERO);
        }
    }
}

/**
 * Checks if the Konami Code has been entered. If so, the
 * Easter Egg is initiated.
 * @param scene - The scene containing the bodies.
 */
void konami_code(scene_t *scene) {
    game_state_t *game_state = scene_get_game_state(scene);
    if (!game_state_get_konami(game_state)) {
        list_t *code = list_init(KONAMI_CODE_LENGTH, NULL);
        list_add(code, (void *)UP_ARROW_CHAR);
        list_add(code, (void *)UP_ARROW_CHAR);
        list_add(code, (void *)DOWN_ARROW_CHAR);
        list_add(code, (void *)DOWN_ARROW_CHAR);
        list_add(code, (void *)LEFT_ARROW_CHAR);
        list_add(code, (void *)RIGHT_ARROW_CHAR);
        list_add(code, (void *)LEFT_ARROW_CHAR);
        list_add(code, (void *)RIGHT_ARROW_CHAR);
        list_add(code, (void *)'b');
        list_add(code, (void *)'a');
        list_t *keys = game_state_get_keys(game_state);
        if (list_size(keys) >= KONAMI_CODE_LENGTH) {
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
            Mix_PlayChannel(BACKGROUND_CHANNEL, background, -1);
            game_state_set_konami(game_state, true);
        }
    }
}

/**
 * Sets up SDL handlers, icon, and title.
 * @param scene - The scene
 */
void SDL_setup(scene_t *scene) {
    sdl_on_mouse((mouse_handler_t)player_mouse_handler, scene);
    sdl_on_key((key_handler_t)player_key_handler, scene);

    SDL_Surface *icon = IMG_Load("images/ball_8.png");
    sdl_set_icon(icon);

    sdl_set_title("Crazy 8 Ball");
}

/**
 * Checks if it is the end of a turn, and if so, calls the gameplay handler.
 * @param scene - The scene containing the bodies
 * @param font - The font of the text
 */
void end_of_turn(scene_t *scene, TTF_Font *font) {
  if (game_state_get_end_of_turn(scene_get_game_state(scene)) && is_balls_stopped(scene)
            && game_state_get_winner(scene_get_game_state(scene)) == NULL){
            vector_t cue_centroid = vec_add(body_get_centroid(get_object(scene, "CUE_BALL")),
                                            (vector_t) {BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2, 0});
            body_set_rotation(get_object(scene, "CUE_STICK"), 0);
            body_set_centroid(get_object(scene, "CUE_STICK"), cue_centroid);
            body_set_origin(get_object(scene, "CUE_STICK"), body_get_centroid(get_object(scene, "CUE_BALL")));
            gameplay_handler(scene, font);
        }
}

void end_game(scene_t*scene) {
    scene_free(scene);
    Mix_Quit();
    IMG_Quit();
    TTF_Quit();
}

bool inited = false;
scene_t *scene;
TTF_Font* font;

void init() {
    sdl_init(LOW_LEFT_CORNER, HIGH_RIGHT_CORNER);
    scene = scene_init();
    game_state_t *game_state = game_state_init();
    scene_set_game_state(scene, game_state);

    TTF_Init();
    font = TTF_OpenFont("fonts/BebasNeue-Regular.TTF", FONT_SIZE);
    game_setup(scene, font);
    SDL_setup(scene);
    add_forces(scene);
}

void c_main(){
        if (!inited) {
            init();
            inited = true;
        }
        if(game_state_get_game_quit(scene_get_game_state(scene))){
            #ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop();
            #else
            exit(0);
            #endif
            end_game(scene);
            return;
        }
        sdl_render_scene(scene);
        scene_tick(scene, time_since_last_tick());
        stop_balls(scene);
        konami_code(scene);
        end_of_turn(scene, font);
        if (sdl_is_done()) {
            #ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop();
            #else
            exit(0);
            #endif
            end_game(scene);
            return;
        }
}


int main() {
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(c_main, 60, 1);
#else
    while (1) {
        c_main();
    }
#endif
}
