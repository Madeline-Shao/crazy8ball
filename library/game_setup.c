#include "game_setup.h"

void game_setup_add_stick(scene_t * scene) {
    list_t *stick_shape = rect_init(CUE_STICK_WIDTH, CUE_STICK_HEIGHT);
    SDL_Surface *ball_image = IMG_Load("images/cue_stick.png");
    body_t *cue_stick = body_init_with_info(stick_shape, INFINITY, WHITE_COLOR, ball_image,
                                            CUE_STICK_WIDTH, CUE_STICK_HEIGHT, "CUE_STICK", NULL);
    vector_t cue_centroid = vec_add(body_get_centroid(get_object(scene, "CUE_BALL")),
                                    (vector_t) {BALL_RADIUS * 2 + CUE_STICK_WIDTH / 2, 0});
    body_set_centroid(cue_stick, cue_centroid);
    body_set_origin(cue_stick, body_get_centroid(get_object(scene, "CUE_BALL")));
    scene_add_body(scene, cue_stick);
}

void game_setup_add_table(scene_t *scene) {
    list_t *rect = rect_init(TABLE_WIDTH, TABLE_HEIGHT);
    SDL_Surface *table_image = IMG_Load("images/pool_table.png");
    body_t *pool_table = body_init_with_info(rect, INFINITY, WHITE_COLOR, table_image, TABLE_WIDTH,
                                             TABLE_HEIGHT, "POOL_TABLE", NULL);
    body_set_centroid(pool_table, (vector_t) {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2});
    scene_add_body(scene, pool_table);
}

void game_setup_add_slider(scene_t *scene){
    list_t *slider_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *slider_image = IMG_Load("images/slider_bg.png");
    body_t *slider = body_init_with_info(slider_list, INFINITY, WHITE_COLOR, slider_image,
                                         SLIDER_WIDTH, SLIDER_HEIGHT, "SLIDER", NULL);
    body_set_centroid(slider, (vector_t) {SLIDER_X, HIGH_RIGHT_CORNER.y / 2});
    scene_add_body(scene, slider);

    list_t *button_list = rect_init(BUTTON_WIDTH, BUTTON_HEIGHT);
    SDL_Surface *button_image = IMG_Load("images/light_slider.png");
    body_t *button = body_init_with_info(button_list, INFINITY, WHITE_COLOR, button_image,
                                         BUTTON_WIDTH, BUTTON_HEIGHT, "BUTTON", NULL);
    body_set_centroid(button, (vector_t) {SLIDER_X, BUTTON_Y});
    scene_add_body(scene, button);
}

