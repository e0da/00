#ifndef STATE_H
#define STATE_H

#include "types.h"

State *state_create(const State *initialState);
void state_destroy(State *state);

#endif
