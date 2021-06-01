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

const vector_t LOW_LEFT_CORNER = {0, 0};
const vector_t HIGH_RIGHT_CORNER = {1500, 900};
const double CIRCLE_POINTS = 10;
const double BALL_RADIUS = 12;
const double TABLE_MASS = INFINITY;
const double BALL_MASS = 10;
const double NUM_BALLS  = 16;
const double CUE_STICK_WIDTH = 400;
const double CUE_STICK_HEIGHT = 10;
const double CUE_STICK_MASS = INFINITY;
const rgb_color_t WHITE_COLOR = {1, 1, 1, 1};
const SDL_Color WHITE_COLOR_SDL = {255, 255, 255};
const SDL_Color BLACK_COLOR = {0, 0, 0};
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
const double DRAG_DIST = 2400;
const double SLIDER_WIDTH = 50;
const double SLIDER_HEIGHT = 500;
const double SLIDER_X = 250;
const double BUTTON_WIDTH = 80;
const double BUTTON_HEIGHT = 35;
const double BUTTON_Y = 230;
const double DEFAULT_IMPULSE = 10;
const double CUE_STICK_DEFAULT_Y = 177;
const vector_t VELOCITY_THRESHOLD = {0.5, 0.5};
const double TINY_CONSTANT = 0.8;
const double PULL_FACTOR_ADJUSTMENT_CONSTANT = 47;
const double SIZE_POWERDOWN_ADJUSTMENT_SCALE_FACTOR = 1.265;
const double POWER_TEXT_Y = 800;
const double START_MENU_BUTTON_SIDE_LENGTH = 200;
const double RECTANGULAR_BUTTON_WIDTH = 490;
const double RECTANGULAR_BUTTON_HEIGHT = 110;
const double TITLE_TEXT_Y = 250;
const double PLAY_BUTTON_X = 550;
const double INFO_BUTTON_X = 950;
const double QUIT_BUTTON_Y = 650;
const vector_t QUIT_BUTTON_CENTROID = {1470, 50};
const vector_t HELP_BUTTON_CENTROID = {1400, 50};
const double QUIT_BUTTON_SIDE_LENGTH = 50;
const vector_t START_PLAY_BUTTON_CENTROID = {750, 410};
const vector_t START_INSTRUCTIONS_BUTTON_CENTROID = {750, 560};
const vector_t START_QUIT_BUTTON_CENTROID = {750, 710};
const double BACKLOG_FORCE_CONSTANT_TO_SATISFY_PATRICKS_DESIRES = 100;

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

bool overlaps(double x, double y, vector_t centroid){
    if (x < centroid.x + BALL_RADIUS * 2 && x > centroid.x - BALL_RADIUS * 2 && y < centroid.y + BALL_RADIUS * 2
                    && y > centroid.y - BALL_RADIUS * 2){
                        return true;
                    }
    return false;
}



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

