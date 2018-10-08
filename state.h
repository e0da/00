#ifndef STATE_H
#define STATE_H

#include "bug.h"
#include "engine.h"
#include <SDL2/SDL.h>

typedef struct State {
  uint32_t tick;
  Engine *engine;
  Bug *bug;
  SDL_Texture *bug_texture;
} State;

State *state_create(const State *initialState);
void state_destroy(State *state);

#endif
