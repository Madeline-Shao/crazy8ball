#include "vector.h"
#include <math.h>
#include <stdlib.h>

const vector_t VEC_ZERO = {0, 0};

vector_t vec_add(vector_t v1, vector_t v2){
    double v_sum_x = v1.x + v2.x;
    double v_sum_y = v1.y + v2.y;
    vector_t v_sum = {v_sum_x, v_sum_y};
    return v_sum;
}

vector_t vec_subtract(vector_t v1, vector_t v2){
    double v_diff_x = v1.x - v2.x;
    double v_diff_y = v1.y - v2.y;
    vector_t v_diff = {v_diff_x, v_diff_y};
    return v_diff;
}

vector_t vec_negate(vector_t v){
    double v_neg_x = v.x * -1;
    double v_neg_y = v.y * -1;
    vector_t v_neg = {v_neg_x, v_neg_y};
    return v_neg;
}

vector_t vec_multiply(double scalar, vector_t v){
    double v_prod_x = v.x * scalar;
    double v_prod_y = v.y * scalar;
    vector_t v_prod = {v_prod_x, v_prod_y};
    return v_prod;
}

double vec_dot(vector_t v1, vector_t v2){
    double dot_prod = v1.x * v2.x + v1.y * v2.y;
    return dot_prod;
}

double vec_cross(vector_t v1, vector_t v2){
    double z_comp = v1.x * v2.y - v1.y * v2.x;
    return z_comp;
}

vector_t vec_rotate(vector_t v, double angle){
    double v_rotated_x = v.x * cos(angle) - v.y * sin(angle);
    double v_rotated_y = v.x * sin(angle) + v.y * cos(angle);
    vector_t v_rotated = {v_rotated_x, v_rotated_y};
    return v_rotated;
}

int vec_equal(vector_t v1, vector_t v2){
    return v1.x == v2.x && v1.y == v2.y;
}
