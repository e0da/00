#include "state.h"
#include "direction.h"
#include "logging.h"
#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

State *create_state(const State *initialState) {
  State *state = (State *)malloc(sizeof(State));
  if (!state) {
    WARN("%s:%d: Allocating State failed", __FILE__, __LINE__);
    return NULL;
  }
  memcpy(state, initialState, sizeof(State));
  return state;
}

void destroy_state(State *state) { free(state); }

static void handle_sdl_events(State *state, void (*quit_callback)(State *));
static void handle_keyboard_events(State *state,
                                   void (*quit_callback)(State *));
static void handle_mouse_events(State *state);
static void handle_controller_events(State *state);

void handle_events(State *state, void (*quit_callback)(State *)) {
  handle_sdl_events(state, quit_callback);
  handle_keyboard_events(state, quit_callback);
  handle_mouse_events(state);
  handle_controller_events(state);
}

static void handle_sdl_events(State *state, void (*quit_callback)(State *)) {
  SDL_Event event;
  SDL_PollEvent(&event);
  switch (event.type) {
  case SDL_QUIT:
    quit_callback(state);
  default:
    break;
  }
}

static void handle_keyboard_events(State *state,
                                   void (*quit_callback)(State *)) {
  Bug *bug = state->bug;
  Engine *engine = state->engine;
  const int window_width = engine->window_width;
  const int window_height = engine->window_height;
  /* held keys */
  const Uint8 *keys = SDL_GetKeyboardState(NULL);
  if (keys[SDL_SCANCODE_RIGHT])
    bug_move(bug, RIGHT, window_width, window_height);
  if (keys[SDL_SCANCODE_DOWN])
    bug_move(bug, DOWN, window_width, window_height);
  if (keys[SDL_SCANCODE_LEFT])
    bug_move(bug, LEFT, window_width, window_height);
  if (keys[SDL_SCANCODE_UP])
    bug_move(bug, UP, window_width, window_height);
  if (keys[SDL_SCANCODE_ESCAPE])
    quit_callback(state);
}

static void handle_mouse_events(State *state) {
  Engine *engine = state->engine;
  Bug *bug = state->bug;
  // Move toward the cursor while holding left click
  int pointer_x, pointer_y;
  if (SDL_GetMouseState(&pointer_x, &pointer_y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
    // Invert pointer_y so y increases upward
    pointer_y = engine->scaled_window_height - pointer_y;
    const int window_width = engine->window_width;
    const int window_height = engine->window_height;
    const int renderer_scale = engine->renderer_scale;
    const int distance_x = pointer_x - (bug->x * renderer_scale);
    const int distance_y = pointer_y - (bug->y * renderer_scale);
    // Don't move if we'd overshoot
    const int threshold = bug->speed * renderer_scale;
    if (abs(distance_x) > threshold) {
      if (distance_x > 0)
        bug_move(bug, RIGHT, window_width, window_height);
      if (distance_x < 0)
        bug_move(bug, LEFT, window_width, window_height);
    }
    if (abs(distance_y) > threshold) {
      if (distance_y < 0)
        bug_move(bug, DOWN, window_width, window_height);
      if (distance_y > 0)
        bug_move(bug, UP, window_width, window_height);
    }
  }
}

static void handle_controller_events(State *state) {
  Bug *bug = state->bug;
  Engine *engine = state->engine;
  SDL_GameController *controller = engine->controller;
  const int window_width = engine->window_width;
  const int window_height = engine->window_height;
  if (controller) {
    if (SDL_GameControllerGetButton(controller,
                                    SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
      bug_move(bug, RIGHT, window_width, window_height);
    if (SDL_GameControllerGetButton(controller,
                                    SDL_CONTROLLER_BUTTON_DPAD_DOWN))
      bug_move(bug, DOWN, window_width, window_height);
    if (SDL_GameControllerGetButton(controller,
                                    SDL_CONTROLLER_BUTTON_DPAD_LEFT))
      bug_move(bug, LEFT, window_width, window_height);
    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP))
      bug_move(bug, UP, window_width, window_height);
  }
}
