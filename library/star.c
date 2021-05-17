#include "star.h"
#include "vector.h"
#include "color.h"
#include "polygon.h"
#include "list.h"
#include <stdlib.h>
#include <math.h>
#include <assert.h>

typedef struct star {
    list_t *points;
    vector_t *velocity;
    vector_t *translation;
    rgb_color_t rgb_color;
} star_t;

star_t *star_init(int pts, double radius, vector_t vel, vector_t trans_vec, rgb_color_t rgb_color){
    star_t *new = malloc(sizeof(star_t));
    assert(new != NULL);
    new->points = list_init(pts, (free_func_t) free);
    new->velocity = malloc(sizeof(vector_t));
    new->translation = malloc(sizeof(vector_t));
    assert(new->velocity != NULL);
    assert(new->translation != NULL);

    double const_ang = 2 * M_PI / pts;
    double angle = 0.0;
    for(int i = 0; i < pts; i++){
        vector_t *temp_v = malloc(sizeof(vector_t));
        // if i is even, full radius
        // if i is odd, only need a portion of the radius
        if(i % 2 == 0){
            temp_v->x = radius * cos(angle);
            temp_v->y = radius * sin(angle);
        }
        else{
            temp_v->x = (radius / 2) * cos(angle);
            temp_v->y = (radius / 2) * sin(angle);
        }
        list_add(new->points, (void *) temp_v);
        
        // turn it with angle in each point
        angle += const_ang;
    }
    
    new->velocity->x = vel.x;
    new->velocity->y = vel.y;
    new->translation->x = trans_vec.x;
    new->translation->y = trans_vec.y;

    star_move(new, trans_vec);

    new->rgb_color = rgb_color;

    return new;
}

rgb_color_t star_get_color(star_t *star){
    return star->rgb_color;
}

list_t *star_get_points(star_t *star){
    return star->points;
}

vector_t *star_get_vel(star_t *star){
    return star->velocity;
}

vector_t *star_get_trans(star_t *star){
    return star->translation;
}

void star_move(star_t *star, vector_t translate){
    polygon_translate(star->points, translate);
}

void star_free(star_t *star){
    list_free(star->points);
    free(star->velocity);
    free(star->translation);
    free(star);
}