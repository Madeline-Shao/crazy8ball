#include "power_effects.h"

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
        snprintf(opponent_type, STRIPED_TEXT_LENGTH, "STRIPED_BALL");
    }
    else{
        snprintf(opponent_type, SOLID_TEXT_LENGTH, "SOLID_BALL");
    }

    for(size_t i = 0; i < scene_bodies(scene); i++){
        body_t *body = scene_get_body(scene, i);
        if(!strcmp(body_get_info(body), opponent_type)){
            body_set_mass(body, mass);
            if(mass == 0){
                body_set_color(body, GRAY_TRANSPARENT_COLOR);
            }
            else{
                body_set_color(body, WHITE_COLOR);
            }
        }
    }
}

void add_balls_powerup(scene_t *scene, collision_handler_t balls_collision_handler, collision_handler_t ball_destroy){
    list_t *ball_list = list_init(2, (free_func_t) body_free);

    if (!strcmp(game_state_get_current_type(scene_get_game_state(scene)), "SOLID_BALL")){
        for (int i = 0; i < 2; i++){
            SDL_Surface *image = IMG_Load("images/special_striped_ball.png");
            body_t *ball = create_ball("STRIPED_BALL", image);
            list_add(ball_list, ball);
        }
    }
    else {
        for (int i = 0; i < 2; i++){
            SDL_Surface *image = IMG_Load("images/special_solid_ball.png");
            body_t *ball = create_ball("SOLID_BALL", image);
            list_add(ball_list, ball);
        }
    }

    double maxx = body_get_centroid(get_object(scene, "POOL_TABLE")).x + TABLE_WIDTH / 2 - TABLE_WALL_THICKNESS
                                    - WALL_THICKNESS / 2 - BALL_RADIUS;
    double minx = body_get_centroid(get_object(scene, "POOL_TABLE")).x - TABLE_WIDTH / 2 + TABLE_WALL_THICKNESS
                                    + WALL_THICKNESS / 2 + BALL_RADIUS;
    double maxy = body_get_centroid(get_object(scene, "POOL_TABLE")).y + TABLE_HEIGHT / 2 - TABLE_WALL_THICKNESS
                                    - WALL_THICKNESS / 2 - BALL_RADIUS;
    double miny = body_get_centroid(get_object(scene, "POOL_TABLE")).y - TABLE_HEIGHT/ 2 + TABLE_WALL_THICKNESS
                                    + WALL_THICKNESS / 2 + BALL_RADIUS;

    for (int i = 0; i < list_size(ball_list); i++){
        int xcoord;
        int ycoord;
        bool brake = false;
        while (!brake){
            xcoord = rand() / (float) RAND_MAX * (maxx - minx) + minx;
            ycoord = rand() / (float) RAND_MAX * (maxy - miny) + miny;

            for (int i = 0; i < scene_bodies(scene); i++) {
                body_t *body = scene_get_body(scene, i);
                if (!strcmp(body_get_info(body), "STRIPED_BALL") || !strcmp(body_get_info(body), "SOLID_BALL")
                    || !strcmp(body_get_info(body), "8_BALL")) {
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

    for (int i = 0; i < list_size(ball_list); i++){
        body_t *ball = list_get(ball_list, i);
        create_friction(scene, MU, G, ball);
        for(int j = 0; j < scene_bodies(scene); j++){
            body_t *body = scene_get_body(scene, j);
            if(!strcmp(body_get_info(body), "SOLID_BALL") || !strcmp(body_get_info(body), "STRIPED_BALL")
               || !strcmp(body_get_info(body), "8_BALL") || !strcmp(body_get_info(body), "CUE_BALL")
               || !strcmp(body_get_info(body), "WALL")){
                create_collision(scene, ball, body, balls_collision_handler, NULL, NULL);
            }
            else if(!strcmp(body_get_info(body), "HOLE")){
                create_collision(scene, ball, body, ball_destroy, scene, NULL);
            }
        }
        scene_add_body(scene, ball);
    }
}
