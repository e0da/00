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

static void handle_sdl_events(State *state);
static void handle_keyboard_events(State *state);
static void handle_mouse_events(State *state);
static void handle_controller_events(State *state);

void handle_events(State *state) {
  handle_sdl_events(state);
  handle_keyboard_events(state);
  handle_mouse_events(state);
  handle_controller_events(state);
}

void emit(State *state, Event event, void *payload) {
  switch (event) {
  case NO_EVENT:
    break;
  case TICK:
    state->tick++;
    break;
  case MOVE_BUG:
    move_bug(state->bug, *(Direction *)payload, state->engine->window_width,
             state->engine->window_height);
    break;
  case QUIT:
    state->quit(state);
    break;
  }
}

static void handle_sdl_events(State *state) {
  SDL_Event event;
  SDL_PollEvent(&event);
  switch (event.type) {
  case SDL_QUIT:
    emit(state, QUIT, NULL);
  default:
    break;
  }
}

static void handle_keyboard_events(State *state) {
  // held keys
  const Uint8 *keys = SDL_GetKeyboardState(NULL);

  // directions
  Direction x_direction = NO_DIRECTION;
  Direction y_direction = NO_DIRECTION;
  if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
    x_direction = LEFT;
  if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
    x_direction = RIGHT;
  if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
    y_direction = UP;
  if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
    y_direction = DOWN;

  if (x_direction)
    emit(state, MOVE_BUG, &x_direction);
  if (y_direction)
    emit(state, MOVE_BUG, &y_direction);

  // quit
  if (keys[SDL_SCANCODE_ESCAPE])
    emit(state, QUIT, NULL);
}

static void handle_mouse_events(State *state) {
  Engine *engine = state->engine;
  Bug *bug = state->bug;

  // Move toward the cursor while holding left click
  int x_pointer, y_pointer;
  if (SDL_GetMouseState(&x_pointer, &y_pointer) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
    // Invert y_pointer so y increases upward
    y_pointer = engine->scaled_window_height - y_pointer;
    const int renderer_scale = engine->renderer_scale;
    const int x_distance = x_pointer - (bug->x * renderer_scale);
    const int y_distance = y_pointer - (bug->y * renderer_scale);
    // Don't move if we'd overshoot
    const int threshold = bug->speed * renderer_scale;
    Direction x_direction = NO_DIRECTION;
    Direction y_direction = NO_DIRECTION;
    if (abs(x_distance) > threshold) {
      if (x_distance > 0)
        x_direction = RIGHT;
      if (x_distance < 0)
        x_direction = LEFT;
    }
    if (abs(y_distance) > threshold) {
      if (y_distance < 0)
        y_direction = DOWN;
      if (y_distance > 0)
        y_direction = UP;
    }
    if (x_direction)
      emit(state, MOVE_BUG, &x_direction);
    if (y_direction)
      emit(state, MOVE_BUG, &y_direction);
  }
}

static void handle_controller_events(State *state) {
  Engine *engine = state->engine;
  SDL_GameController *controller = engine->controller;
  // alias SDL_GameControllerGetButton to button
  static Uint8 (*button)(SDL_GameController * controller,
                         SDL_GameControllerButton button) =
      SDL_GameControllerGetButton;
  if (controller) {
    Direction x_direction = NO_DIRECTION;
    Direction y_direction = NO_DIRECTION;
    if (button(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
      x_direction = LEFT;
    if (button(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
      x_direction = RIGHT;
    if (button(controller, SDL_CONTROLLER_BUTTON_DPAD_UP))
      y_direction = UP;
    if (button(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
      y_direction = DOWN;

    if (x_direction)
      emit(state, MOVE_BUG, &x_direction);
    if (y_direction)
      emit(state, MOVE_BUG, &y_direction);
  }
}
