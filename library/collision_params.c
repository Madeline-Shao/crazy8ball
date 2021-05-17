#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "list.h"
#include "vector.h"
#include "body.h"
#include "scene.h"
#include "collision_params.h"

typedef struct collision_params{
    void *aux;
    body_t *body1;
    body_t *body2;
    collision_handler_t collision_creator;
    bool colliding;
} collision_params_t;

collision_params_t *collision_params_init(void *aux, body_t *body_1, body_t *body_2, collision_handler_t collision_creator){
    collision_params_t *params = malloc(sizeof(collision_params_t));
    params->aux = aux;
    params->body1 = body_1;
    params->body2 = body_2;
    params->collision_creator = collision_creator;
    params->colliding = false;
    return params;
}

void collision_params_free(collision_params_t *params){
    free(params);
}

void *collision_params_get_aux(collision_params_t *params){
    return params->aux;
}
 
body_t *collision_params_get_body_1(collision_params_t *params){
    return params->body1;
}
 
body_t *collision_params_get_body_2(collision_params_t *params){
    return params->body2;
}

collision_handler_t collision_get_handler(collision_params_t *params){
    return params->collision_creator;
}

bool collision_params_get_colliding(collision_params_t *params){
    return params->colliding;
}

void collision_params_set_colliding(collision_params_t *params, bool val){
    params->colliding = val;
}
