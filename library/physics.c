#include <math.h>
#include <stdlib.h>
#include "polygon.h"
#include "vector.h"
#include "list.h"

const double GRAVITY = 0.01; // pixels/s^2
const double ENERGY_LOSS = 0.2;

double physics_acceleration(double velocity_y){
    velocity_y -= GRAVITY;
    return velocity_y;
}

vector_t physics_bounce_y(list_t *star, vector_t *velocity, vector_t curr_pos, vector_t low_left_corner){
    int vertical = 0;
    // translate first and then check for edge behavior
    for(size_t i = 0; i < list_size(star); i++){
        double d;
        vector_t diff = {0, 0};
        // checking bottom edge
        if(( (vector_t *) list_get(star, i))->y <= low_left_corner.y){
            vertical = 1;
            d = low_left_corner.y - ( (vector_t *) list_get(star, i))->y;
        }
        if(vertical){
            diff.y = d;
            velocity->y *= -1 * (1 - ENERGY_LOSS);
            polygon_translate(star, diff);
            return vec_add(curr_pos, diff);
        }
    }
    return curr_pos;
}

