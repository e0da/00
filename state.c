#include "state.h"
#include "logging.h"
#include <stdlib.h>
#include <string.h>

State *state_create(const State *initialState) {
  State *state = (State *)malloc(sizeof(State));
  if (!state) {
    WARN("%s:%d: Allocating State failed", __FILE__, __LINE__);
    return NULL;
  }
  memcpy(state, initialState, sizeof(State));
  return state;
}

void state_destroy(State *state) { free(state); }
