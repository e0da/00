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
  void (*quit)(struct State *state);
} State;

State *create_state(const State *initialState);
void destroy_state(State *state);

typedef enum Event { NO_EVENT = 0, TICK, MOVE_BUG, QUIT } Event;

void handle_events(State *state);

void emit(State *state, Event event, void *payload);

#endif
