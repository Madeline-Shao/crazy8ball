#ifndef __FORCE_PARAMS_H__
#define __FORCE_PARAMS_H__

#include "body.h"
#include "scene.h"
#include "vector.h"

/**
 * An encapsulation for all the parameters that are
 * needed for the force creators.
 */ 
typedef struct two_body_params two_body_params_t;

typedef struct one_body_params one_body_params_t;
typedef struct two_constants_one_body_params two_constants_one_body_params_t;
/**
 * Allocates memory for a force parameter with the given parameters.
 *
 * @param constant - the constant corresponding to the force
 * @param body_1 - a pointer to the first body needed for the force
 * @param body_2 - a pointer to the second body needed for the force
 * @return a pointer to the newly allocated body
 */
two_body_params_t *two_body_params_init(double constant, body_t *body_1, body_t *body_2);

/**
 * Frees the force parameter.
 * @param params - the force parameter to free.
 */ 
void two_body_params_free(two_body_params_t *params);

/**
 * Returns the constant in the given force parameter.
 * @param params - the force parameter to free.
 * @return the constant (double)
 */  
double two_body_params_get_constant(two_body_params_t *params);

/**
 * Returns the first body in the given force parameter.
 * @param params - the force parameter to free.
 * @return the first body (body_t)
 */  
body_t *two_body_params_get_body_1(two_body_params_t *params);

/**
 * Returns the second body in the given force parameter.
 * @param params - the force parameter to free.
 * @return the second body (body_t)
 */  
body_t *two_body_params_get_body_2(two_body_params_t *params);


/**
 * Allocates memory for a force parameter with the given parameters.
 *
 * @param constant - the constant corresponding to the force
 * @param body_1 - a pointer to the first body needed for the force
 * @param body_2 - a pointer to the second body needed for the force
 * @return a pointer to the newly allocated body
 */
one_body_params_t *one_body_params_init(double constant, body_t *body);

/**
 * Frees the force parameter.
 * @param params - the force parameter to free.
 */ 
void one_body_params_free(one_body_params_t *params);

/**
 * Returns the constant in the given force parameter.
 * @param params - the force parameter to free.
 * @return the constant (double)
 */  
double one_body_params_get_constant(one_body_params_t *params);

/**
 * Returns the first body in the given force parameter.
 * @param params - the force parameter to free.
 * @return the first body (body_t)
 */  
body_t *one_body_params_get_body(one_body_params_t *params);

two_constants_one_body_params_t *two_constants_one_body_params_init(double constant1, double constant2, body_t *body);

void two_constants_one_body_params_free(two_constants_one_body_params_t *params);
  
double two_constants_one_body_params_get_constant1(two_constants_one_body_params_t *params);

double two_constants_one_body_params_get_constant2(two_constants_one_body_params_t *params);
 
body_t *two_constants_one_body_params_get_body(two_constants_one_body_params_t *params);


#endif // #ifndef __FORCE_PARAMS_H__
