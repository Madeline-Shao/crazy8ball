#ifndef __FORCE_WRAPPER_H__
#define __FORCE_WRAPPER_H__

#include "body.h"
#include "scene.h"
#include "forces.h"
#include "vector.h"
#include "force_params.h"

/**
 * A wrapper for the force parameters, the force creator itself,
 * and the free function needed.
 */
typedef struct force_wrapper force_wrapper_t;

/**
 * Allocates memory for a force wrapper with the given parameters.
 *
 * @param func - the force creator
 * @param params - the parameters for the force creator
 * @param freefunc - the free function for the force creator
 * @return a pointer to the newly allocated body
 */
force_wrapper_t *force_wrapper_init(force_creator_t func, void *params, free_func_t freefunc, list_t *bodies);

/**
 * Frees the force parameter and the wrapper itself.
 * @param wrapper - the force wrapper to free.
 */
void force_wrapper_free(force_wrapper_t *wrapper);

/**
 * Returns the list of bodies associated with the force wrapper.
 * @param wrapper - the force wrapper.
 * @return a list of bodies
 */
list_t *force_wrapper_bodies(force_wrapper_t *wrapper);

/**
 * Applies the force creator function to the parameters.
 * @param wrapper - the force wrapper of which the function is applied.
 */
void force_wrapper_apply(force_wrapper_t *wrapper);

#endif // #ifndef __FORCE_WRAPPER_H__
