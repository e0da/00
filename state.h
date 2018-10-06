#ifndef STATE_H
#define STATE_H

#include "bug.h"
#include "engine.h"

typedef struct State {
  Uint32 tick;
  Engine *engine;
  Bug *bug;
} State;

State *state_create(const State *initialState);
void state_destroy(State *state);

#endif
