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

const vector_t LOW_LEFT_CORNER = {0, 0};
const vector_t HIGH_RIGHT_CORNER = {1500, 900};
const double CIRCLE_POINTS = 30;
const double BALL_RADIUS = 12;
const double TABLE_MASS = INFINITY;
const double BALL_MASS = 10;
const double NUM_BALLS  = 16;
const double CUE_STICK_WIDTH = 400;
const double CUE_STICK_HEIGHT = 10;
const double CUE_STICK_MASS = INFINITY;
const rgb_color_t WHITE_COLOR = {1, 1, 1};
const double TABLE_WIDTH = 800;
const double TABLE_HEIGHT = 480;
const double WALL_THICKNESS = 10;
const double WALL_MASS = INFINITY;
const double WALL_TABLE_WIDTH_DIFF = 140;
const double TABLE_WALL_THICKNESS = 50;
const double WALL_GAP = 20;
const double HOLE_RADIUS = 17.36;
const double HOLE_CONSTANT = 45;
const double CORRECTION_CONSTANT = 16;
const double BREEZY_CONSTANT = 12;
const double BALL_ELASTICITY = 0.6;
const double MU = 0.9;
const double G = 15;
const double DRAG_DIST = 800;
const double SLIDER_WIDTH = 30;
const double SLIDER_HEIGHT = 500;
const double SLIDER_X = 250;
const double BUTTON_WIDTH = 60;
const double BUTTON_HEIGHT = 30;
const double BUTTON_Y = 230;
const double DEFAULT_IMPULSE = 10;
const double CUE_STICK_DEFAULT_Y = 30;
const vector_t VELOCITY_THRESHOLD = {0.5, 0.5};

// // stick force buildup, animation, and ball collision
// body_t *shoot_stick(vector_t initial_position, int direction, double width,

// }

body_t *get_object(scene_t *scene, char *name){
    for (int i = 0; i < scene_bodies(scene); i++) {
        body_t *body = scene_get_body(scene, i);
        if (strcmp(body_get_info(body), name) == 0) {
            return body;
        }
    }
    return NULL;
}

body_t *get_cue_ball(scene_t *scene) {
    for (int i = 0; i < scene_bodies(scene); i++) {
        body_t *body = scene_get_body(scene, i);
        if (strcmp(body_get_info(body),"CUE_BALL") == 0) {
            return body;
        }
    }
    return NULL;
}

body_t *get_cue_stick(scene_t *scene) {
    for (int i = 0; i < scene_bodies(scene); i++) {
        body_t *body = scene_get_body(scene, i);
        if (strcmp(body_get_info(body),"CUE_STICK") == 0) {
            return body;
        }
    }
    return NULL;
}

