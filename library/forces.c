#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "body.h"
#include "list.h"
#include "scene.h"
#include "force_wrapper.h"
#include "force_params.h"
#include "collision_params.h"
#include "collision.h"

const double GRAVITY_DISTANCE_BOUND = 5;
const double DESTRUCTIVE_CONSTANT = 1.0;

void newtonian_gravity_creator(two_body_params_t *params){
    double x1 = body_get_centroid(two_body_params_get_body_1(params)).x;
    double y1 = body_get_centroid(two_body_params_get_body_1(params)).y;
    double x2 = body_get_centroid(two_body_params_get_body_2(params)).x;
    double y2 = body_get_centroid(two_body_params_get_body_2(params)).y;

    double r = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    if (r > GRAVITY_DISTANCE_BOUND) {
        double magnitude = two_body_params_get_constant(params)
                         * body_get_mass(two_body_params_get_body_1(params))
                         * body_get_mass(two_body_params_get_body_2(params)) / (r * r);
        // apply the magnitude as a vector on each body using body_add_force
        vector_t force12 = (vector_t) {magnitude * (x2 - x1) / r, magnitude * (y2 - y1) / r};

        vector_t force21 = (vector_t) {magnitude * (x1 - x2) / r, magnitude * (y1 - y2) / r};
        body_add_force(two_body_params_get_body_1(params), force12);
        body_add_force(two_body_params_get_body_2(params), force21);
   }

}

void create_newtonian_gravity(scene_t *scene, double G, body_t *body1, body_t *body2){
    two_body_params_t *params = two_body_params_init(G, body1, body2);
    list_t *bodies = list_init(2, (free_func_t) body_free);
    list_add(bodies, body1);
    list_add(bodies, body2);
    scene_add_bodies_force_creator(scene, (force_creator_t) newtonian_gravity_creator,
                            params, bodies, (free_func_t) two_body_params_free);
}

void spring_creator(two_body_params_t *params){
    double x1 = body_get_centroid(two_body_params_get_body_1(params)).x;
    double y1 = body_get_centroid(two_body_params_get_body_1(params)).y;
    double x2 = body_get_centroid(two_body_params_get_body_2(params)).x;
    double y2 = body_get_centroid(two_body_params_get_body_2(params)).y;

    double x = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    double magnitude = two_body_params_get_constant(params) * x;
    if (x > 1e-4){
        // apply to both bodies using body_add_force;
        vector_t force12 = (vector_t) {magnitude * (x2 - x1) / x, magnitude * (y2 - y1) / x};
        body_add_force(two_body_params_get_body_1(params), force12);

        vector_t force21 = (vector_t) {magnitude * (x1 - x2) / x, magnitude * (y1 - y2) / x};
        body_add_force(two_body_params_get_body_2(params), force21);
    }
}

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2){
    two_body_params_t *params = two_body_params_init(k, body1, body2);
    list_t *bodies = list_init(2, (free_func_t) body_free);
    list_add(bodies, body1);
    list_add(bodies, body2);
    scene_add_bodies_force_creator(scene, (force_creator_t) spring_creator,
                            params, bodies, (free_func_t) two_body_params_free);
}

void drag_creator(one_body_params_t *params){
    body_add_force(one_body_params_get_body(params),
                    vec_negate(vec_multiply(one_body_params_get_constant(params),
                    body_get_velocity(one_body_params_get_body(params)))));
}

void create_drag(scene_t *scene, double gamma, body_t *body){
    one_body_params_t *params = one_body_params_init(gamma, body);
    list_t *bodies = list_init(1, (free_func_t) body_free);
    list_add(bodies, body);
    scene_add_bodies_force_creator(scene, (force_creator_t) drag_creator,
                            params, bodies, (free_func_t) one_body_params_free);
}