void game_setup_add_balls(scene_t *scene) {
    list_t *balls = list_init(NUM_BALLS, (free_func_t) body_free);
    vector_t initial_position = {HIGH_RIGHT_CORNER.x * INITIAL_TRIANGLE_POSITION_FACTOR -
                                 BALL_RADIUS * TRIANGLE_WIDTH_FACTOR, HIGH_RIGHT_CORNER.y / 2 -
                                 BALL_RADIUS * TRIANGLE_HEIGHT_FACTOR};
    for(int i = 1; i <= CUE_BALL_NUM; i++){
        char name[20];
        snprintf(name, IMAGE_PATH_LENGTH, "images/ball_%d.png", i);
        SDL_Surface *ball_image = IMG_Load(name);
        body_t *pool_ball;
        if (i < EIGHT_BALL_NUM) {
            pool_ball = create_ball("SOLID_BALL", ball_image);
        }
        else if (i == EIGHT_BALL_NUM) {
            pool_ball = create_ball("8_BALL", ball_image);
        }
        else if (i == CUE_BALL_NUM) {
            pool_ball = create_ball("CUE_BALL", ball_image);
        }
        else {
            pool_ball = create_ball("STRIPED_BALL", ball_image);
        }
        if (i != CUE_BALL_NUM) {
            list_add(balls, pool_ball);
        }
        else {
            body_set_centroid(pool_ball, (vector_t){HIGH_RIGHT_CORNER.x * CUE_BALL_DEFAULT_X_FACTOR,
                                                    HIGH_RIGHT_CORNER.y / 2});
        }
        scene_add_body(scene, pool_ball);
    }

    int y = 0;
    int x = 0;
    int top_column_y = initial_position.y;
    for (int i = 0; i < TRIANGLE_COL_NUM; i++) {
        for (int j = TRIANGLE_COL_NUM - 1; j >= i; j--) {
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

void game_setup_add_walls(scene_t *scene) {
    // top wall
    double wall_width = (TABLE_WIDTH - WALL_TABLE_WIDTH_DIFF) / 2 - WALL_GAP - BREEZY_CONSTANT;

    list_t *wall_shape_top1 = rect_init(wall_width, WALL_THICKNESS);
    body_t *wall_top1 = body_init_with_info(wall_shape_top1, INFINITY, WHITE_COLOR, NULL,
                                            wall_width, WALL_THICKNESS, "WALL", NULL);
    vector_t wall_centroid_top1 = vec_add(body_get_centroid(wall_top1), (vector_t) {HIGH_RIGHT_CORNER.x / 2 - wall_width / 2 - WALL_GAP,
                                          HIGH_RIGHT_CORNER.y / 2 + TABLE_HEIGHT / 2 - TABLE_WALL_THICKNESS});
    body_set_centroid(wall_top1, wall_centroid_top1);
    scene_add_body(scene, wall_top1);

    list_t *wall_shape_top2 = rect_init(wall_width, WALL_THICKNESS);
    body_t *wall_top2 = body_init_with_info(wall_shape_top2, INFINITY, WHITE_COLOR, NULL,
                                            wall_width, WALL_THICKNESS, "WALL", NULL);
    vector_t wall_centroid_top2 = vec_add(body_get_centroid(wall_top2), (vector_t) {HIGH_RIGHT_CORNER.x / 2 + wall_width / 2 + WALL_GAP,
                                          HIGH_RIGHT_CORNER.y / 2 + TABLE_HEIGHT / 2 - TABLE_WALL_THICKNESS});
    body_set_centroid(wall_top2, wall_centroid_top2);
    scene_add_body(scene, wall_top2);

    //bottom wall
    list_t *wall_shape_bot1 = rect_init(wall_width, WALL_THICKNESS);
    body_t *wall_bot1 = body_init_with_info(wall_shape_bot1, INFINITY, WHITE_COLOR, NULL, wall_width,
                                            WALL_THICKNESS, "WALL", NULL);
    vector_t wall_centroid_bot1 = vec_add(body_get_centroid(wall_bot1), (vector_t) {HIGH_RIGHT_CORNER.x / 2 - wall_width / 2 - WALL_GAP,
                                          HIGH_RIGHT_CORNER.y / 2 - TABLE_HEIGHT / 2 + TABLE_WALL_THICKNESS});
    body_set_centroid(wall_bot1, wall_centroid_bot1);
    scene_add_body(scene, wall_bot1);

    list_t *wall_shape_bot2 = rect_init(wall_width, WALL_THICKNESS);
    body_t *wall_bot2 = body_init_with_info(wall_shape_bot2, INFINITY, WHITE_COLOR, NULL, wall_width,
                                            WALL_THICKNESS, "WALL", NULL);
    vector_t wall_centroid_bot2 = vec_add(body_get_centroid(wall_bot2), (vector_t) {HIGH_RIGHT_CORNER.x / 2 + wall_width / 2 + WALL_GAP,
                                          HIGH_RIGHT_CORNER.y / 2 - TABLE_HEIGHT / 2 + TABLE_WALL_THICKNESS});
    body_set_centroid(wall_bot2, wall_centroid_bot2);
    scene_add_body(scene, wall_bot2);

    //right wall
    double wall_height = TABLE_HEIGHT - WALL_TABLE_WIDTH_DIFF - BREEZY_CONSTANT;
    list_t *wall_shape_right = rect_init(WALL_THICKNESS, wall_height);
    body_t *wall_right = body_init_with_info(wall_shape_right, INFINITY, WHITE_COLOR, NULL,
                                             WALL_THICKNESS, wall_height, "WALL", NULL);
    vector_t wall_centroid_right = vec_add(body_get_centroid(wall_right), (vector_t) {HIGH_RIGHT_CORNER.x / 2 + TABLE_WIDTH / 2 - TABLE_WALL_THICKNESS,
                                           HIGH_RIGHT_CORNER.y / 2});
    body_set_centroid(wall_right, wall_centroid_right);
    scene_add_body(scene, wall_right);
    //left wall
    list_t *wall_shape_left = rect_init(WALL_THICKNESS, wall_height);
    body_t *wall_left = body_init_with_info(wall_shape_left, INFINITY, WHITE_COLOR, NULL, WALL_THICKNESS,
                                            wall_height, "WALL", NULL);
    vector_t wall_centroid_left = vec_add(body_get_centroid(wall_left), (vector_t) {HIGH_RIGHT_CORNER.x / 2 - TABLE_WIDTH / 2 + TABLE_WALL_THICKNESS,
                                          HIGH_RIGHT_CORNER.y / 2});
    body_set_centroid(wall_left, wall_centroid_left);
    scene_add_body(scene, wall_left);
}

void game_setup_add_holes(scene_t *scene){
    list_t *hole_shape1 = circle_init(HOLE_RADIUS);
    body_t *hole_top1 = body_init_with_info(hole_shape1, INFINITY, WHITE_COLOR, NULL, HOLE_RADIUS * 2,
                                            HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top1 = (vector_t) {HIGH_RIGHT_CORNER.x / 2 - TABLE_WIDTH / 2 + HOLE_CONSTANT,
                                  HIGH_RIGHT_CORNER.y / 2 + TABLE_HEIGHT / 2 - HOLE_CONSTANT};
    body_set_centroid(hole_top1, hole_centroid_top1);
    scene_add_body(scene, hole_top1);

    list_t *hole_shape2 = circle_init(HOLE_RADIUS);
    body_t *hole_top2 = body_init_with_info(hole_shape2, INFINITY, WHITE_COLOR, NULL, HOLE_RADIUS * 2,
                                            HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top2 = (vector_t) {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2 +
                                  TABLE_HEIGHT / 2 - HOLE_CONSTANT + CORRECTION_CONSTANT};
    body_set_centroid(hole_top2, hole_centroid_top2);
    scene_add_body(scene, hole_top2);

    list_t *hole_shape3 = circle_init(HOLE_RADIUS);
    body_t *hole_top3 = body_init_with_info(hole_shape3, INFINITY, WHITE_COLOR, NULL, HOLE_RADIUS * 2,
                                            HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top3 = (vector_t) {HIGH_RIGHT_CORNER.x / 2 + TABLE_WIDTH / 2 - HOLE_CONSTANT,
                                  HIGH_RIGHT_CORNER.y / 2 + TABLE_HEIGHT / 2 - HOLE_CONSTANT};
    body_set_centroid(hole_top3, hole_centroid_top3);
    scene_add_body(scene, hole_top3);

    list_t *hole_shape4 = circle_init(HOLE_RADIUS);
    body_t *hole_top4 = body_init_with_info(hole_shape4, INFINITY, WHITE_COLOR, NULL, HOLE_RADIUS * 2,
                                            HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top4 = (vector_t) {HIGH_RIGHT_CORNER.x / 2 - TABLE_WIDTH / 2 + HOLE_CONSTANT,
                                  HIGH_RIGHT_CORNER.y / 2 - TABLE_HEIGHT / 2 + HOLE_CONSTANT};
    body_set_centroid(hole_top4, hole_centroid_top4);
    scene_add_body(scene, hole_top4);

    list_t *hole_shape5 = circle_init(HOLE_RADIUS);
    body_t *hole_top5 = body_init_with_info(hole_shape5, INFINITY, WHITE_COLOR, NULL, HOLE_RADIUS * 2,
                                            HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top5 = (vector_t) {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2 -
                                  TABLE_HEIGHT / 2 + HOLE_CONSTANT - CORRECTION_CONSTANT};
    body_set_centroid(hole_top5, hole_centroid_top5);
    scene_add_body(scene, hole_top5);

    list_t *hole_shape6 = circle_init(HOLE_RADIUS);
    body_t *hole_top6 = body_init_with_info(hole_shape6, INFINITY, WHITE_COLOR, NULL, HOLE_RADIUS * 2,
                                            HOLE_RADIUS * 2, "HOLE", NULL);
    vector_t hole_centroid_top6 = (vector_t) {HIGH_RIGHT_CORNER.x / 2 + TABLE_WIDTH / 2 - HOLE_CONSTANT,
                                  HIGH_RIGHT_CORNER.y / 2 - TABLE_HEIGHT / 2 + HOLE_CONSTANT};
    body_set_centroid(hole_top6, hole_centroid_top6);
    scene_add_body(scene, hole_top6);
}

void game_setup_add_initial_line(scene_t *scene){
    double wall_height = TABLE_HEIGHT - 2 * WALL_TABLE_WIDTH_DIFF;
    list_t *line_list = rect_init(TINY_CONSTANT, 2 * wall_height - WALL_DIFF_FACTOR * WALL_THICKNESS);
    body_t *line = body_init_with_info(line_list, INFINITY, WHITE_COLOR, NULL, 0, 0, "INITIAL_LINE", NULL);
    vector_t line_centroid = (vector_t) {HIGH_RIGHT_CORNER.x * CUE_BALL_DEFAULT_X_FACTOR, HIGH_RIGHT_CORNER.y / 2};
    body_set_centroid(line, line_centroid);
    scene_add_body(scene, line);
}

void game_setup_add_text(scene_t *scene, TTF_Font *font){
    list_t *shape = list_init(0, free);
    SDL_Surface *turn = TTF_RenderText_Solid(font, "Player 1", WHITE_COLOR_SDL);
    body_t *turn_text = body_init_with_info(shape, INFINITY, WHITE_COLOR, turn, TURN_TEXT_WIDTH,
                                            TEXT_HEIGHT, "TURN_TEXT", NULL);
    vector_t turn_text_centroid = {HIGH_RIGHT_CORNER.x / 2, TURN_TEXT_Y};
    body_set_centroid(turn_text, turn_text_centroid);
    scene_add_body(scene, turn_text);

    body_t *ball = body_init_with_info(circle_init(BALL_RADIUS), BALL_MASS, WHITE_COLOR, NULL,
                        TYPE_IMAGE_FACTOR * BALL_RADIUS, TYPE_IMAGE_FACTOR * BALL_RADIUS, "TYPE_INDICATOR", NULL);

    body_set_centroid(ball, (vector_t) {HIGH_RIGHT_CORNER.x / 2 + TURN_TEXT_WIDTH / 2.0 +
                                        TEXT_TYPE_INDICATOR_FACTOR * BALL_RADIUS, TYPE_IMAGE_Y});
    scene_add_body(scene, ball);

    list_t *shape2 = list_init(0, free);
    SDL_Surface *win = TTF_RenderText_Solid(font, "", WHITE_COLOR_SDL);
    body_t *win_text = body_init_with_info(shape2, INFINITY, WHITE_COLOR, win, WIN_TEXT_WIDTH,
                                           TEXT_HEIGHT, "WIN_TEXT", NULL);
    vector_t win_text_centroid = {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2};
    body_set_centroid(win_text, win_text_centroid);
    scene_add_body(scene, win_text);

    list_t *shape3 = list_init(0, free);
    SDL_Surface *power = TTF_RenderText_Solid(font, "", WHITE_COLOR_SDL);
    body_t *power_text = body_init_with_info(shape3, INFINITY, WHITE_COLOR, power, POWER_TEXT_WIDTH,
                                             TEXT_HEIGHT, "POWER_TEXT", NULL);
    vector_t power_text_centroid = {HIGH_RIGHT_CORNER.x / 2, POWER_TEXT_Y};
    body_set_centroid(power_text, power_text_centroid);
    scene_add_body(scene, power_text);
}

void game_setup_add_start_menu(scene_t *scene, TTF_Font *font) {
    // background
    list_t *bg_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *bg_image = IMG_Load("images/carpet-background.png");
    body_t *bg = body_init_with_info(bg_list, INFINITY, WHITE_COLOR, bg_image, HIGH_RIGHT_CORNER.x,
                                     HIGH_RIGHT_CORNER.y, "START_MENU", NULL);
    body_set_centroid(bg, (vector_t) {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2});
    scene_add_body(scene, bg);

    // play button
    list_t *start_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *start_image = IMG_Load("images/play-button.png");
    body_t *start_button = body_init_with_info(start_list, INFINITY, WHITE_COLOR, start_image,
                                               RECTANGULAR_BUTTON_WIDTH, RECTANGULAR_BUTTON_HEIGHT,
                                               "PLAY_BUTTON", NULL);
    body_set_centroid(start_button, START_PLAY_BUTTON_CENTROID);
    scene_add_body(scene, start_button);

    // instructions button
    list_t *info_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *info_image = IMG_Load("images/help-button.png");
    body_t *info_button = body_init_with_info(info_list, INFINITY, WHITE_COLOR, info_image,
                                              RECTANGULAR_BUTTON_WIDTH, RECTANGULAR_BUTTON_HEIGHT,
                                              "INFO_BUTTON", NULL);
    body_set_centroid(info_button, START_INSTRUCTIONS_BUTTON_CENTROID);
    scene_add_body(scene, info_button);

    // quit button
    list_t *quit_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *quit_image = IMG_Load("images/quit-button.png");
    body_t *quit_button = body_init_with_info(quit_list, INFINITY, WHITE_COLOR, quit_image,
                                              RECTANGULAR_BUTTON_WIDTH, RECTANGULAR_BUTTON_HEIGHT,
                                              "START_QUIT_BUTTON", NULL);
    body_set_centroid(quit_button, START_QUIT_BUTTON_CENTROID);
    scene_add_body(scene, quit_button);

    // title text
    list_t *shape = list_init(0, free);
    SDL_Surface *title = TTF_RenderText_Solid(font, "CRAZY 8 BALL", WHITE_COLOR_SDL);
    body_t *title_text = body_init_with_info(shape, INFINITY, WHITE_COLOR, title, TITLE_TEXT_WIDTH,
                                             TITLE_TEXT_HEIGHT, "TITLE_TEXT", NULL);
    vector_t title_text_centroid = {HIGH_RIGHT_CORNER.x / 2, TITLE_TEXT_Y};
    body_set_centroid(title_text, title_text_centroid);
    scene_add_body(scene, title_text);
}

void game_setup_add_in_game_buttons(scene_t *scene){
    // help button
    list_t *instr_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *instr_image = IMG_Load("images/question-button.png");
    body_t *instr = body_init_with_info(instr_list, INFINITY, WHITE_COLOR, instr_image,
                                        QUIT_BUTTON_SIDE_LENGTH, QUIT_BUTTON_SIDE_LENGTH, "HELP_BUTTON", NULL);
    body_set_centroid(instr, HELP_BUTTON_CENTROID);
    scene_add_body(scene, instr);

    // quit button
    list_t *quit_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *quit_image = IMG_Load("images/x-button.png");
    body_t *quit_button = body_init_with_info(quit_list, INFINITY, WHITE_COLOR, quit_image,
                                              QUIT_BUTTON_SIDE_LENGTH, QUIT_BUTTON_SIDE_LENGTH, "QUIT_BUTTON", NULL);
    body_set_centroid(quit_button, QUIT_BUTTON_CENTROID);
    scene_add_body(scene, quit_button);
}

void game_setup_add_background(scene_t *scene) {
    list_t *bg_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *bg_image = IMG_Load("images/carpet-background.png");
    body_t *bg = body_init_with_info(bg_list, INFINITY, WHITE_COLOR, bg_image, HIGH_RIGHT_CORNER.x,
                                     HIGH_RIGHT_CORNER.y, "BACKGROUND", NULL);
    body_set_centroid(bg, (vector_t) {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2});
    scene_add_body(scene, bg);
}

void sound_setup() {
    Mix_OpenAudio(FREQUENCY, AUDIO_S16SYS, STEREO, CHUNK_SIZE);
    Mix_AllocateChannels(NUM_CHANNELS);
    Mix_Chunk *background = Mix_LoadWAV("sounds/TakeFive.wav");
    Mix_PlayChannel(BACKGROUND_CHANNEL, background, -1);
    Mix_Volume(BACKGROUND_CHANNEL, BACKGROUND_VOLUME);
    for (int chan = COLLISION_CHANNEL_START; chan < NUM_CHANNELS; chan++) {
        Mix_Volume(chan, COLLISION_VOLUME);
    }
}

void game_setup_add_instructions(scene_t *scene){
    list_t *instr_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *instr_image = IMG_Load("images/instructions-background.png");
    body_t *instr = body_init_with_info(instr_list, INFINITY, WHITE_COLOR, instr_image,
                                        HIGH_RIGHT_CORNER.x, HIGH_RIGHT_CORNER.y, "INSTRUCTIONS", NULL);
    body_set_centroid(instr, (vector_t) {HIGH_RIGHT_CORNER.x / 2, HIGH_RIGHT_CORNER.y / 2});
    scene_add_body(scene, instr);

    // instructions back button
    list_t *instr_quit_list = rect_init(SLIDER_WIDTH, SLIDER_HEIGHT);
    SDL_Surface *instr_quit_image = IMG_Load("images/x-button.png");
    body_t *instr_quit_button = body_init_with_info(instr_quit_list, INFINITY, WHITE_COLOR,
                                                    instr_quit_image, QUIT_BUTTON_SIDE_LENGTH,
                                                    QUIT_BUTTON_SIDE_LENGTH, "INSTR_QUIT", NULL);
    body_set_centroid(instr_quit_button, QUIT_BUTTON_CENTROID);
    scene_add_body(scene, instr_quit_button);
}

void game_setup(scene_t *scene, TTF_Font *font){
    game_setup_add_background(scene);
    game_setup_add_table(scene);
    game_setup_add_initial_line(scene);
    game_setup_add_balls(scene);
    game_setup_add_stick(scene);
    game_setup_add_walls(scene);
    game_setup_add_holes(scene);
    game_setup_add_slider(scene);
    game_setup_add_text(scene, font);
    game_setup_add_in_game_buttons(scene);
    game_setup_add_start_menu(scene, font);
    sound_setup();
}
