#include "bug.h"
#include "drawing.h"
#include "engine.h"
#include "logging.h"
#include "state.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

#define WINDOW_HEIGHT 240
#define ASPECT_RATIO_X 16
#define ASPECT_RATIO_Y 9
#define WINDOW_WIDTH ((WINDOW_HEIGHT * ASPECT_RATIO_X) / ASPECT_RATIO_Y)

// TODO I don't like how calculating RENDERER_SCALE works
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
static const int RENDERER_SCALE = 3;
#else
static const int RENDERER_SCALE = 5;
#endif

static const char *WINDOW_TITLE = "00: o hai windoe";
static const char *BUG_IMAGE_ASSET = "assets/bug.png";

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
      create_engine(WINDOW_WIDTH, WINDOW_HEIGHT, RENDERER_SCALE, WINDOW_TITLE);
  if (!engine) {
    WARN("%s:%d: create_engine failed", __FILE__, __LINE__);
    return false;
  }
  SDL_Texture *bug_texture =
      create_texture_from_file(engine->renderer, BUG_IMAGE_ASSET);
  if (!bug_texture) {
    WARN("%s:%d: create_texture_from_file failed", __FILE__, __LINE__);
    return NULL;
  }
  Bug *bug = create_bug(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
  if (!bug) {
    WARN("%s:%d: create_bug failed", __FILE__, __LINE__);
    return false;
  }
  State initialState = {
      .tick = 0, .engine = engine, .bug = bug, .bug_texture = bug_texture};
  *state = create_state(&initialState);
  if (!*state) {
    WARN("%s:%d: state_create failed", __FILE__, __LINE__);
    return false;
  }
  return true;
}

void iterate(State *state) {
  state->tick++;
  handle_events(state, quit);
  draw(state);
}

void quit(State *state) {
  if (!state) {
    WARN("%s:%d: state is NULL while quitting. WEIRD!", __FILE__, __LINE__);
    return;
  }
  if (state->bug)
    destroy_bug(state->bug);
  state->bug = NULL;
  if (state->bug_texture)
    SDL_DestroyTexture(state->bug_texture);
  state->bug_texture = NULL;
  if (state->engine)
    destroy_engine(state->engine);
  state->engine = NULL;
  destroy_state(state);
  state = NULL;
#ifdef __EMSCRIPTEN__
  emscripten_force_exit(0);
#else
  exit(0);
#endif
}
