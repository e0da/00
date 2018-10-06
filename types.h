#ifndef TYPES_H
#define TYPES_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "types.h"

/* The order is RIGHT, DOWN, LEFT, UP so that
  1. it's clockwise
  2. RIGHT is 0, so facing right is the default like god intended */
typedef enum { RIGHT, DOWN, LEFT, UP } Direction;

typedef struct Bug Bug;
typedef struct Engine Engine;
typedef struct State State;

#endif
