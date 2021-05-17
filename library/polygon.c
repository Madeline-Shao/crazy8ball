#include <stdlib.h>
#include <math.h>
#include "list.h"
#include "vector.h"
#include "polygon.h"

double polygon_area(list_t *polygon){
    double area = 0;
    size_t polygon_size = list_size(polygon);
    for (size_t i = 0; i < polygon_size; i++){
        vector_t *current_point = (vector_t *) list_get(polygon, i);
        vector_t *next_point = (vector_t *) list_get(polygon, (i + 1) % polygon_size);
        area += vec_cross(*current_point, *next_point);
    }
    return fabs(area) / 2;
}

vector_t polygon_centroid(list_t *polygon){
    double center_x = 0;
    double center_y = 0;
    size_t polygon_size = list_size(polygon);
    for (size_t i = 0; i < polygon_size; i++){
        vector_t *current_point = (vector_t *) list_get(polygon, i);
        vector_t *next_point = (vector_t *) list_get(polygon, (i + 1) % polygon_size);
        center_x += (current_point->x + next_point->x)
                    * vec_cross(*current_point, *next_point);
        center_y += (current_point->y + next_point->y)
                    * vec_cross(*current_point, *next_point);
    }
    double area = polygon_area(polygon);
    center_x /= (6 * area);
    center_y /= (6 * area);
    vector_t center_of_mass = {center_x, center_y};
    return center_of_mass;
}

void polygon_translate(list_t *polygon, vector_t translation){
    size_t polygon_size = list_size(polygon);
    for (size_t i = 0; i < polygon_size; i++){
        vector_t *vec = (vector_t *) list_get(polygon, i);
        *vec = vec_add(*vec, translation);
    }
}

void polygon_rotate(list_t *polygon, double angle, vector_t point){
    vector_t point_negated = vec_negate(point);
    polygon_translate(polygon, point_negated);
    size_t polygon_size = list_size(polygon);
    for (size_t i = 0; i < polygon_size; i++){
        vector_t *vec = (vector_t *) list_get(polygon, i);
        *vec = vec_rotate(*vec, angle);
    }
    polygon_translate(polygon, point);
}