void rotation_handler(double x, double y, double xrel, double yrel, void *aux) {
    // if on cue ball then move cue ball
    // if on force bar then change prepare to shoot stick
    // printf("origin: x: %f, y: %f\n", body_get_origin(get_cue_stick((scene_t *)aux)).x, body_get_origin(get_cue_stick((scene_t *)aux)).y);
    body_t *ball = get_cue_ball((scene_t *)aux);
    double angle = 2 * M_PI * (sqrt(pow(xrel, 2) + pow(yrel, 2))) / DRAG_DIST;
    // first quadrant
    if (x > body_get_centroid(ball).x && y < body_get_centroid(ball).y){
        // counterclockwise
        if (-1 * yrel >= xrel){
            body_set_rotation(get_cue_stick((scene_t *)aux), body_get_angle(get_cue_stick((scene_t *)aux)) - angle);
            double angle1 = body_get_angle(get_cue_stick((scene_t *)aux)) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1), (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(get_cue_ball((scene_t *)aux)));
            body_set_centroid(get_cue_stick((scene_t *)aux), new_centroid);
        }
        else{
            body_set_rotation(get_cue_stick((scene_t *)aux), body_get_angle(get_cue_stick((scene_t *)aux)) + angle);
            double angle1 = body_get_angle(get_cue_stick((scene_t *)aux)) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1), (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(get_cue_ball((scene_t *)aux)));
            body_set_centroid(get_cue_stick((scene_t *)aux), new_centroid);
        }
    }
    // second quadrant
    else if (x < body_get_centroid(ball).x && y < body_get_centroid(ball).y){
        // counterclockwise
        if (yrel >= xrel){
            body_set_rotation(get_cue_stick((scene_t *)aux), body_get_angle(get_cue_stick((scene_t *)aux)) - angle);
            double angle1 = body_get_angle(get_cue_stick((scene_t *)aux)) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1), (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(get_cue_ball((scene_t *)aux)));
            body_set_centroid(get_cue_stick((scene_t *)aux), new_centroid);
        }
        else{
            body_set_rotation(get_cue_stick((scene_t *)aux), body_get_angle(get_cue_stick((scene_t *)aux)) + angle);
            double angle1 = body_get_angle(get_cue_stick((scene_t *)aux)) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1), (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(get_cue_ball((scene_t *)aux)));
            body_set_centroid(get_cue_stick((scene_t *)aux), new_centroid);
        }
    }
    // third quadrant
    else if (x < body_get_centroid(ball).x && y > body_get_centroid(ball).y){
        // counterclockwise
        if (-1 * yrel <= xrel){
            body_set_rotation(get_cue_stick((scene_t *)aux), body_get_angle(get_cue_stick((scene_t *)aux)) - angle);
            double angle1 = body_get_angle(get_cue_stick((scene_t *)aux)) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1), (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(get_cue_ball((scene_t *)aux)));
            body_set_centroid(get_cue_stick((scene_t *)aux), new_centroid);
        }
        else{
            body_set_rotation(get_cue_stick((scene_t *)aux), body_get_angle(get_cue_stick((scene_t *)aux)) + angle);
            double angle1 = body_get_angle(get_cue_stick((scene_t *)aux)) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1), (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(get_cue_ball((scene_t *)aux)));
            body_set_centroid(get_cue_stick((scene_t *)aux), new_centroid);
        }
    }
    // fourth quandrant
    else if (x > body_get_centroid(ball).x && y > body_get_centroid(ball).y){
        // counterclockwise
        if (yrel <= xrel){
            body_set_rotation(get_cue_stick((scene_t *)aux), body_get_angle(get_cue_stick((scene_t *)aux)) - angle);
            double angle1 = body_get_angle(get_cue_stick((scene_t *)aux)) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1), (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(get_cue_ball((scene_t *)aux)));
            body_set_centroid(get_cue_stick((scene_t *)aux), new_centroid);
        }
        else{
            body_set_rotation(get_cue_stick((scene_t *)aux), body_get_angle(get_cue_stick((scene_t *)aux)) + angle);
            double angle1 = body_get_angle(get_cue_stick((scene_t *)aux)) - angle;
            vector_t new_centroid = {(BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * cos(angle1), (BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2) * sin(angle1)};
            new_centroid = vec_add(new_centroid, body_get_centroid(get_cue_ball((scene_t *)aux)));
            body_set_centroid(get_cue_stick((scene_t *)aux), new_centroid);
        }
    }
    // printf("mouse motion - x: %f, y: %f, xrel: %f, yrel: %f\n", x, y, xrel, yrel);
}

void slider_handler(double x, double y, double xrel, double yrel, void *aux) {
    body_t *button = get_object((scene_t *) aux, "BUTTON");
    body_t *cue_stick = get_object((scene_t *) aux, "CUE_STICK");
    if (y >= BUTTON_Y && y <= HIGH_RIGHT_CORNER.y - BUTTON_Y){
        body_set_centroid(button, (vector_t) {SLIDER_X, y});

        // body_set_centroid(cue_stick, (vector_t) {body_get_centroid(cue_stick).x + (-cos(body_get_angle(cue_stick)) * (y - yrel)),
        //     body_get_centroid(cue_stick).x + (-sin(body_get_angle(cue_stick)) * (y - yrel))});
    }
}

