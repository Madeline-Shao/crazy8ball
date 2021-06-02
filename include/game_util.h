#include "vector.h"
#include "color.h"
#include "sdl_wrapper.h"
#include "list.h"
#include "body.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

extern const vector_t LOW_LEFT_CORNER;
extern const vector_t HIGH_RIGHT_CORNER;
extern const double CIRCLE_POINTS;
extern const double BALL_RADIUS;
extern const double CUE_STICK_HEIGHT;
extern const double CUE_STICK_WIDTH;
extern const double CUE_STICK_DEFAULT_X;
extern const double CUE_BALL_DEFAULT_X_FACTOR;
extern const double BALL_MASS;
extern const double NUM_BALLS;
extern const double CUE_STICK_WIDTH;
extern const rgb_color_t WHITE_COLOR;
extern const SDL_Color WHITE_COLOR_SDL;
extern const SDL_Color BLACK_COLOR_SDL;
extern const SDL_Color GOLD_COLOR_SDL;
extern const double TABLE_WIDTH;;
extern const double TABLE_HEIGHT;
extern const double WALL_THICKNESS;
extern const double WALL_TABLE_WIDTH_DIFF;
extern const double TABLE_WALL_THICKNESS;
extern const double WALL_GAP;
extern const double HOLE_RADIUS;
extern const double HOLE_CONSTANT;
extern const double CORRECTION_CONSTANT;
extern const double BREEZY_CONSTANT;
extern const double BALL_ELASTICITY;
extern const double MU;
extern const double G;
extern const double DRAG_DIST;
extern const double SLIDER_WIDTH;
extern const double SLIDER_HEIGHT;
extern const double SLIDER_X;
extern const double BUTTON_WIDTH;
extern const double BUTTON_HEIGHT;
extern const double BUTTON_Y;
extern const double DEFAULT_IMPULSE;
extern const vector_t VELOCITY_THRESHOLD;
extern const double TINY_CONSTANT;
extern const double PULL_FACTOR_ADJUSTMENT_CONSTANT;
extern const double SIZE_POWERDOWN_ADJUSTMENT_SCALE_FACTOR;
extern const double POWER_TEXT_Y;
extern const double POWER_PROBABILITY_SPACING;
extern const int WINNER_TEXT_LENGTH;
extern const int WIN_MESSAGE_LENGTH;
extern const int KONAMI_CODE_LENGTH;
extern const int FONT_SIZE;
extern const double START_MENU_BUTTON_SIDE_LENGTH;
extern const double RECTANGULAR_BUTTON_WIDTH;
extern const double RECTANGULAR_BUTTON_HEIGHT;
extern const double TITLE_TEXT_Y;
extern const double PLAY_BUTTON_X;
extern const double INFO_BUTTON_X;
extern const double QUIT_BUTTON_Y;
extern const vector_t QUIT_BUTTON_CENTROID;
extern const vector_t HELP_BUTTON_CENTROID;
extern const double QUIT_BUTTON_SIDE_LENGTH;
extern const vector_t START_PLAY_BUTTON_CENTROID;
extern const vector_t START_INSTRUCTIONS_BUTTON_CENTROID;
extern const vector_t START_QUIT_BUTTON_CENTROID;
extern const double BACKLOG_FORCE_CONSTANT_TO_SATISFY_PATRICKS_DESIRES;
extern const int CUE_STICK_BALL_CHANNEL;
extern const int POCKET_CHANNEL;
extern const int BACKGROUND_CHANNEL;
extern const int COLLISION_CHANNEL_START;
extern const rgb_color_t GRAY_TRANSPARENT_COLOR;
extern const int STRIPED_TEXT_LENGTH;
extern const int SOLID_TEXT_LENGTH;
extern const int IMAGE_PATH_LENGTH;
extern const int EIGHT_BALL_NUM;
extern const int CUE_BALL_NUM;
extern const double INITIAL_TRIANGLE_POSITION_FACTOR;
extern const int TRIANGLE_COL_NUM;
extern const int TURN_TEXT_WIDTH;
extern const int TEXT_HEIGHT;
extern const int TURN_TEXT_Y;
extern const int TYPE_IMAGE_FACTOR;
extern const int TYPE_IMAGE_Y;
extern const int WIN_TEXT_WIDTH;
extern const int POWER_TEXT_WIDTH;
extern const int TITLE_TEXT_WIDTH;
extern const int TITLE_TEXT_HEIGHT;
extern const int FREQUENCY;
extern const int STEREO;
extern const int CHUNK_SIZE;
extern const int NUM_CHANNELS;
extern const int BACKGROUND_VOLUME;
extern const int COLLISION_VOLUME;
extern const int TRIANGLE_WIDTH_FACTOR;
extern const int TRIANGLE_HEIGHT_FACTOR;
extern const int WALL_DIFF_FACTOR;
extern const int TEXT_TYPE_INDICATOR_FACTOR;

bool overlaps(double x, double y, vector_t centroid);

list_t *rect_init(double width, double height);

list_t *circle_init(double radius);

body_t *get_object(scene_t *scene, char *name);

body_t *create_ball(scene_t *scene, char *info, SDL_Surface *img);

void change_text(scene_t *scene, char *info, char *text, TTF_Font *font, SDL_Color color);

void play_sound(int channel, char *sound_file);
