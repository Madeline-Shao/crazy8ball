#include "game_util.h"

const vector_t LOW_LEFT_CORNER = {0, 0};
const vector_t HIGH_RIGHT_CORNER = {1500, 900};
const double CIRCLE_POINTS = 30;
const double BALL_RADIUS = 12;
const double BALL_MASS = 10;
const double NUM_BALLS  = 16;
const double CUE_STICK_WIDTH = 400;
const double CUE_STICK_HEIGHT = 10;
const double CUE_STICK_DEFAULT_X = 1200;
const double CUE_BALL_DEFAULT_X_FACTOR = 4.0 / 7;
const rgb_color_t WHITE_COLOR = {1, 1, 1, 1};
const SDL_Color WHITE_COLOR_SDL = {255, 255, 255};
const SDL_Color BLACK_COLOR_SDL = {0, 0, 0};
const SDL_Color GOLD_COLOR_SDL = {241, 194, 50};
const double TABLE_WIDTH = 800;
const double TABLE_HEIGHT = 480;
const double WALL_THICKNESS = 10;
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
const vector_t VELOCITY_THRESHOLD = {0.5, 0.5};
const double TINY_CONSTANT = 0.8;
const double PULL_FACTOR_ADJUSTMENT_CONSTANT = 47;
const double SIZE_POWERDOWN_ADJUSTMENT_SCALE_FACTOR = 1.265;
const double POWER_TEXT_Y = 800;
const double POWER_PROBABILITY_SPACING = 0.05;
const int WINNER_TEXT_LENGTH = 9;
const int WIN_MESSAGE_LENGTH = 17;
const int KONAMI_CODE_LENGTH = 10;
const int FONT_SIZE = 100;
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
const int CUE_STICK_BALL_CHANNEL = 1;
const int POCKET_CHANNEL = 2;
const int BACKGROUND_CHANNEL = 3;
const int COLLISION_CHANNEL_START = 4;
const rgb_color_t GRAY_TRANSPARENT_COLOR = {148 / 255, 148 / 255, 148 / 255, 0.5};
const int STRIPED_TEXT_LENGTH = 13;
const int SOLID_TEXT_LENGTH = 11;
const int IMAGE_PATH_LENGTH = 20;
const int EIGHT_BALL_NUM = 8;
const int CUE_BALL_NUM = 16;
const double INITIAL_TRIANGLE_POSITION_FACTOR = 3.0 / 7;
const int TRIANGLE_COL_NUM = 5;
const int TURN_TEXT_WIDTH = 300;
const int TEXT_HEIGHT = 100;
const int TURN_TEXT_Y = 130;
const int TYPE_IMAGE_FACTOR = 5;
const int TYPE_IMAGE_Y = 125;
const int WIN_TEXT_WIDTH = 700;
const int POWER_TEXT_WIDTH = 1100;
const int TITLE_TEXT_WIDTH = 1000;
const int TITLE_TEXT_HEIGHT = 200;
const int FREQUENCY = 22050;
const int STEREO = 2;
const int CHUNK_SIZE = 4096;
const int NUM_CHANNELS = 100;
const int BACKGROUND_VOLUME = 5;
const int COLLISION_VOLUME = 7;
const int TRIANGLE_WIDTH_FACTOR = 9;
const int TRIANGLE_HEIGHT_FACTOR = 4;
const int WALL_DIFF_FACTOR = 4;
const int TEXT_TYPE_INDICATOR_FACTOR = 4;

bool overlaps(double x, double y, vector_t centroid){
    if (x < centroid.x + BALL_RADIUS * 2 && x > centroid.x - BALL_RADIUS * 2 && y < centroid.y + BALL_RADIUS * 2
                    && y > centroid.y - BALL_RADIUS * 2){
                        return true;
                    }
    return false;
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

body_t *get_object(scene_t *scene, char *name){
    for (int i = 0; i < scene_bodies(scene); i++) {
        body_t *body = scene_get_body(scene, i);
        if (strcmp(body_get_info(body), name) == 0) {
            return body;
        }
    }
    return NULL;
}

body_t *create_ball(char *info, SDL_Surface *img){
    body_t *ball = body_init_with_info(circle_init(BALL_RADIUS), BALL_MASS, WHITE_COLOR, img,
                                        2*BALL_RADIUS, 2*BALL_RADIUS, info, NULL);
    return ball;
}

void change_text(scene_t *scene, char *info, char *text, TTF_Font *font, SDL_Color color){
    SDL_Surface *new_text = TTF_RenderText_Solid(font, text, color);
    body_set_image(get_object(scene, info), new_text);
}

void play_sound(int channel, char *sound_file){
    Mix_Chunk *sound = Mix_LoadWAV(sound_file);
    Mix_PlayChannel(channel, sound, 0);
}