void friction_creator(two_constants_one_body_params_t *params){
    if(!vec_equal(VEC_ZERO, body_get_velocity(two_constants_one_body_params_get_body(params)))){
        double x = body_get_velocity(two_constants_one_body_params_get_body(params)).x;
        double y = body_get_velocity(two_constants_one_body_params_get_body(params)).y;
        double r = sqrt(pow(x, 2) + pow(y, 2));

        double force_component = -1 * two_constants_one_body_params_get_constant1(params)
                                * two_constants_one_body_params_get_constant2(params)
                                * body_get_mass(two_constants_one_body_params_get_body(params));
        // if (r < 0.5) {
        //     body_set_velocity(two_constants_one_body_params_get_body(params), (vector_t){0, 0});
        // }
        body_add_force(two_constants_one_body_params_get_body(params), (vector_t) {force_component * (x / r), force_component * (y / r)});
    }
}

void create_friction(scene_t *scene, double mu, double g, body_t *body){
    two_constants_one_body_params_t *params = two_constants_one_body_params_init(mu, g, body);
    list_t *bodies = list_init(1, (free_func_t) body_free);
    list_add(bodies, body);
    scene_add_bodies_force_creator(scene, (force_creator_t) friction_creator,
                            params, bodies, (free_func_t) two_constants_one_body_params_free);
}
// Check collisions
void collision_creator(collision_params_t *params){
    body_t *body1 = collision_params_get_body_1(params);
    body_t *body2 = collision_params_get_body_2(params);
    list_t *shape1 = body_get_shape(body1);
    list_t *shape2 = body_get_shape(body2);
    collision_info_t collided = find_collision(shape1, shape2);
    if (collided.collided && !collision_params_get_colliding(params)){
        collision_params_set_colliding(params, true);
        collision_handler_t handler = collision_get_handler(params);
        vector_t axis = collided.axis;
        handler(body1, body2, axis, collision_params_get_aux(params));
    }
    else if(!collided.collided){
        collision_params_set_colliding(params, false);
    }
    list_free(shape1);
    list_free(shape2);
}


void create_collision(
    scene_t *scene,
    body_t *body1,
    body_t *body2,
    collision_handler_t handler,
    void *aux,
    free_func_t freer
) {
    list_t *bodies = list_init(2, (free_func_t) body_free);
    list_add(bodies, body1);
    list_add(bodies, body2);

    collision_params_t *params = collision_params_init(aux, body1, body2, handler);
    scene_add_bodies_force_creator(scene, (force_creator_t) collision_creator,
                                    params, bodies, (free_func_t) collision_params_free);
}

void destructive_collision_handler(body_t *body1, body_t *body2, vector_t axis, void *aux){
    body_remove(body1);
    body_remove(body2);
}

void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2){
    create_collision(scene, body1, body2, destructive_collision_handler, NULL, (free_func_t)collision_params_free);
}

void physics_collision_handler(body_t *body1, body_t *body2, vector_t axis, void *aux){
    double u1 = vec_dot(axis, body_get_velocity(body1));
    double u2 = vec_dot(axis, body_get_velocity(body2));
    double reduced_mass;
    if (body_get_mass(body1) == INFINITY) {
        reduced_mass = body_get_mass(body2);
    }
    else if (body_get_mass(body2) == INFINITY) {
        reduced_mass = body_get_mass(body1);
    }
    else {
        reduced_mass = (body_get_mass(body1) * body_get_mass(body2) / (body_get_mass(body1) + body_get_mass(body2)));
    }
    vector_t impulse = vec_multiply((reduced_mass * (1 + *((double *)aux)) * (u2 - u1)), axis);
    body_add_impulse(body1, impulse);
    body_add_impulse(body2, vec_multiply(-1, impulse));
}

void create_physics_collision(scene_t *scene, double constant, body_t *body1, body_t *body2){
    double *phys_constant = malloc(sizeof(double));
    *phys_constant = constant;
    create_collision(scene, body1, body2, physics_collision_handler, phys_constant, (free_func_t)collision_params_free);
}
