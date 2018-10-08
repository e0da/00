#include "input.h"
#include "direction.h"
#include "state.h"
#include <SDL2/SDL.h>
#include <math.h>

void input_handle_events(State *state, void (*quit_callback)(State *)) {
  { /* key presses */
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
      quit_callback(state);
    default:
      break;
    }
  }

  Bug *bug = state->bug;
  Engine *engine = state->engine;
  const int window_width = engine->window_width;
  const int window_height = engine->window_height;

  { /* held keys */
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

  { /* when the left mouse button is held move toward the cursor unless it's
       so close that we'd overshoot */
    int pointer_x, pointer_y;
    if (SDL_GetMouseState(&pointer_x, &pointer_y) &
        SDL_BUTTON(SDL_BUTTON_LEFT)) {
      // Invert pointer_y so y increases upward
      pointer_y = engine->scaled_window_height - pointer_y;
      const int renderer_scale = engine->renderer_scale;
      const int distance_x = pointer_x - (bug->x * renderer_scale);
      const int distance_y = pointer_y - (bug->y * renderer_scale);
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

  /* game controller */
  SDL_GameController *controller = engine->controller;
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
