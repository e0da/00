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

static Tick tick;
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_GameController *controller;
static Bug *bug;
static bool quitting;
static bool initialized;

bool init(void);
void iterate(void);
void quit(void);

void init_game_state(void);
bool init_bug(void);

bool update(void); // returns false if a quit event is received

void draw(void);
void draw_background(void);
void draw_bug(void);

int main() {
  initialized = false;
#ifdef __EMSCRIPTEN__
  static const int use_request_animation_frame = 0;
  static const int simulate_infinite_loop = 1;
  emscripten_set_main_loop(iterate, use_request_animation_frame,
                           simulate_infinite_loop);
#else
  while (true) {
    iterate();
    if (quitting)
      break;
  }
#endif
  quit();
  return 0;
}

bool init() {
  init_game_state();
  if (!EngineInit(&window, &renderer, &controller, SCALED_WINDOW_WIDTH, SCALED_WINDOW_HEIGHT,
                  RENDERER_SCALE, WINDOW_TITLE)) {
    WARN("%s:%d: EngineInit failed in init", __FILE__, __LINE__);
    return false;
  }
  if (!init_bug()) {
    WARN("%s:%d: init_bug failed in init", __FILE__, __LINE__);
    return false;
  }
  return true;
}

void iterate() {
  quitting = !update();
  draw();
}

void quit() {
  if (bug) {
    BugDestroy(bug);
    bug = NULL;
  }
  EngineQuit(window, renderer, controller);
  window = NULL;
  renderer = NULL;
  controller = NULL;
}

void init_game_state() {
  quitting = false;
  tick = 0;
}

bool init_bug() {
  static const int bug_init_x = WINDOW_WIDTH / 2;
  static const int bug_init_y = WINDOW_HEIGHT / 2;
  bug = BugCreate(bug_init_x, bug_init_y, BUG_SIZE, BUG_SIZE, renderer);
  if (!bug) {
    WARN("%s:%d: BugCreate failed in init_bug", __FILE__, __LINE__);
    return false;
  }
  return true;
}

bool update() {
  if (!initialized) {
    initialized = init();
    if (!initialized) {
      WARN("%s:%d: init failed in update", __FILE__, __LINE__);
      return false;
    }
  }

  tick++;

  { /* key presses */
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
      return false;
    default:
      break;
    }
  }

  { /* held keys */
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_RIGHT])
      BugMove(bug, RIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (keys[SDL_SCANCODE_DOWN])
      BugMove(bug, DOWN, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (keys[SDL_SCANCODE_LEFT])
      BugMove(bug, LEFT, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (keys[SDL_SCANCODE_UP])
      BugMove(bug, UP, WINDOW_WIDTH, WINDOW_HEIGHT);
  }

  { /* when the left mouse button is held move toward the cursor unless it's so
       close that we'd overshoot */
    int x, y, dx, dy;
    if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
      y = SCALED_WINDOW_HEIGHT - y; // so y increases upward
      dx = x - (bug->x * RENDERER_SCALE);
      dy = y - (bug->y * RENDERER_SCALE);
      if (dx > 0 && dx > BUG_SPEED)
        BugMove(bug, RIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
      if (dy < 0 && dy < -BUG_SPEED)
        BugMove(bug, DOWN, WINDOW_WIDTH, WINDOW_HEIGHT);
      if (dx < 0 && dx < -BUG_SPEED)
        BugMove(bug, LEFT, WINDOW_WIDTH, WINDOW_HEIGHT);
      if (dy > 0 && dy > BUG_SPEED)
        BugMove(bug, UP, WINDOW_WIDTH, WINDOW_HEIGHT);
    }
  }

  /* game controller */
  if (controller) {
    if (SDL_GameControllerGetButton(controller,
                                    SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
      BugMove(bug, RIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (SDL_GameControllerGetButton(controller,
                                    SDL_CONTROLLER_BUTTON_DPAD_DOWN))
      BugMove(bug, DOWN, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (SDL_GameControllerGetButton(controller,
                                    SDL_CONTROLLER_BUTTON_DPAD_LEFT))
      BugMove(bug, LEFT, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP))
      BugMove(bug, UP, WINDOW_WIDTH, WINDOW_HEIGHT);
  }

  return true;
}

void draw() {
  draw_background();
  draw_bug();
  SDL_RenderPresent(renderer);
}

void draw_background() {
  SDL_SetRenderDrawColor(renderer, 164, 206, 86, 255);
  SDL_RenderClear(renderer);
}

void draw_bug() {
  const int offset = -BUG_SIZE / 2;
  SDL_Rect dst = {.w = BUG_SIZE,
                  .h = BUG_SIZE,
                  .x = bug->x + offset,
                  .y = (WINDOW_HEIGHT - bug->y) + offset};
  const SDL_RendererFlip flip =
      bug->face == LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
  if (SDL_RenderCopyEx(renderer, bug->texture, NULL, &dst, 0, 0, flip) < 0) {
    WARN("%s:%d: SDL_RenderCopyEx failed in draw_bug -- SDL_Error: %s",
         __FILE__, __LINE__, SDL_GetError());
  }
}