void up_down_handler(double x, double y, double xrel, double yrel, void *aux) {
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

body_t *create_ball(scene_t *scene, char *info, SDL_Surface *img){
    body_t *ball = body_init_with_info(circle_init(BALL_RADIUS), BALL_MASS, WHITE_COLOR, img,
                                        2*BALL_RADIUS, 2*BALL_RADIUS, info, NULL);
    // scene_add_body(scene, ball);
    return ball;
}

void add_size_powerdown(scene_t *scene, double radius){
    char *own_type = game_state_get_current_type(scene_get_game_state(scene));
    if (own_type != NULL ) {
        for(size_t i = 0; i < scene_bodies(scene); i++){
        body_t *body = scene_get_body(scene, i);
        if(!strcmp(body_get_info(body), own_type)){
            list_t *new_circle = circle_init(radius);
            body_set_shape(body, new_circle, body_get_centroid(body));
            body_set_height(body, 2 * radius);
            body_set_width(body, 2 * radius);
        }
    }
    }
}

void add_ghost_powerup(scene_t *scene, double mass){
    char *opponent_type[13];

    if(!strcmp(game_state_get_current_type(scene_get_game_state(scene)), "SOLID_BALL")){
        snprintf(opponent_type, 13, "STRIPED_BALL");
    }
    else{
        snprintf(opponent_type, 11, "SOLID_BALL");
    }

    for(size_t i = 0; i < scene_bodies(scene); i++){
        body_t *body = scene_get_body(scene, i);
        if(!strcmp(body_get_info(body), opponent_type)){
            body_set_mass(body, mass);
            if(mass == 0){
                body_set_color(body, (rgb_color_t) {148 / 255, 148 / 255, 148 / 255, 0.5});
            }
            else{
                body_set_color(body, WHITE_COLOR);
            }
        }
    }

}

void add_balls_powerup(scene_t *scene){
    list_t *ball_list = list_init(2, (free_func_t) body_free);

    if (!strcmp(game_state_get_current_type(scene_get_game_state(scene)), "SOLID_BALL")){
        for (int i = 0; i < 2; i++){
            SDL_Surface *image = IMG_Load("images/special_striped_ball.png");
            body_t *ball = create_ball(scene, "STRIPED_BALL", image);
            list_add(ball_list, ball);
        }
    }
    else {
        for (int i = 0; i < 2; i++){
            SDL_Surface *image = IMG_Load("images/special_solid_ball.png");
            body_t *ball = create_ball(scene, "SOLID_BALL", image);
            list_add(ball_list, ball);
        }
    }

    double maxx = body_get_centroid(get_object(scene, "POOL_TABLE")).x + TABLE_WIDTH / 2 - TABLE_WALL_THICKNESS - WALL_THICKNESS / 2 - BALL_RADIUS;
    double minx = body_get_centroid(get_object(scene, "POOL_TABLE")).x - TABLE_WIDTH / 2 + TABLE_WALL_THICKNESS  + WALL_THICKNESS / 2 + BALL_RADIUS;
    double maxy = body_get_centroid(get_object(scene, "POOL_TABLE")).y + TABLE_HEIGHT / 2 - TABLE_WALL_THICKNESS - WALL_THICKNESS / 2 - BALL_RADIUS;
    double miny = body_get_centroid(get_object(scene, "POOL_TABLE")).y - TABLE_HEIGHT/ 2 + TABLE_WALL_THICKNESS + WALL_THICKNESS / 2 + BALL_RADIUS;

    for (int i = 0; i < list_size(ball_list); i++){
        int xcoord;
        int ycoord;
        bool brake = false;
        while (!brake){
            xcoord = rand() / (float) RAND_MAX * (maxx - minx) + minx;
            ycoord = rand() / (float) RAND_MAX * (maxy - miny) + miny;

            for (int i = 0; i < scene_bodies(scene); i++) {
                body_t *body = scene_get_body(scene, i);
                if (!strcmp(body_get_info(body), "STRIPED_BALL") || !strcmp(body_get_info(body), "SOLID_BALL") || !strcmp(body_get_info(body), "8_BALL")) {
                    vector_t ball_centroid = body_get_centroid(body);
                    if (!overlaps(xcoord, ycoord, ball_centroid)) {
                        brake = true;
                    }
                    else{
                        brake = false;
                        break;
                    }
                }
            }
        }
        body_set_centroid(list_get(ball_list, i), (vector_t) {xcoord, ycoord});
    }

    int channel_num = 3;
    for (int i = 0; i < list_size(ball_list); i++){
        body_t *ball = list_get(ball_list, i);
        create_friction(scene, MU, G, ball);
        for(int j = 0; j < scene_bodies(scene); j++){
            body_t *body = scene_get_body(scene, j);
            if(!strcmp(body_get_info(body), "SOLID_BALL") || !strcmp(body_get_info(body), "STRIPED_BALL") || !strcmp(body_get_info(body), "8_BALL") || !strcmp(body_get_info(body), "CUE_BALL") || !strcmp(body_get_info(body), "WALL")){
                int *aux = malloc(sizeof(int));
                *aux = channel_num;
                create_collision(scene, ball, body, (collision_handler_t) balls_collision_handler, aux, NULL);
                channel_num++;
            }
            else if(!strcmp(body_get_info(body), "HOLE")){
                // int *aux = malloc(sizeof(int));
                // *aux = channel_num;
                create_collision(scene, ball, body, (collision_handler_t) ball_destroy, scene, NULL);
                // channel_num++;
            }
        }
        scene_add_body(scene, ball);
    }
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
                add_balls_powerup(scene);
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

void add_instructions(scene_t *scene){
    list_t *instr_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *instr_image = IMG_Load("images/instructions-background.png");
    body_t *instr = body_init_with_info(instr_list, INFINITY, (rgb_color_t) {0,0,0,1}, instr_image, HIGH_RIGHT_CORNER.x, HIGH_RIGHT_CORNER.y, "INSTRUCTIONS", NULL);//magic numbers
    body_set_centroid(instr, (vector_t) {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2});
    scene_add_body(scene, instr);

    // instructions back button
    list_t *instr_quit_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *instr_quit_image = IMG_Load("images/x-button.png");
    body_t *instr_quit_button = body_init_with_info(instr_quit_list, INFINITY, (rgb_color_t) {0,0,0,1}, instr_quit_image, QUIT_BUTTON_SIDE_LENGTH, QUIT_BUTTON_SIDE_LENGTH, "INSTR_QUIT", NULL);
    body_set_centroid(instr_quit_button, QUIT_BUTTON_CENTROID);
    scene_add_body(scene, instr_quit_button);
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
                        add_instructions((scene_t *)aux);
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
                    add_instructions((scene_t *)aux);
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
                            sdl_on_motion((motion_handler_t)up_down_handler, aux);
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

void add_stick(scene_t * scene) {
    list_t *stick_shape = rect_init(CUE_STICK_WIDTH, CUE_STICK_HEIGHT);
    SDL_Surface *ball_image = IMG_Load("images/cue_stick.png");
    body_t *cue_stick = body_init_with_info(stick_shape, CUE_STICK_MASS, (rgb_color_t) {1, 0, 0, 1}, ball_image, CUE_STICK_WIDTH, CUE_STICK_HEIGHT, "CUE_STICK", NULL);
    vector_t cue_centroid = vec_add(body_get_centroid(get_cue_ball(scene)), (vector_t) {BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2, 0});
    body_set_centroid(cue_stick, cue_centroid);
    body_set_origin(cue_stick, body_get_centroid(get_cue_ball(scene)));
    scene_add_body(scene, cue_stick);
}

void add_table(scene_t *scene) {
    list_t *rect = rect_init(1000, 600);
    SDL_Surface *table_image = IMG_Load("images/pool_table.png");
    body_t *pool_table = body_init_with_info(rect, TABLE_MASS, (rgb_color_t) {0,0,0,1}, table_image, TABLE_WIDTH, TABLE_HEIGHT, "POOL_TABLE", NULL);//magic numbers
    body_set_centroid(pool_table, (vector_t) {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2});
    scene_add_body(scene, pool_table);
}

void add_slider(scene_t *scene){
    list_t *slider_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *slider_image = IMG_Load("images/slider_bg.png");
    body_t *slider = body_init_with_info(slider_list, INFINITY, (rgb_color_t) {0,0,0,1}, slider_image, SLIDER_WIDTH, SLIDER_HEIGHT, "SLIDER", NULL);//magic numbers
    body_set_centroid(slider, (vector_t) {SLIDER_X, HIGH_RIGHT_CORNER.y / 2});
    scene_add_body(scene, slider);

    list_t *button_list = rect_init(BUTTON_WIDTH, BUTTON_HEIGHT);
    SDL_Surface *button_image = IMG_Load("images/light_slider.png");
    body_t *button = body_init_with_info(button_list, INFINITY, (rgb_color_t) {0,0,0,1}, button_image, BUTTON_WIDTH, BUTTON_HEIGHT, "BUTTON", NULL);//magic numbers
    body_set_centroid(button, (vector_t) {SLIDER_X, BUTTON_Y});
    scene_add_body(scene, button);
}

void add_balls(scene_t *scene) {
    list_t *balls = list_init(NUM_BALLS, (free_func_t) body_free);
     //magic numbers!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    vector_t initial_position = {HIGH_RIGHT_CORNER.x * 3 / 7 - BALL_RADIUS * 9, HIGH_RIGHT_CORNER.y / 2 - BALL_RADIUS * 4};
    for(int i = 1; i <= 16; i++){
        char name[20];
        snprintf(name, 20, "images/ball_%d.png", i);
        SDL_Surface *ball_image = IMG_Load(name);
        body_t *pool_ball;
        if (i < 8) {
            pool_ball = create_ball(scene, "SOLID_BALL", ball_image);
        }
        else if (i == 8) {
            pool_ball = create_ball(scene, "8_BALL", ball_image);
        }
        else if (i == 16) {
            pool_ball = create_ball(scene, "CUE_BALL", ball_image);
        }
        else {
            pool_ball = create_ball(scene, "STRIPED_BALL", ball_image);
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
    body_t *wall_top1 = body_init_with_info(wall_shape_top1, WALL_MASS, (rgb_color_t){0, 1, 0, 1}, NULL, wall_width, WALL_THICKNESS, "WALL", NULL);
    vector_t wall_centroid_top1 = vec_add(body_get_centroid(wall_top1), (vector_t) {HIGH_RIGHT_CORNER.x / 2 - wall_width / 2 - WALL_GAP, HIGH_RIGHT_CORNER.y / 2 + TABLE_HEIGHT / 2 - TABLE_WALL_THICKNESS});
    body_set_centroid(wall_top1, wall_centroid_top1);
    scene_add_body(scene, wall_top1);

    list_t *wall_shape_top2 = rect_init(wall_width, WALL_THICKNESS);
    body_t *wall_top2 = body_init_with_info(wall_shape_top2, WALL_MASS, (rgb_color_t){0, 1, 0, 1}, NULL, wall_width, WALL_THICKNESS, "WALL", NULL);
    vector_t wall_centroid_top2 = vec_add(body_get_centroid(wall_top2), (vector_t) {HIGH_RIGHT_CORNER.x / 2 + wall_width / 2 + WALL_GAP, HIGH_RIGHT_CORNER.y / 2 + TABLE_HEIGHT / 2 - TABLE_WALL_THICKNESS});
    body_set_centroid(wall_top2, wall_centroid_top2);
    scene_add_body(scene, wall_top2);

    //bottom wall
    list_t *wall_shape_bot1 = rect_init(wall_width, WALL_THICKNESS);
    body_t *wall_bot1 = body_init_with_info(wall_shape_bot1, WALL_MASS, (rgb_color_t){0, 1, 0, 1}, NULL, wall_width, WALL_THICKNESS, "WALL", NULL);
    vector_t wall_centroid_bot1 = vec_add(body_get_centroid(wall_bot1), (vector_t) {HIGH_RIGHT_CORNER.x / 2 - wall_width / 2 - WALL_GAP, HIGH_RIGHT_CORNER.y / 2 - TABLE_HEIGHT / 2 + TABLE_WALL_THICKNESS});
    body_set_centroid(wall_bot1, wall_centroid_bot1);
    scene_add_body(scene, wall_bot1);

    list_t *wall_shape_bot2 = rect_init(wall_width, WALL_THICKNESS);
    body_t *wall_bot2 = body_init_with_info(wall_shape_bot2, WALL_MASS, (rgb_color_t){0, 1, 0, 1}, NULL, wall_width, WALL_THICKNESS, "WALL", NULL);
    vector_t wall_centroid_bot2 = vec_add(body_get_centroid(wall_bot2), (vector_t) {HIGH_RIGHT_CORNER.x / 2 + wall_width / 2 + WALL_GAP, HIGH_RIGHT_CORNER.y / 2 - TABLE_HEIGHT / 2 + TABLE_WALL_THICKNESS});
    body_set_centroid(wall_bot2, wall_centroid_bot2);
    scene_add_body(scene, wall_bot2);

    //right wall
    double wall_height = TABLE_HEIGHT - WALL_TABLE_WIDTH_DIFF - BREEZY_CONSTANT;
    list_t *wall_shape_right = rect_init(WALL_THICKNESS, wall_height);
    body_t *wall_right = body_init_with_info(wall_shape_right, WALL_MASS, (rgb_color_t){0, 1, 0, 1}, NULL, WALL_THICKNESS, wall_height, "WALL", NULL);
    vector_t wall_centroid_right = vec_add(body_get_centroid(wall_right), (vector_t) {HIGH_RIGHT_CORNER.x / 2 + TABLE_WIDTH / 2 - TABLE_WALL_THICKNESS, HIGH_RIGHT_CORNER.y / 2});
    body_set_centroid(wall_right, wall_centroid_right);
    scene_add_body(scene, wall_right);
    //left wall
    list_t *wall_shape_left = rect_init(WALL_THICKNESS, wall_height);
    body_t *wall_left = body_init_with_info(wall_shape_left, WALL_MASS, (rgb_color_t){0, 1, 0, 1}, NULL, WALL_THICKNESS, wall_height, "WALL", NULL);
    vector_t wall_centroid_left = vec_add(body_get_centroid(wall_left), (vector_t) {HIGH_RIGHT_CORNER.x / 2 - TABLE_WIDTH / 2 + TABLE_WALL_THICKNESS, HIGH_RIGHT_CORNER.y / 2});
    body_set_centroid(wall_left, wall_centroid_left);
    scene_add_body(scene, wall_left);
}

void add_holes(scene_t *scene){
    list_t *hole_shape1 = circle_init(HOLE_RADIUS);
    body_t *hole_top1 = body_init_with_info(hole_shape1, INFINITY, (rgb_color_t){1, 0, 0, 1}, NULL, HOLE_RADIUS * 2, HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top1 = (vector_t) {HIGH_RIGHT_CORNER.x / 2 - TABLE_WIDTH / 2 + HOLE_CONSTANT, HIGH_RIGHT_CORNER.y / 2 + TABLE_HEIGHT / 2 - HOLE_CONSTANT};
    body_set_centroid(hole_top1, hole_centroid_top1);
    scene_add_body(scene, hole_top1);

    list_t *hole_shape2 = circle_init(HOLE_RADIUS);
    body_t *hole_top2 = body_init_with_info(hole_shape2, INFINITY, (rgb_color_t){1, 0, 0, 1}, NULL, HOLE_RADIUS * 2, HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top2 = (vector_t) {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2 + TABLE_HEIGHT / 2 - HOLE_CONSTANT + CORRECTION_CONSTANT};
    body_set_centroid(hole_top2, hole_centroid_top2);
    scene_add_body(scene, hole_top2);

    list_t *hole_shape3 = circle_init(HOLE_RADIUS);
    body_t *hole_top3 = body_init_with_info(hole_shape3, INFINITY, (rgb_color_t){1, 0, 0, 1}, NULL, HOLE_RADIUS * 2, HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top3 = (vector_t) {HIGH_RIGHT_CORNER.x / 2 + TABLE_WIDTH / 2 - HOLE_CONSTANT, HIGH_RIGHT_CORNER.y / 2 + TABLE_HEIGHT / 2 - HOLE_CONSTANT};
    body_set_centroid(hole_top3, hole_centroid_top3);
    scene_add_body(scene, hole_top3);

    list_t *hole_shape4 = circle_init(HOLE_RADIUS);
    body_t *hole_top4 = body_init_with_info(hole_shape4, INFINITY, (rgb_color_t){1, 0, 0, 1}, NULL, HOLE_RADIUS * 2, HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top4 = (vector_t) {HIGH_RIGHT_CORNER.x / 2 - TABLE_WIDTH / 2 + HOLE_CONSTANT, HIGH_RIGHT_CORNER.y / 2 - TABLE_HEIGHT / 2 + HOLE_CONSTANT};
    body_set_centroid(hole_top4, hole_centroid_top4);
    scene_add_body(scene, hole_top4);

    list_t *hole_shape5 = circle_init(HOLE_RADIUS);
    body_t *hole_top5 = body_init_with_info(hole_shape5, INFINITY, (rgb_color_t){1, 0, 0, 1}, NULL, HOLE_RADIUS * 2, HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top5 = (vector_t) {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2 - TABLE_HEIGHT / 2 + HOLE_CONSTANT - CORRECTION_CONSTANT};
    body_set_centroid(hole_top5, hole_centroid_top5);
    scene_add_body(scene, hole_top5);

    list_t *hole_shape6 = circle_init(HOLE_RADIUS);
    body_t *hole_top6 = body_init_with_info(hole_shape6, INFINITY, (rgb_color_t){1, 0, 0, 1}, NULL, HOLE_RADIUS * 2, HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top6 = (vector_t) {HIGH_RIGHT_CORNER.x / 2 + TABLE_WIDTH / 2 - HOLE_CONSTANT, HIGH_RIGHT_CORNER.y / 2 - TABLE_HEIGHT / 2 + HOLE_CONSTANT};
    body_set_centroid(hole_top6, hole_centroid_top6);
    scene_add_body(scene, hole_top6);
}

void add_initial_line(scene_t *scene){
    double wall_height = TABLE_HEIGHT - 2 * WALL_TABLE_WIDTH_DIFF;
    list_t *line_list = rect_init(TINY_CONSTANT, 2 * wall_height - 4 * WALL_THICKNESS);
    body_t *line = body_init_with_info(line_list, INFINITY, WHITE_COLOR, NULL, 0, 0, "INITIAL_LINE", NULL);
    vector_t line_centroid = (vector_t) {HIGH_RIGHT_CORNER.x * 4 / 7, HIGH_RIGHT_CORNER.y / 2};
    body_set_centroid(line, line_centroid);
    scene_add_body(scene, line);
}

void add_text(scene_t *scene, TTF_Font *font){
    list_t *shape = list_init(0, free);
    SDL_Surface *turn = TTF_RenderText_Solid(font, "Player 1", WHITE_COLOR_SDL);
    body_t *turn_text = body_init_with_info(shape, INFINITY, (rgb_color_t) {1, 0, 0, 1}, turn, 300, 100, "TURN_TEXT", NULL);
    vector_t turn_text_centroid = {HIGH_RIGHT_CORNER.x / 2, 130};
    body_set_centroid(turn_text, turn_text_centroid);
    scene_add_body(scene, turn_text);

    body_t *ball = body_init_with_info(circle_init(BALL_RADIUS), BALL_MASS, WHITE_COLOR, NULL,
                        5*BALL_RADIUS, 5*BALL_RADIUS, "TYPE_INDICATOR", NULL);

    body_set_centroid(ball, (vector_t) {HIGH_RIGHT_CORNER.x / 2 + 300 / 2 + 4 * BALL_RADIUS, 125}); //MAGIC NUMBERS!!!!!!
    scene_add_body(scene, ball);

    // list_t *shape1 = list_init(0, free);
    // SDL_Surface *type = TTF_RenderText_Solid(font, "", WHITE_COLOR_SDL);
    // body_t *type_text = body_init_with_info(shape1, INFINITY, (rgb_color_t) {1, 0, 0, 1}, type, 700, 100, "TYPE_TEXT", NULL);
    // vector_t type_text_centroid = {LOW_LEFT_CORNER.x + 400, 150};
    // body_set_centroid(type_text, type_text_centroid);
    // scene_add_body(scene, type_text);

    list_t *shape2 = list_init(0, free);
    SDL_Surface *win = TTF_RenderText_Solid(font, "", WHITE_COLOR_SDL);
    body_t *win_text = body_init_with_info(shape2, INFINITY, (rgb_color_t) {1, 0, 0, 1}, win, 700, 100, "WIN_TEXT", NULL);
    vector_t win_text_centroid = {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2};
    body_set_centroid(win_text, win_text_centroid);
    scene_add_body(scene, win_text);

    list_t *shape3 = list_init(0, free);
    SDL_Surface *power = TTF_RenderText_Solid(font, "", WHITE_COLOR_SDL);
    body_t *power_text = body_init_with_info(shape3, INFINITY, (rgb_color_t) {1, 0, 0, 1}, power, 1100, 100, "POWER_TEXT", NULL);
    vector_t power_text_centroid = {HIGH_RIGHT_CORNER.x / 2, POWER_TEXT_Y};
    body_set_centroid(power_text, power_text_centroid);
    scene_add_body(scene, power_text);
}

void add_start_menu(scene_t *scene, TTF_Font *font) {
    // background
    list_t *bg_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *bg_image = IMG_Load("images/carpet-background.png");
    body_t *bg = body_init_with_info(bg_list, INFINITY, (rgb_color_t) {0,0,0,1}, bg_image, HIGH_RIGHT_CORNER.x, HIGH_RIGHT_CORNER.y, "START_MENU", NULL);//magic numbers
    body_set_centroid(bg, (vector_t) {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2});
    scene_add_body(scene, bg);

    // play button
    list_t *start_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *start_image = IMG_Load("images/play-button.png");
    body_t *start_button = body_init_with_info(start_list, INFINITY, (rgb_color_t) {0,0,0,1}, start_image, RECTANGULAR_BUTTON_WIDTH, RECTANGULAR_BUTTON_HEIGHT, "PLAY_BUTTON", NULL);
    body_set_centroid(start_button, START_PLAY_BUTTON_CENTROID);
    scene_add_body(scene, start_button);

    // instructions button
    list_t *info_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *info_image = IMG_Load("images/help-button.png");
    body_t *info_button = body_init_with_info(info_list, INFINITY, (rgb_color_t) {0,0,0,1}, info_image, RECTANGULAR_BUTTON_WIDTH, RECTANGULAR_BUTTON_HEIGHT, "INFO_BUTTON", NULL);
    body_set_centroid(info_button, START_INSTRUCTIONS_BUTTON_CENTROID);
    scene_add_body(scene, info_button);

    // quit button
    list_t *quit_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *quit_image = IMG_Load("images/quit-button.png");
    body_t *quit_button = body_init_with_info(quit_list, INFINITY, (rgb_color_t) {0,0,0,1}, quit_image, RECTANGULAR_BUTTON_WIDTH, RECTANGULAR_BUTTON_HEIGHT, "START_QUIT_BUTTON", NULL);
    body_set_centroid(quit_button, START_QUIT_BUTTON_CENTROID);
    scene_add_body(scene, quit_button);

    // title text
    list_t *shape = list_init(0, free);
    SDL_Surface *title = TTF_RenderText_Solid(font, "CRAZY 8 BALL", WHITE_COLOR_SDL);
    body_t *title_text = body_init_with_info(shape, INFINITY, (rgb_color_t) {1, 0, 0, 1}, title, 1000, 200, "TITLE_TEXT", NULL);
    vector_t title_text_centroid = {HIGH_RIGHT_CORNER.x / 2, TITLE_TEXT_Y};
    body_set_centroid(title_text, title_text_centroid);
    scene_add_body(scene, title_text);
}

void add_in_game_buttons(scene_t *scene){
    // help button
    list_t *instr_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *instr_image = IMG_Load("images/question-button.png");
    body_t *instr = body_init_with_info(instr_list, INFINITY, (rgb_color_t) {0,0,0,1}, instr_image, QUIT_BUTTON_SIDE_LENGTH, QUIT_BUTTON_SIDE_LENGTH, "HELP_BUTTON", NULL);//magic numbers
    body_set_centroid(instr, HELP_BUTTON_CENTROID);
    scene_add_body(scene, instr);

    // quit button
    list_t *quit_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *quit_image = IMG_Load("images/x-button.png");
    body_t *quit_button = body_init_with_info(quit_list, INFINITY, (rgb_color_t) {0,0,0,1}, quit_image, QUIT_BUTTON_SIDE_LENGTH, QUIT_BUTTON_SIDE_LENGTH, "QUIT_BUTTON", NULL);
    body_set_centroid(quit_button, QUIT_BUTTON_CENTROID);
    scene_add_body(scene, quit_button);
}

void add_background(scene_t *scene) {
    list_t *bg_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *bg_image = IMG_Load("images/carpet-background.png");
    body_t *bg = body_init_with_info(bg_list, INFINITY, (rgb_color_t) {0,0,0,1}, bg_image, HIGH_RIGHT_CORNER.x, HIGH_RIGHT_CORNER.y, "BACKGROUND", NULL);//magic numbers
    body_set_centroid(bg, (vector_t) {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2});
    scene_add_body(scene, bg);
}

void sound_setup() {
    Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096);
    Mix_AllocateChannels(100); //magic numbers!!!!!!!!!!!!!!!!!!!!!!!!!!
    Mix_Chunk *background = Mix_LoadWAV("sounds/TakeFive.wav");
    Mix_PlayChannel(3, background, -1);
    Mix_Volume(3, 10);
    for (int chan = 4; chan < 100; chan++) {
        Mix_Volume(chan, 7);
    }
}

void game_setup(scene_t *scene, TTF_Font *font){
    add_background(scene);
    add_table(scene);
    add_initial_line(scene);
    add_balls(scene);
    add_stick(scene);
    add_walls(scene);
    add_holes(scene);
    add_slider(scene);
    add_text(scene, font);
    add_in_game_buttons(scene);
    add_start_menu(scene, font);
    sound_setup();
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

    SDL_Surface *icon = IMG_Load("images/ball_8.png");
    sdl_set_icon(icon);

    while (!sdl_is_done()){
        if(game_state_get_game_quit(scene_get_game_state(scene))){
            break;
        }
        sdl_render_scene(scene);
        scene_tick(scene, time_since_last_tick());
        stop_balls(scene);
        if (game_state_get_end_of_turn(scene_get_game_state(scene)) && is_balls_stopped(scene) && game_state_get_winner(scene_get_game_state(scene)) == NULL){
            vector_t cue_centroid = vec_add(body_get_centroid(get_cue_ball(scene)), (vector_t) {BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2, 0});
            body_set_rotation(get_cue_stick(scene), 0);
            body_set_centroid(get_cue_stick(scene), cue_centroid);
            body_set_origin(get_cue_stick(scene), body_get_centroid(get_cue_ball(scene)));
            gameplay_handler(scene, font);
        }
        if (game_state_get_winner(scene_get_game_state(scene)) != NULL) {
            // printf("winner: %s: \n", game_state_get_winner(scene_get_game_state(scene)));
        }
    }
    scene_free(scene);
}
