#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "color.h"
#include "list.h"
#include "vector.h"
#include "body.h"
#include "polygon.h"
#include <SDL2/SDL_image.h>

const double ORIGINAL_ANGLE = 0.0;

typedef struct body{
    list_t *shape;
    double mass;
    rgb_color_t color;
    vector_t centroid;
    vector_t *velocity;
    double angle;
    vector_t origin;
    vector_t force;
    vector_t impulse;
    void *info;
    free_func_t info_freer;
    bool removed_marker;
    SDL_Surface *image;
    double height;
    double width;
} body_t;

body_t *body_init(list_t *shape, double mass, rgb_color_t color){
    body_t *new_body = malloc(sizeof(body_t));
    assert(new_body != NULL);

    new_body->shape = shape;
    new_body->mass = mass;
    new_body->color = color;

    vector_t *velocity = malloc(sizeof(vector_t));
    velocity->x = 0;
    velocity->y = 0;
    new_body->velocity = velocity;

    vector_t centroid = polygon_centroid(shape);
    new_body->centroid = centroid;


    new_body->angle = ORIGINAL_ANGLE;

    new_body->origin = centroid;

    polygon_rotate(new_body->shape, new_body->angle, new_body->centroid);

    vector_t force = {0, 0};
    new_body->force = force;

    vector_t impulse = {0, 0};
    new_body->impulse = impulse;

    new_body->removed_marker = false;

    new_body->info_freer = NULL;

    return new_body;
}

body_t *body_init_with_info(
    list_t *shape,
    double mass,
    rgb_color_t color,
    SDL_Surface *image,
    double width,
    double height,
    void *info,
    free_func_t info_freer
){
    body_t *new_body = body_init(shape, mass, color);
    new_body->image = image;
    new_body->width = width;
    new_body->height = height;
    new_body->info = info;
    new_body->info_freer = info_freer;

    return new_body;
}

void body_free(body_t *body){
    if (body->info_freer != NULL)
        body->info_freer(body->info);
    list_free(body->shape);
    free(body->velocity);
    SDL_FreeSurface(body->image);
    free(body);
}

list_t *body_get_shape(body_t *body){
    list_t *shape_copy = list_init(list_size(body->shape), (free_func_t)free);
    for (int i = 0; i < list_size(body->shape); i++){
        vector_t *v = malloc(sizeof(vector_t));
        v->x = ((vector_t *) list_get(body->shape, i))->x;
        v->y = ((vector_t *) list_get(body->shape, i))->y;
        list_add(shape_copy, (void *)v);
    }

    return shape_copy;
}

vector_t body_get_centroid(body_t *body){
    return body->centroid;
}

vector_t body_get_velocity(body_t *body){
    return *(body->velocity);
}

rgb_color_t body_get_color(body_t *body){
    return body->color;
}

double body_get_mass(body_t *body){
    return body->mass;
}

double body_get_angle(body_t *body) {
    return body->angle;
}

void *body_get_info(body_t *body){
    return body->info;
}

SDL_Surface *body_get_image(body_t *body){
    return body->image;
}

double body_get_height(body_t *body){
    return body->height;
}

double body_get_width(body_t *body){
    return body->width;
}

vector_t body_get_origin(body_t *body){
    return body->origin;
}

void body_set_height(body_t *body, double height){
    body->height = height;
}

void body_set_width(body_t *body, double width){
    body->width = width;
}

void body_set_image(body_t *body, SDL_Surface *image) {
    SDL_FreeSurface(body->image);
    body->image = image;
}

void body_set_mass(body_t *body, double mass){
    body->mass = mass;
}

void body_set_centroid(body_t *body, vector_t x){
    polygon_translate(body->shape, vec_subtract(x, body->centroid));
    body->centroid = x;
}

void body_set_shape(body_t *body, list_t *shape, vector_t current_pos){
    list_free(body->shape);
    body->shape = shape;
    body->centroid = polygon_centroid(shape);
    body_set_centroid(body, current_pos);
}

void body_set_velocity(body_t *body, vector_t v){
    vector_t *velocity = body->velocity;
    velocity->x = v.x;
    velocity->y = v.y;
}

void body_set_rotation(body_t *body, double angle){
    polygon_rotate(body->shape, angle - body->angle, body->origin);
    body->angle = angle;
    body->centroid = polygon_centroid(body->shape);
}

void body_set_origin(body_t *body, vector_t origin){
    body->origin = origin;
}

void body_set_color(body_t *body, rgb_color_t color){
    body->color = color;
}

void body_add_force(body_t *body, vector_t force){
    vector_t new_force = {body->force.x + force.x, body->force.y + force.y};
    body->force = new_force;
}

void body_add_impulse(body_t *body, vector_t impulse){
    vector_t new_imp = {body->impulse.x + impulse.x, body->impulse.y + impulse.y};
    body->impulse = new_imp;
}

void body_tick(body_t *body, double dt){
    // update acceleration
    vector_t acceleration = VEC_ZERO;
    if(body->mass != 0.0){
        acceleration = vec_multiply(1 / body->mass, body->force);
    }
    // calculate new velocity from accel and impulse

    vector_t new_vel = vec_add(*(body->velocity), vec_multiply(dt, acceleration));
    if(body->mass != 0.0){
        new_vel = vec_add(new_vel, vec_multiply(1 / body->mass, body->impulse));
    }
    else{
        new_vel = VEC_ZERO;
    }
    // translate by average of old and new velocity
    vector_t average = vec_multiply(0.5, vec_add(*(body->velocity), new_vel));

    body_set_centroid(body, vec_add(body_get_centroid(body), vec_multiply(dt, average)));

    // update velocity
    body->velocity->x = new_vel.x;
    body->velocity->y = new_vel.y;

    // reset forces/impulses
    body->force = (vector_t) {0, 0};
    body->impulse = (vector_t) {0, 0};

}

void body_remove(body_t *body){
    body->removed_marker = true;
}

bool body_is_removed(body_t *body){
    return body->removed_marker;
}
