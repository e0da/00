#include "bug.h"
#include "direction.h"
#include "engine.h"
#include "input.h"
#include "logging.h"
#include "state.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

#define R(X) ((X & 0xff000000) >> 24)
#define G(X) ((X & 0x00ff0000) >> 16)
#define B(X) ((X & 0x0000ff00) >> 8)
#define A(X) (X & 0x000000ff)

// TODO I don't like how calculating RENDERER_SCALE works
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
static const int RENDERER_SCALE = 1;
#else
static const int RENDERER_SCALE = 2;
#endif

static const char *WINDOW_TITLE = "00: o hai windoe";
static const int WINDOW_WIDTH = 1024;
static const int WINDOW_HEIGHT = 768;

bool init(State **state);
void iterate(State *state);
void quit(State *state);

void update(State *state);

void draw(State *state);
void draw_background(State *state);
void draw_bug(State *state);

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
  Bug *bug =
      bug_create(bug_init_x, bug_init_y, BUG_SIZE, BUG_SIZE, engine->renderer);
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
  update(state);
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

void update(State *state) {
  state->tick++;
  input_handle_events(state, quit);
}

void draw(State *state) {
  draw_background(state);
  draw_bug(state);
  SDL_RenderPresent(state->engine->renderer);
}

void draw_background(State *state) {
  const uint32_t green = 0xa4ce56ff;
  SDL_SetRenderDrawColor(state->engine->renderer, R(green), G(green), B(green),
                         A(green));
  SDL_RenderClear(state->engine->renderer);
}

void draw_bug(State *state) {
  const int offset = -BUG_SIZE / 2;
  SDL_Rect dst = {.w = BUG_SIZE,
                  .h = BUG_SIZE,
                  .x = state->bug->x + offset,
                  .y = (WINDOW_HEIGHT - state->bug->y) + offset};
  const SDL_RendererFlip flip =
      state->bug->face == LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
  if (SDL_RenderCopyEx(state->engine->renderer, state->bug->texture, NULL, &dst,
                       0, 0, flip) < 0) {
    WARN("%s:%d: SDL_RenderCopyEx failed -- SDL_Error: %s", __FILE__, __LINE__,
         SDL_GetError());
  }
}
