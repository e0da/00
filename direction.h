#ifndef DIRECTION_H
#define DIRECTION_H

/* The order is RIGHT, DOWN, LEFT, UP so that
  1. it's clockwise
  2. RIGHT is 0, so facing right is the default like god intended */
typedef enum DIRECTION { RIGHT, DOWN, LEFT, UP } Direction;

#endif
