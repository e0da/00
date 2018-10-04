#include <SDL2/SDL.h>
#include <stdbool.h>

#include "Bug.h"
#include "logging.h"

// TODO I don't like how calculating RENDERER_SCALE works
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define RENDERER_SCALE 1
#else
#define RENDERER_SCALE 2
#endif

#define WINDOW_TITLE "00: o hai windoe"
#define WIDTH 1024
#define HEIGHT 768
#define WINDOW_WIDTH (WIDTH * RENDERER_SCALE)
#define WINDOW_HEIGHT (HEIGHT * RENDERER_SCALE)
#define WINDOW_FLAGS SDL_WINDOW_OPENGL
#define RENDERER_FLAGS (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
#define VSYNC 1

#ifdef __EMSCRIPTEN__
#define USE_REQUEST_ANIMATION_FRAME 0
#define SIMULATE_INFINITE_LOOP 1
#endif

static TICK tick;
static SDL_Window *window;
static SDL_Renderer *renderer;
static Bug *bug;
static bool quitting;
static bool initialized;

bool init(void);
void iterate(void);
void quit(void);

void init_game_state(void);
bool init_window(void);
bool init_renderer(void);
bool init_bug(void);

bool update(void); // returns false if a quit event is received

void draw(void);
void draw_background(void);
void draw_bug(void);

int main() {
  initialized = false;
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(iterate, USE_REQUEST_ANIMATION_FRAME,
                           SIMULATE_INFINITE_LOOP);
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
  if (!init_window()) {
    WARN("%s:%d: init_window failed in init", __FILE__, __LINE__);
    return false;
  }
  if (!init_renderer()) {
    WARN("%s:%d: init_renderer failed in init", __FILE__, __LINE__);
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
  BugDestroy(bug);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void init_game_state() {
  quitting = false;
  tick = 0;
}

bool init_window() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    WARN("%s:%d: SDL_Init failed in init_window -- SDL_Error: %s", __FILE__,
         __LINE__, SDL_GetError());
    return false;
  }

  window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                            WINDOW_HEIGHT, WINDOW_FLAGS);
  if (!window) {
    WARN("%s:%d: SDL_CreateWindow failed in init_window -- SDL_Error: %s",
         __FILE__, __LINE__, SDL_GetError());
    return false;
  }
  return true;
}

bool init_renderer() {
  renderer = SDL_CreateRenderer(window, -1, RENDERER_FLAGS);
  if (!renderer) {
    WARN("%s:%d: SDL_CreateRenderer failed in init_renderer -- SDL_Error: %s",
         __FILE__, __LINE__, SDL_GetError());
    return false;
  }
  SDL_RenderSetScale(renderer, RENDERER_SCALE, RENDERER_SCALE);
  if (SDL_GL_SetSwapInterval(VSYNC) == -1) {
    WARN("%s:%d: SDL_RenderSetScale failed in init_renderer -- Swap interval "
         "not supported -- SDL_GetError: %s",
         __FILE__, __LINE__, SDL_GetError());
    return false;
  };
  return true;
}

bool init_bug() {
  bug = BugCreate(BUG_INIT_X, BUG_INIT_Y, BUG_SIZE, BUG_SIZE, renderer);
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
    if (keys[SDL_SCANCODE_LEFT])
      BugMove(bug, LEFT, WIDTH, HEIGHT);
    if (keys[SDL_SCANCODE_RIGHT])
      BugMove(bug, RIGHT, WIDTH, HEIGHT);
    if (keys[SDL_SCANCODE_UP])
      BugMove(bug, UP, WIDTH, HEIGHT);
    if (keys[SDL_SCANCODE_DOWN])
      BugMove(bug, DOWN, WIDTH, HEIGHT);
  }

  { /* when the left mouse button is held move toward the cursor unless it's so
       close that we'd overshoot */
    int x, y, dx, dy;
    if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
      y = WINDOW_HEIGHT - y; // so y increases upward
      dx = x - (bug->x * RENDERER_SCALE);
      dy = y - (bug->y * RENDERER_SCALE);
      if (dx < 0 && dx < -BUG_SPEED)
        BugMove(bug, LEFT, WIDTH, HEIGHT);
      if (dx > 0 && dx > BUG_SPEED)
        BugMove(bug, RIGHT, WIDTH, HEIGHT);
      if (dy < 0 && dy < -BUG_SPEED)
        BugMove(bug, DOWN, WIDTH, HEIGHT);
      if (dy > 0 && dy > BUG_SPEED)
        BugMove(bug, UP, WIDTH, HEIGHT);
    }
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
                  .y = (HEIGHT - bug->y) + offset};
  const SDL_RendererFlip flip =
      bug->face == LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
  if (SDL_RenderCopyEx(renderer, bug->texture, NULL, &dst, 0, 0, flip) < 0) {
    WARN("%s:%d: SDL_RenderCopyEx failed in draw_bug -- SDL_Error: %s",
         __FILE__, __LINE__, SDL_GetError());
  }
}
