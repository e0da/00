#ifndef STATE_H
#define STATE_H

#include "bug.h"
#include "engine.h"
#include <SDL2/SDL.h>

typedef enum Event { NO_EVENT = 0, TICK } Event;

typedef struct State {
  uint32_t tick;
  Engine *engine;
  Bug *bug;
  SDL_Texture *bug_texture;
} State;

State *create_state(const State *initialState);
void destroy_state(State *state);

void handle_events(State *state, void (*quit_callback)(State *state));

void emit(State *state, Event event, void **payload);

#endif
