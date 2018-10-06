#ifndef STATE_H
#define STATE_H

#include "types.h"

State *StateCreate(const State *initialState);
void StateDestroy(State *state);

#endif
