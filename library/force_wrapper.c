#include <stdlib.h>
#include <math.h>
#include "list.h"
#include "vector.h"
#include "body.h"
#include "scene.h"
#include "force_wrapper.h"
#include "force_params.h"

typedef struct force_wrapper{
    force_creator_t func;
    void *params;
    free_func_t free_func;
    list_t *bodies;
} force_wrapper_t;

force_wrapper_t *force_wrapper_init(force_creator_t func, void *params, free_func_t free_func, list_t *bodies){
    force_wrapper_t *wrapper = malloc(sizeof(force_wrapper_t));
    wrapper->func = func;
    wrapper->params = params;
    wrapper->free_func = free_func;
    wrapper->bodies = bodies;
    return wrapper;
};

list_t *force_wrapper_bodies(force_wrapper_t *wrapper){
    return wrapper->bodies;
}

void force_wrapper_free(force_wrapper_t *wrapper){
    if (wrapper->free_func != NULL){
        wrapper->free_func(wrapper->params);
    }
    free(wrapper);
}

void force_wrapper_apply(force_wrapper_t *wrapper){
    wrapper->func(wrapper->params);
}
