#include "input.h"
#include "direction.h"
#include "state.h"
#include <SDL2/SDL.h>

void input_handle_events(State *state, void (*quit_callback)(void)) {
  { /* key presses */
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
      quit_callback();
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
      quit_callback();
  }

  { /* when the left mouse button is held move toward the cursor unless it's
       so close that we'd overshoot */
    int x, y, dx, dy;
    if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
      y = engine->scaled_window_height - y; // so y increases upward
      const int renderer_scale = engine->renderer_scale;
      dx = x - (bug->x * renderer_scale);
      dy = y - (bug->y * renderer_scale);
      Direction direction = NO_DIRECTION;
      if (dx > 0 && dx > BUG_SPEED)
        direction = RIGHT;
      if (dy < 0 && dy < -BUG_SPEED)
        direction = DOWN;
      if (dx < 0 && dx < -BUG_SPEED)
        direction = LEFT;
      if (dy > 0 && dy > BUG_SPEED)
        direction = UP;
      bug_move(bug, direction, window_width, window_height);
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