void shoot_handler(double y, void *aux){
    body_t *button = get_object((scene_t *) aux, "BUTTON");
    body_t *cue_ball = get_object((scene_t *) aux, "CUE_BALL");
    body_t *cue_stick = get_object((scene_t *) aux, "CUE_STICK");
    if (body_get_centroid(button).y != BUTTON_Y){
        body_set_centroid(cue_stick, (vector_t){HIGH_RIGHT_CORNER.x / 2, CUE_STICK_DEFAULT_Y});
        body_set_origin(cue_stick, (vector_t){HIGH_RIGHT_CORNER.x / 2, CUE_STICK_DEFAULT_Y});
        double impulse_factor = y - BUTTON_Y;
        vector_t impulse = {impulse_factor * DEFAULT_IMPULSE * -cos(body_get_angle(cue_stick)), impulse_factor * DEFAULT_IMPULSE * -sin(body_get_angle(cue_stick))};
        body_add_impulse(cue_ball, impulse);
        body_set_rotation(cue_stick, 0);
        // set end of turn to TRUE
        game_state_set_end_of_turn(scene_get_game_state((scene_t *)aux), true);
    }
    body_set_centroid(button, (vector_t) {SLIDER_X, BUTTON_Y});
}

bool is_balls_stopped(scene_t *scene) {
    for (int i = 0; i < scene_bodies(scene); i++) {
        body_t *body = scene_get_body(scene, i);
        vector_t velocity = body_get_velocity(body);
        if (velocity.x > VELOCITY_THRESHOLD.x || velocity.y > VELOCITY_THRESHOLD.y){
            return false;
        }
    }
    return true;
}

bool self_balls_done(scene_t *scene) {
    for (int i = 0; i < scene_bodies(scene); i++) {
        body_t *ball = scene_get_body(scene, i);
        if (!strcmp(body_get_info(ball), game_state_get_current_type(scene_get_game_state(scene)))){
            return false;
        }
    }
    return true;
}

