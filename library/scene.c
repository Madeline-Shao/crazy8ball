#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include "body.h"
#include "list.h"
#include "scene.h"
#include "force_wrapper.h"
#include "game_state.h"

const double INITIAL_SCENE_SIZE = 100;
const double INITIAL_FORCE_SIZE = 100;

typedef struct scene{
    list_t *body_list;
    list_t *force_list;
    game_state_t *game_state;
} scene_t;

scene_t *scene_init(void){
    scene_t *new_scene = malloc(sizeof(scene_t));
    assert(new_scene != NULL);
    list_t *body_list = list_init(INITIAL_SCENE_SIZE, (free_func_t)(body_free));
    new_scene->body_list = body_list;

    list_t *force_list = list_init(INITIAL_FORCE_SIZE, (free_func_t) force_wrapper_free);
    new_scene->force_list = force_list;
    return new_scene;
}

void scene_free(scene_t *scene){
    free(scene->body_list);
    list_free(scene->force_list);
    game_state_free(scene->game_state);
    free(scene);
}

size_t scene_bodies(scene_t *scene){
    return list_size(scene->body_list);
}

body_t *scene_get_body(scene_t *scene, size_t index){
    return list_get(scene->body_list, index);
}

void scene_add_body(scene_t *scene, body_t *body){
    list_add(scene->body_list, (void *) body);
}

void scene_remove_body(scene_t *scene, size_t index){
    body_remove(scene_get_body(scene, index));
}

game_state_t *scene_get_game_state(scene_t *scene){
    return scene->game_state;
}

void scene_set_game_state(scene_t *scene, game_state_t *game_state){
    scene->game_state = game_state;
}

// deprecated
void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux, free_func_t freer){
    scene_add_bodies_force_creator(scene, forcer, aux, list_init(0, (free_func_t) body_free), freer);
}

void scene_add_bodies_force_creator(
    scene_t *scene,
    force_creator_t forcer,
    void *aux,
    list_t *bodies,
    free_func_t freer
){
    force_wrapper_t *wrapper = force_wrapper_init(forcer, aux, freer, bodies);
    list_add(scene->force_list, wrapper);
}


void scene_tick(scene_t *scene, double dt){
    for (size_t i = 0; i < list_size(scene->force_list); i++){
        force_wrapper_apply((force_wrapper_t *) list_get(scene->force_list, i));
    }
    for (size_t i = 0; i < list_size(scene->force_list); i++){
        list_t *force_bodies = force_wrapper_bodies(list_get(scene->force_list, i));
        bool remove = false;
        for (size_t j = 0; j < list_size(force_bodies); j++){
            if (body_is_removed(list_get(force_bodies, j))){
                remove = true;
            }
        }
        if (remove){
            force_wrapper_t *removed_force = (force_wrapper_t *) list_remove(scene->force_list, i);
            force_wrapper_free(removed_force);
            i--;
        }
    }

    for (size_t i = 0; i < list_size(scene->body_list); i++){
        if (body_is_removed(list_get(scene->body_list, i)))
        {
            body_t *removed_body = list_remove(scene->body_list, i);
            body_free(removed_body);
            i--;
        }
        else{
            body_tick(list_get(scene->body_list, i), dt);
        }
    }
}
