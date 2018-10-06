#ifndef STATE_H
#define STATE_H

#include "bug.h"
#include "engine.h"

typedef struct State {
  uint32_t tick;
  Engine *engine;
  Bug *bug;
} State;

State *state_create(const State *initialState);
void state_destroy(State *state);

#endif