void gameplay_handler(scene_t *scene) {
    game_state_t *game_state = scene_get_game_state(scene);
    if (game_state_get_player_1_type(game_state) != NULL && game_state_get_player_2_type(game_state) != NULL) {
        printf("current turn: %d 1_type: %s, 2_type: %s\n", game_state_get_curr_player_turn(game_state), game_state_get_player_1_type(game_state), game_state_get_player_2_type(game_state));
    }
    else {
        printf("current turn: %d\n", game_state_get_curr_player_turn(game_state));
    }

    bool switch_turn = false;
    bool self_balls_sunk = false;
    bool cue_ball_sunk = false;

    list_t *balls_sunk = game_state_get_balls_sunk(game_state);
    for (int i = 0; i < list_size(balls_sunk); i++) {
        body_t *ball = list_get(balls_sunk, i);
        // cue ball is sunk
        if (!strcmp(body_get_info(ball), "CUE_BALL")) {
            switch_turn = true;
            cue_ball_sunk = true;
            //TODO cue ball placement
        }
        // 8 ball is sunk and all of your own balls are already sunk
        else if (!strcmp(body_get_info(ball), "8_BALL") && self_balls_done(scene)) {
            //TODO win condition
            //break
        }
        // 8 ball is sunk prematurely
        else if (!strcmp(body_get_info(ball), "8_BALL")) {
            //TODO loss condition
            //break
        }
        // sink one of your own balls
        else if (game_state_get_current_type(game_state) != NULL && !strcmp(body_get_info(ball), game_state_get_current_type(game_state))) {
            self_balls_sunk = true;
        }
    }
    if (!game_state_get_first_turn(game_state) && game_state_get_player_1_type(game_state) == NULL) {
        if ((cue_ball_sunk && list_size(balls_sunk) > 1) || (!cue_ball_sunk && list_size(balls_sunk) > 0)) {
            for (int i = 0; i < list_size(balls_sunk); i++) {
                body_t *ball = list_get(balls_sunk, i);
                if (strcmp(body_get_info(ball), "CUE_BALL")) {
                    if (game_state_get_curr_player_turn(game_state) == 1) {
                        game_state_set_player_1_type(game_state, body_get_info(ball));
                        if (!strcmp(body_get_info(ball), "SOLID_BALL")) {
                            game_state_set_player_2_type(game_state, "STRIPED_BALL");
                        }
                        else {
                            game_state_set_player_2_type(game_state, "SOLID_BALL");
                        }
                    }
                    else {
                        game_state_set_player_2_type(game_state, body_get_info(ball));
                        if (!strcmp(body_get_info(ball), "SOLID_BALL")) {
                            game_state_set_player_1_type(game_state, "STRIPED_BALL");
                        }
                        else {
                            game_state_set_player_1_type(game_state, "SOLID_BALL");
                        }
                    }
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
        game_state_set_curr_player_turn(game_state, 3 - game_state_get_curr_player_turn(game_state));
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
    game_state_set_first_turn(game_state, false);
}

// // stick rotation
void player_mouse_handler(int key, mouse_event_type_t type, double x, double y, void *aux) {
    if (key == SDL_BUTTON_LEFT) {
        if (type == MOUSE_DOWN) {
            if (game_state_get_end_of_turn(scene_get_game_state((scene_t *) aux))) {
                gameplay_handler((scene_t *)aux);
            }
            // printf("mouse down  - x: %f, y: %f\n", x, y);
            body_t *button = get_object((scene_t *) aux, "BUTTON");
            if (x >= body_get_centroid(button).x - BUTTON_WIDTH / 2
                && x <= body_get_centroid(button).x + BUTTON_WIDTH / 2
                && y >= body_get_centroid(button).y - BUTTON_WIDTH / 2
                && y <= body_get_centroid(button).y + BUTTON_WIDTH / 2){
                    sdl_on_motion((motion_handler_t)slider_handler, aux);
            }
            else{
                vector_t cue_centroid = vec_add(body_get_centroid(get_cue_ball((scene_t *)aux)), (vector_t) {BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2, 0});
                body_set_centroid(get_cue_stick((scene_t *)aux), cue_centroid);
                body_set_origin(get_cue_stick((scene_t *)aux), body_get_centroid(get_cue_ball((scene_t *)aux)));
                sdl_on_motion((motion_handler_t)rotation_handler, aux);
            }
        }
        if (type == MOUSE_UP) {
            // printf("mouse up - x: %f, y: %f\n", x, y);
            sdl_on_motion((NULL), NULL);
            shoot_handler(y, aux);
        }
    }
}

list_t *rect_init(double width, double height) {
    vector_t half_width  = {.x = width / 2, .y = 0.0},
             half_height = {.x = 0.0, .y = height / 2};
    list_t *rect = list_init(4, free);
    vector_t *v = malloc(sizeof(*v));
    *v = vec_add(half_width, half_height);
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = vec_subtract(half_height, half_width);
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = vec_negate(*(vector_t *) list_get(rect, 0));
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = vec_subtract(half_width, half_height);
    list_add(rect, v);
    return rect;
}

list_t *circle_init(double radius){
    list_t *circle_list = list_init(CIRCLE_POINTS, (free_func_t) free);
    for (size_t i = 0; i < CIRCLE_POINTS - 1; i++) {
        double angle = 2 * M_PI * i / CIRCLE_POINTS;
        vector_t *v = malloc(sizeof(vector_t));
        *v = (vector_t) {radius * cos(angle), radius * sin(angle)};
        list_add(circle_list, (void *) v);
    }
    return circle_list;
}


void add_stick(scene_t * scene) {
    list_t *stick_shape = rect_init(CUE_STICK_WIDTH, CUE_STICK_HEIGHT);
    SDL_Surface *ball_image = IMG_Load("images/cue_stick.png");
    body_t *cue_stick = body_init_with_info(stick_shape, CUE_STICK_MASS, (rgb_color_t) {1, 0, 0}, ball_image, CUE_STICK_WIDTH, CUE_STICK_HEIGHT, "CUE_STICK", NULL);
    vector_t cue_centroid = vec_add(body_get_centroid(get_cue_ball(scene)), (vector_t) {BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2, 0});
    body_set_centroid(cue_stick, cue_centroid);
    body_set_origin(cue_stick, body_get_centroid(get_cue_ball(scene)));
    scene_add_body(scene, cue_stick);
}

void add_table(scene_t *scene) {
    list_t *rect = rect_init(1000, 600);
    SDL_Surface *table_image = IMG_Load("images/pool_table.png");
    body_t *pool_table = body_init_with_info(rect, TABLE_MASS, (rgb_color_t) {0,0,0}, table_image, TABLE_WIDTH, TABLE_HEIGHT, "POOL_TABLE", NULL);//magic numbers
    body_set_centroid(pool_table, (vector_t) {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2});
    scene_add_body(scene, pool_table);
}

void add_slider(scene_t *scene){
    list_t *slider_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *slider_image = IMG_Load("images/dark_slider.png");
    body_t *slider = body_init_with_info(slider_list, INFINITY, (rgb_color_t) {0,0,0}, slider_image, SLIDER_WIDTH, SLIDER_HEIGHT, "SLIDER", NULL);//magic numbers
    body_set_centroid(slider, (vector_t) {SLIDER_X, HIGH_RIGHT_CORNER.y / 2});
    scene_add_body(scene, slider);

    list_t *button_list = rect_init(BUTTON_WIDTH, BUTTON_HEIGHT);
    SDL_Surface *button_image = IMG_Load("images/light_slider.png");
    body_t *button = body_init_with_info(button_list, INFINITY, (rgb_color_t) {0,0,0}, button_image, BUTTON_WIDTH, BUTTON_HEIGHT, "BUTTON", NULL);//magic numbers
    body_set_centroid(button, (vector_t) {SLIDER_X, BUTTON_Y});
    scene_add_body(scene, button);
}

void add_balls(scene_t *scene) {
    list_t *balls = list_init(NUM_BALLS, (free_func_t) body_free);
    vector_t initial_position = {HIGH_RIGHT_CORNER.x * 3 / 7 - BALL_RADIUS * 9, HIGH_RIGHT_CORNER.y / 2 - BALL_RADIUS * 4}; //magic numbers
    for(int i = 1; i <= 16; i++){
        char name[20];
        snprintf(name, 20, "images/ball_%d.png", i);
        SDL_Surface *ball_image = IMG_Load(name);
        body_t *pool_ball;
        if (i < 8) {
            pool_ball = body_init_with_info(circle_init(BALL_RADIUS), BALL_MASS, WHITE_COLOR, ball_image,
                                        2*BALL_RADIUS, 2*BALL_RADIUS, "SOLID_BALL", NULL);
        }
        else if (i == 8) {
            pool_ball = body_init_with_info(circle_init(BALL_RADIUS), BALL_MASS, WHITE_COLOR, ball_image,
                                        2*BALL_RADIUS, 2*BALL_RADIUS, "8_BALL", NULL);
        }
        else if (i == 16) {
            pool_ball = body_init_with_info(circle_init(BALL_RADIUS), BALL_MASS, WHITE_COLOR, ball_image,
                                        2*BALL_RADIUS, 2*BALL_RADIUS, "CUE_BALL", NULL);
        }
        else {
            pool_ball = body_init_with_info(circle_init(BALL_RADIUS), BALL_MASS, WHITE_COLOR, ball_image,
                                        2*BALL_RADIUS, 2*BALL_RADIUS, "STRIPED_BALL", NULL);
        }

        if (i != 16) { //don't want the white cue ball in the triangle - magic number?
            list_add(balls, pool_ball);
        }
        else {
            body_set_centroid(pool_ball, (vector_t){HIGH_RIGHT_CORNER.x * 4 / 7, HIGH_RIGHT_CORNER.y / 2});
        }
        scene_add_body(scene, pool_ball);
    }

    int columns = 5;
    int y = 0;
    int x = 0;
    int top_column_y = initial_position.y;
    for (int i = 0; i < columns; i++) {
        for (int j = columns - 1; j >= i; j--) {
            int ball_num = rand() % list_size(balls);
            body_set_centroid(list_get(balls, ball_num), (vector_t) {initial_position.x + x, top_column_y + y});
            list_remove(balls, ball_num);
            y += BALL_RADIUS * 2;
        }
        x += BALL_RADIUS * 2;
        top_column_y += BALL_RADIUS;
        y = 0;
    }
}

void add_walls(scene_t *scene) {
    // top wall
    double wall_width = (TABLE_WIDTH - WALL_TABLE_WIDTH_DIFF) / 2 - WALL_GAP - BREEZY_CONSTANT;

    list_t *wall_shape_top1 = rect_init(wall_width, WALL_THICKNESS);
    body_t *wall_top1 = body_init_with_info(wall_shape_top1, WALL_MASS, (rgb_color_t){0, 1, 0}, NULL, wall_width, WALL_THICKNESS, "WALL", NULL);
    vector_t wall_centroid_top1 = vec_add(body_get_centroid(wall_top1), (vector_t) {HIGH_RIGHT_CORNER.x / 2 - wall_width / 2 - WALL_GAP, HIGH_RIGHT_CORNER.y / 2 + TABLE_HEIGHT / 2 - TABLE_WALL_THICKNESS});
    body_set_centroid(wall_top1, wall_centroid_top1);
    scene_add_body(scene, wall_top1);

    list_t *wall_shape_top2 = rect_init(wall_width, WALL_THICKNESS);
    body_t *wall_top2 = body_init_with_info(wall_shape_top2, WALL_MASS, (rgb_color_t){0, 1, 0}, NULL, wall_width, WALL_THICKNESS, "WALL", NULL);
    vector_t wall_centroid_top2 = vec_add(body_get_centroid(wall_top2), (vector_t) {HIGH_RIGHT_CORNER.x / 2 + wall_width / 2 + WALL_GAP, HIGH_RIGHT_CORNER.y / 2 + TABLE_HEIGHT / 2 - TABLE_WALL_THICKNESS});
    body_set_centroid(wall_top2, wall_centroid_top2);
    scene_add_body(scene, wall_top2);

    //bottom wall
    list_t *wall_shape_bot1 = rect_init(wall_width, WALL_THICKNESS);
    body_t *wall_bot1 = body_init_with_info(wall_shape_bot1, WALL_MASS, (rgb_color_t){0, 1, 0}, NULL, wall_width, WALL_THICKNESS, "WALL", NULL);
    vector_t wall_centroid_bot1 = vec_add(body_get_centroid(wall_bot1), (vector_t) {HIGH_RIGHT_CORNER.x / 2 - wall_width / 2 - WALL_GAP, HIGH_RIGHT_CORNER.y / 2 - TABLE_HEIGHT / 2 + TABLE_WALL_THICKNESS});
    body_set_centroid(wall_bot1, wall_centroid_bot1);
    scene_add_body(scene, wall_bot1);

    list_t *wall_shape_bot2 = rect_init(wall_width, WALL_THICKNESS);
    body_t *wall_bot2 = body_init_with_info(wall_shape_bot2, WALL_MASS, (rgb_color_t){0, 1, 0}, NULL, wall_width, WALL_THICKNESS, "WALL", NULL);
    vector_t wall_centroid_bot2 = vec_add(body_get_centroid(wall_bot2), (vector_t) {HIGH_RIGHT_CORNER.x / 2 + wall_width / 2 + WALL_GAP, HIGH_RIGHT_CORNER.y / 2 - TABLE_HEIGHT / 2 + TABLE_WALL_THICKNESS});
    body_set_centroid(wall_bot2, wall_centroid_bot2);
    scene_add_body(scene, wall_bot2);

    //right wall
    double wall_height = TABLE_HEIGHT - WALL_TABLE_WIDTH_DIFF - BREEZY_CONSTANT;
    list_t *wall_shape_right = rect_init(WALL_THICKNESS, wall_height);
    body_t *wall_right = body_init_with_info(wall_shape_right, WALL_MASS, (rgb_color_t){0, 1, 0}, NULL, WALL_THICKNESS, wall_height, "WALL", NULL);
    vector_t wall_centroid_right = vec_add(body_get_centroid(wall_right), (vector_t) {HIGH_RIGHT_CORNER.x / 2 + TABLE_WIDTH / 2 - TABLE_WALL_THICKNESS, HIGH_RIGHT_CORNER.y / 2});
    body_set_centroid(wall_right, wall_centroid_right);
    scene_add_body(scene, wall_right);
    //left wall
    list_t *wall_shape_left = rect_init(WALL_THICKNESS, wall_height);
    body_t *wall_left = body_init_with_info(wall_shape_left, WALL_MASS, (rgb_color_t){0, 1, 0}, NULL, WALL_THICKNESS, wall_height, "WALL", NULL);
    vector_t wall_centroid_left = vec_add(body_get_centroid(wall_left), (vector_t) {HIGH_RIGHT_CORNER.x / 2 - TABLE_WIDTH / 2 + TABLE_WALL_THICKNESS, HIGH_RIGHT_CORNER.y / 2});
    body_set_centroid(wall_left, wall_centroid_left);
    scene_add_body(scene, wall_left);
}

void add_holes(scene_t *scene){
    list_t *hole_shape1 = circle_init(HOLE_RADIUS);
    body_t *hole_top1 = body_init_with_info(hole_shape1, INFINITY, (rgb_color_t){1, 0, 0}, NULL, HOLE_RADIUS * 2, HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top1 = (vector_t) {HIGH_RIGHT_CORNER.x / 2 - TABLE_WIDTH / 2 + HOLE_CONSTANT, HIGH_RIGHT_CORNER.y / 2 + TABLE_HEIGHT / 2 - HOLE_CONSTANT};
    body_set_centroid(hole_top1, hole_centroid_top1);
    scene_add_body(scene, hole_top1);

    list_t *hole_shape2 = circle_init(HOLE_RADIUS);
    body_t *hole_top2 = body_init_with_info(hole_shape2, INFINITY, (rgb_color_t){1, 0, 0}, NULL, HOLE_RADIUS * 2, HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top2 = (vector_t) {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2 + TABLE_HEIGHT / 2 - HOLE_CONSTANT + CORRECTION_CONSTANT};
    body_set_centroid(hole_top2, hole_centroid_top2);
    scene_add_body(scene, hole_top2);

    list_t *hole_shape3 = circle_init(HOLE_RADIUS);
    body_t *hole_top3 = body_init_with_info(hole_shape3, INFINITY, (rgb_color_t){1, 0, 0}, NULL, HOLE_RADIUS * 2, HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top3 = (vector_t) {HIGH_RIGHT_CORNER.x / 2 + TABLE_WIDTH / 2 - HOLE_CONSTANT, HIGH_RIGHT_CORNER.y / 2 + TABLE_HEIGHT / 2 - HOLE_CONSTANT};
    body_set_centroid(hole_top3, hole_centroid_top3);
    scene_add_body(scene, hole_top3);

    list_t *hole_shape4 = circle_init(HOLE_RADIUS);
    body_t *hole_top4 = body_init_with_info(hole_shape4, INFINITY, (rgb_color_t){1, 0, 0}, NULL, HOLE_RADIUS * 2, HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top4 = (vector_t) {HIGH_RIGHT_CORNER.x / 2 - TABLE_WIDTH / 2 + HOLE_CONSTANT, HIGH_RIGHT_CORNER.y / 2 - TABLE_HEIGHT / 2 + HOLE_CONSTANT};
    body_set_centroid(hole_top4, hole_centroid_top4);
    scene_add_body(scene, hole_top4);

    list_t *hole_shape5 = circle_init(HOLE_RADIUS);
    body_t *hole_top5 = body_init_with_info(hole_shape5, INFINITY, (rgb_color_t){1, 0, 0}, NULL, HOLE_RADIUS * 2, HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top5 = (vector_t) {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2 - TABLE_HEIGHT / 2 + HOLE_CONSTANT - CORRECTION_CONSTANT};
    body_set_centroid(hole_top5, hole_centroid_top5);
    scene_add_body(scene, hole_top5);

    list_t *hole_shape6 = circle_init(HOLE_RADIUS);
    body_t *hole_top6 = body_init_with_info(hole_shape6, INFINITY, (rgb_color_t){1, 0, 0}, NULL, HOLE_RADIUS * 2, HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top6 = (vector_t) {HIGH_RIGHT_CORNER.x / 2 + TABLE_WIDTH / 2 - HOLE_CONSTANT, HIGH_RIGHT_CORNER.y / 2 - TABLE_HEIGHT / 2 + HOLE_CONSTANT};
    body_set_centroid(hole_top6, hole_centroid_top6);
    scene_add_body(scene, hole_top6);
}

void game_setup(scene_t *scene){
    add_table(scene);
    add_balls(scene);
    add_stick(scene);
    add_walls(scene);
    add_holes(scene);
    add_slider(scene);
}

void ball_destroy(body_t *ball, body_t *hole, vector_t axis, void *aux) {
    list_add(game_state_get_balls_sunk(scene_get_game_state((scene_t *) aux)), ball);
    body_set_image(ball, NULL);
}

void add_forces(scene_t *scene){
    for(int i = 0; i < scene_bodies(scene) - 1; i++){
        body_t *body1 = scene_get_body(scene, i);
        if(!strcmp(body_get_info(body1), "SOLID_BALL") || !strcmp(body_get_info(body1), "STRIPED_BALL") || !strcmp(body_get_info(body1), "8_BALL") || !strcmp(body_get_info(body1), "CUE_BALL")){
            create_friction(scene, MU, G, body1);
            for (int j = i + 1; j < scene_bodies(scene); j++) {
                body_t *body2 = scene_get_body(scene, j);
                if(!strcmp(body_get_info(body2), "SOLID_BALL") || !strcmp(body_get_info(body2), "STRIPED_BALL") || !strcmp(body_get_info(body2), "8_BALL") || !strcmp(body_get_info(body2), "CUE_BALL") || !strcmp(body_get_info(body2), "WALL")){
                    create_physics_collision(scene, BALL_ELASTICITY, body1, body2);
                }
                else if(!strcmp(body_get_info(body2), "HOLE")){
                    create_collision(scene, body1, body2, (collision_handler_t) ball_destroy, NULL, NULL);
                }
                if(!strcmp(body_get_info(body1), "CUE_BALL") && !strcmp(body_get_info(body2), "CUE_STICK")){
                    create_physics_collision(scene, BALL_ELASTICITY, body1, body2);
                }
            }
        }
    }
}

int main(){
    scene_t *scene = scene_init();
    game_state_t *game_state = game_state_init();
    scene_set_game_state(scene, game_state);
    game_setup(scene);
    SDL_Renderer *renderer = sdl_init(LOW_LEFT_CORNER, HIGH_RIGHT_CORNER);
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 0);
    add_forces(scene);
    sdl_on_mouse((mouse_handler_t)player_mouse_handler, scene);

    while (!sdl_is_done()){
        sdl_render_scene(scene);
        scene_tick(scene, time_since_last_tick());
    }
    scene_free(scene);
}
