#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "body.h"
#include "color.h"
#include "list.h"
#include "scene.h"
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
const double CUE_STICK_MASS = 30;
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
const double BALL_ELASTICITY = 1.0;
const double MU = 5;
const double G = 10;

// // stick force buildup, animation, and ball collision
// body_t *shoot_stick(vector_t initial_position, int direction, double width,

// }

// // stick rotation
// void player_key_handler(char key, key_event_type_t type, double held_time)

// }

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

body_t *get_cue_ball(scene_t *scene) {
    for (int i = 0; i < scene_bodies(scene); i++) {
        body_t *body = scene_get_body(scene, i);
        if (strcmp(body_get_info(body),"CUE_BALL") == 0) {
            return body;
        }
    }
    return NULL;
}

void add_stick(scene_t * scene) {
    list_t *stick_shape = rect_init(CUE_STICK_WIDTH, CUE_STICK_HEIGHT);
    SDL_Surface *ball_image = IMG_Load("images/cue_stick.png");
    body_t *cue_stick = body_init_with_info(stick_shape, CUE_STICK_MASS, WHITE_COLOR, ball_image, CUE_STICK_WIDTH, CUE_STICK_HEIGHT, "CUE_STICK", NULL);
    vector_t cue_centroid = vec_add(body_get_centroid(get_cue_ball(scene)), (vector_t) {BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2, 0});
    body_set_centroid(cue_stick, cue_centroid);
    scene_add_body(scene, cue_stick);
}

void add_table(scene_t *scene) {
    list_t *rect = rect_init(1000, 600);
    SDL_Surface *table_image = IMG_Load("images/pool_table.png");
    body_t *pool_table = body_init_with_info(rect, TABLE_MASS, (rgb_color_t) {0,0,0}, table_image, TABLE_WIDTH, TABLE_HEIGHT, "POOL_TABLE", NULL);//magic numbers
    body_set_centroid(pool_table, (vector_t) {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2});
    scene_add_body(scene, pool_table);
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
            body_add_impulse(pool_ball, (vector_t) {-2000, 0});
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

//that's what she said!
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
}

void hole_destroy(body_t *ball, body_t *hole, vector_t axis, void *aux) {
    body_remove(ball);
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
                    create_collision(scene, body1, body2, (collision_handler_t) hole_destroy, NULL, NULL);
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
    game_setup(scene);
    SDL_Renderer *renderer = sdl_init(LOW_LEFT_CORNER, HIGH_RIGHT_CORNER);
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 0);
    add_forces(scene);

    while (!sdl_is_done()){
        sdl_render_scene(scene);
        scene_tick(scene, time_since_last_tick());
    }
    scene_free(scene);
}
