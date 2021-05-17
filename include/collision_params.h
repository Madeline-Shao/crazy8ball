#ifndef __COLLISION_PARAMS_H__
#define __COLLISION_PARAMS_H__

#include "body.h"
#include "scene.h"
#include "vector.h"
#include "forces.h"

/**
 * An encapsulation for all the parameters that are
 * needed for the collision creators.
 */ 
typedef struct collision_params collision_params_t;

/**
 * Allocates memory for a force parameter with the given parameters.
 *
 * @param constant - the constant corresponding to the force
 * @param body_1 - a pointer to the first body needed for the force
 * @param body_2 - a pointer to the second body needed for the force
 * @return a pointer to the newly allocated body
 */
collision_params_t *collision_params_init(void *constant, body_t *body_1, body_t *body_2, collision_handler_t collision_creator);

void *collision_params_get_aux(collision_params_t *params);

/**
 * Frees the force parameter.
 * @param params - the force parameter to free.
 */ 
void collision_params_free(collision_params_t *params);

/**
 * Returns the first body in the given force parameter.
 * @param params - the force parameter to free.
 * @return the first body (body_t)
 */  
body_t *collision_params_get_body_1(collision_params_t *params);

/**
 * Returns the second body in the given force parameter.
 * @param params - the force parameter to free.
 * @return the second body (body_t)
 */  
body_t *collision_params_get_body_2(collision_params_t *params);

collision_handler_t collision_get_handler(collision_params_t *params);

bool collision_params_get_colliding(collision_params_t *params);

void collision_params_set_colliding(collision_params_t *params, bool val);

#endif // #ifndef __COLLISION_PARAMS_H__