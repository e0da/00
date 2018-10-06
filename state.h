#ifndef STATE_H
#define STATE_H

#include "types.h"

struct State {
  Uint32 tick;
  Engine *engine;
  Bug *bug;
};

State *state_create(const State *initialState);
void state_destroy(State *state);

#endif
