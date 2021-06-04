#include <stdbool.h>
#include <stdlib.h>
#include "collision.h"
#include "list.h"
#include "vector.h"
#include "polygon.h"
#include "color.h"
#include <math.h>

typedef struct projection{
    double min;
    double max;
} projection_t;

/**
 * Gets all the axes whose projections must be checked
 * for a given pair of polygons.
 * @param shape1 - the first polygon whose perpendicular axes are computed
 * @param shape2 - the second polygon whose perpendicular axes are computed
 * @return a list of the perpendicular axes of the two polygons.
 **/
list_t *get_axes(list_t *shape1, list_t* shape2){
    list_t *axes_list = list_init(10, (free_func_t) free);
    size_t num_vertices1 = list_size(shape1);
    size_t num_vertices2 = list_size(shape2);
    for (size_t i = 0; i < num_vertices1; i++){
        vector_t edge = vec_subtract(*((vector_t *) list_get(shape1, (i + 1) % num_vertices1)),
                                     *((vector_t *) list_get(shape1, i)));
        vector_t *edge_pointer = malloc(sizeof(vector_t));
        edge_pointer->x = edge.y * -1;
        edge_pointer->y = edge.x;
        list_add(axes_list, edge_pointer);
    }
    for (size_t i = 0; i < num_vertices2; i++){
        vector_t edge = vec_subtract(*((vector_t *) list_get(shape2, (i + 1) % num_vertices2)),
                                     *((vector_t *) list_get(shape2, i)));
        vector_t *edge_pointer = malloc(sizeof(vector_t));
        edge_pointer->x = edge.y * -1;
        edge_pointer->y = edge.x;
        list_add(axes_list, edge_pointer);
    }
    return axes_list;
}

/**
 * Computes the projection of a polygon onto an axis.
 * @param shape - the polygon whose projection is calculated
 * @param axis - the axis onto which the projection is done
 * @return the projection of shape onto axis
 **/
projection_t get_projection(list_t *shape, vector_t *axis){
    double min = vec_dot(*((vector_t *) list_get(shape, 0)), *axis);
    double max = vec_dot(*((vector_t *) list_get(shape, 0)), *axis);
    size_t num_vertices = list_size(shape);
    for (size_t i = 1; i < num_vertices; i++){
        double new_projection = vec_dot(*((vector_t *) list_get(shape, i)), *axis);
        if (new_projection < min){
            min = new_projection;
        }
        else if (new_projection > max){
            max = new_projection;
        }
    }
    projection_t new_projection = {min, max};
    return new_projection;
}

collision_info_t find_collision(list_t *shape1, list_t *shape2){
    list_t *axis_list = get_axes(shape1, shape2);
    collision_info_t collision_info;
    double min_overlap = INFINITY;
    vector_t *separating_axis = NULL;
    for (size_t i = 0; i < list_size(axis_list); i++){
        vector_t *axis = (vector_t *) list_get(axis_list, i);
        projection_t projection1 = get_projection(shape1, axis);
        projection_t projection2 = get_projection(shape2, axis);
        if (projection2.min > projection1.max || projection1.min > projection2.max){
            collision_info.collided = false;
            list_free(axis_list);
            return collision_info;
        }
        if (projection1.max - projection2.min > 0 && projection1.min < projection2.max) {
            double overlap = projection1.max - projection2.min;
            if (overlap < min_overlap) {
                min_overlap = overlap;
                separating_axis = axis;
            }
        }
        if (projection2.max - projection1.min > 0 && projection2.min < projection1.max) {
            double overlap = projection2.max - projection1.min;
            if (overlap < min_overlap) {
                min_overlap = overlap;
                separating_axis = axis;
            }
        }
    }
    collision_info.collided = true;
    double r = sqrt(pow(separating_axis->x, 2) + pow(separating_axis->y, 2));
    vector_t collision_axis = {separating_axis->x / r, separating_axis->y / r};
    collision_info.axis = collision_axis;
    list_free(axis_list);
    return collision_info;
}
