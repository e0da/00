#include "bug.h"
#include "direction.h"
#include "drawing.h"
#include "engine.h"
#include "input.h"
#include "logging.h"
#include "state.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

// TODO I don't like how calculating RENDERER_SCALE works
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
static const int RENDERER_SCALE = 3;
#else
static const int RENDERER_SCALE = 5;
#endif

static const char *WINDOW_TITLE = "00: o hai windoe";
#define WINDOW_HEIGHT 240
#define WINDOW_WIDTH ((WINDOW_HEIGHT * 16) / 9)

bool init(State **state);
void iterate(State *state);
void quit(State *state);

int main() {
  State *state;
  init(&state);
#ifdef __EMSCRIPTEN__
  static const int use_request_animation_frame = 0;
  static const int simulate_infinite_loop = 1;
  emscripten_set_main_loop_arg((void (*)(void *))iterate, state,
                               use_request_animation_frame,
                               simulate_infinite_loop);
#else
  while (true)
    iterate(state);
#endif
}

bool init(State **state) {
  Engine *engine =
      engine_create(WINDOW_WIDTH, WINDOW_HEIGHT, RENDERER_SCALE, WINDOW_TITLE);
  if (!engine) {
    WARN("%s:%d: engine_create failed", __FILE__, __LINE__);
    return false;
  }
  static const int bug_init_x = WINDOW_WIDTH / 2;
  static const int bug_init_y = WINDOW_HEIGHT / 2;
  Bug *bug = bug_create(bug_init_x, bug_init_y, engine->renderer);
  if (!bug) {
    WARN("%s:%d: bug_create failed", __FILE__, __LINE__);
    return false;
  }
  State initialState = {.tick = 0, .engine = engine, .bug = bug};
  *state = state_create(&initialState);
  if (!*state) {
    WARN("%s:%d: state_create failed", __FILE__, __LINE__);
    return false;
  }
  return true;
}

void iterate(State *state) {
  state->tick++;
  input_handle_events(state, quit);
  draw(state);
}

void quit(State *state) {
  if (!state) {
    WARN("%s:%d: state is NULL while quitting. WEIRD!", __FILE__, __LINE__);
    return;
  }
  if (state->bug)
    bug_destroy(state->bug);
  state->bug = NULL;
  if (state->engine)
    engine_destroy(state->engine);
  state->engine = NULL;
  state_destroy(state);
  state = NULL;
#ifdef __EMSCRIPTEN__
  emscripten_force_exit(0);
#else
  exit(0);
#endif
}
