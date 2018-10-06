#include <SDL2/SDL.h>
#include <stdbool.h>

#include "bug.h"
#include "engine.h"
#include "logging.h"
#include "types.h"

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
static const int SCALED_WINDOW_WIDTH = WINDOW_WIDTH * RENDERER_SCALE;
static const int SCALED_WINDOW_HEIGHT = WINDOW_HEIGHT * RENDERER_SCALE;

bool init(void);
void iterate(void);
void quit(void);

void update(void);

void draw(void);
void draw_background(void);
void draw_bug(void);

// Static because iterate must be (void (*f)(void)) for Emscripten
static State *state;

int main() {
#ifdef __EMSCRIPTEN__
  static const int use_request_animation_frame = 0;
  static const int simulate_infinite_loop = 1;
  emscripten_set_main_loop(iterate, use_request_animation_frame,
                           simulate_infinite_loop);
#else
  while (true) {
    iterate();
    if (state->quitting)
      break;
  }
#endif
  quit();
  return 0;
}

bool init() {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_GameController *controller;
  if (!EngineInit(&window, &renderer, &controller, SCALED_WINDOW_WIDTH,
                  SCALED_WINDOW_HEIGHT, RENDERER_SCALE, WINDOW_TITLE)) {
    WARN("%s:%d: EngineInit failed", __FILE__, __LINE__);
    return false;
  }
  static const int bug_init_x = WINDOW_WIDTH / 2;
  static const int bug_init_y = WINDOW_HEIGHT / 2;
  Bug *bug = BugCreate(bug_init_x, bug_init_y, BUG_SIZE, BUG_SIZE, renderer);
  if (!bug) {
    WARN("%s:%d: BugCreate failed", __FILE__, __LINE__);
    return false;
  }
  State tmpStore = {.tick = 0,
                    .window = window,
                    .renderer = renderer,
                    .controller = controller,
                    .bug = bug,
                    .quitting = false};
  state = (State *)malloc(sizeof(State));
  if (!state) {
    WARN("%s:%d: Allocating State failed", __FILE__, __LINE__);
    return false;
  }
  memcpy(state, &tmpStore, sizeof(State));
  return true;
}

void iterate() {
  if (!state) {
    if (!init())
      WARN("%s:%d: init failed", __FILE__, __LINE__);
    return;
  }
  update();
  draw();
}

void quit() {
  if (state->bug) {
    BugDestroy(state->bug);
    state->bug = NULL;
  }
  EngineQuit(state->window, state->renderer, state->controller);
  state->window = NULL;
  state->renderer = NULL;
  state->controller = NULL;
}

void update() {
  state->tick++;

  { /* key presses */
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
      state->quitting = true;
    default:
      break;
    }
  }

  { /* held keys */
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_RIGHT])
      BugMove(state->bug, RIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (keys[SDL_SCANCODE_DOWN])
      BugMove(state->bug, DOWN, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (keys[SDL_SCANCODE_LEFT])
      BugMove(state->bug, LEFT, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (keys[SDL_SCANCODE_UP])
      BugMove(state->bug, UP, WINDOW_WIDTH, WINDOW_HEIGHT);
  }

  { /* when the left mouse button is held move toward the cursor unless it's so
       close that we'd overshoot */
    int x, y, dx, dy;
    if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
      y = SCALED_WINDOW_HEIGHT - y; // so y increases upward
      dx = x - (state->bug->x * RENDERER_SCALE);
      dy = y - (state->bug->y * RENDERER_SCALE);
      if (dx > 0 && dx > BUG_SPEED)
        BugMove(state->bug, RIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
      if (dy < 0 && dy < -BUG_SPEED)
        BugMove(state->bug, DOWN, WINDOW_WIDTH, WINDOW_HEIGHT);
      if (dx < 0 && dx < -BUG_SPEED)
        BugMove(state->bug, LEFT, WINDOW_WIDTH, WINDOW_HEIGHT);
      if (dy > 0 && dy > BUG_SPEED)
        BugMove(state->bug, UP, WINDOW_WIDTH, WINDOW_HEIGHT);
    }
  }

  /* game controller */
  if (state->controller) {
    if (SDL_GameControllerGetButton(state->controller,
                                    SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
      BugMove(state->bug, RIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (SDL_GameControllerGetButton(state->controller,
                                    SDL_CONTROLLER_BUTTON_DPAD_DOWN))
      BugMove(state->bug, DOWN, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (SDL_GameControllerGetButton(state->controller,
                                    SDL_CONTROLLER_BUTTON_DPAD_LEFT))
      BugMove(state->bug, LEFT, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (SDL_GameControllerGetButton(state->controller,
                                    SDL_CONTROLLER_BUTTON_DPAD_UP))
      BugMove(state->bug, UP, WINDOW_WIDTH, WINDOW_HEIGHT);
  }
}

void draw() {
  draw_background();
  draw_bug();
  SDL_RenderPresent(state->renderer);
}

void draw_background() {
  SDL_SetRenderDrawColor(state->renderer, 164, 206, 86, 255);
  SDL_RenderClear(state->renderer);
}

void draw_bug() {
  const int offset = -BUG_SIZE / 2;
  SDL_Rect dst = {.w = BUG_SIZE,
                  .h = BUG_SIZE,
                  .x = state->bug->x + offset,
                  .y = (WINDOW_HEIGHT - state->bug->y) + offset};
  const SDL_RendererFlip flip =
      state->bug->face == LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
  if (SDL_RenderCopyEx(state->renderer, state->bug->texture, NULL, &dst, 0, 0,
                       flip) < 0) {
    WARN("%s:%d: SDL_RenderCopyEx failed in draw_bug -- SDL_Error: %s",
         __FILE__, __LINE__, SDL_GetError());
  }
}
