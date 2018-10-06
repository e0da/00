#include "state.h"

#include "logging.h"

State *StateCreate(const State *initialState) {
  State *state = (State *)malloc(sizeof(State));
  if (!state) {
    WARN("%s:%d: Allocating State failed", __FILE__, __LINE__);
    return NULL;
  }
  memcpy(state, initialState, sizeof(State));
  return state;
}

void StateDestroy(State *state) { free(state); }
