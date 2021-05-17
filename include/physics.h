#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include <math.h>
#include <stdlib.h>
#include "polygon.h"
#include "vector.h"
#include "list.h"

/** @brief Returns the new velocity of the star in the y-direction after
 *  it accelerates due to gravity.
 *
 *  @param velocity_y the initial velocity of the star in the y-direction
 *  @return the new velocity of the star in the y-direction after the star
 *  accelerates according to gravity
 */
double physics_acceleration(double velocity_y);

/** @brief Returns the new velocity of the star in the y-direction after
 *  checking whether it collides with the top or bottom wall.
 *
 *  @param star the vector list representing the star
 *  @param velocity_y the initial velocity of the star in the y-direction
 *  @return the new velocity of the star in the y-direction after the star
 *  either does or does not collide with the top or bottom wall
 */
vector_t physics_bounce_y(list_t *star, vector_t *velocity, vector_t curr_pos,
                  vector_t low_left_corner);

#endif // #ifndef __PHYSICS_H__
