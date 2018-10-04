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

void init(void);
void iterate(void);
void quit(void);

void init_game_state(void);
void init_window(void);
void init_renderer(void);
void init_bug(void);

bool update(void); // returns true if a quit event is received

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

void init() {
  init_game_state();
  init_window();
  init_renderer();
  init_bug();
  initialized = true;
}

void iterate() {
  quitting = update();
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

void init_window() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    warn("%s:%d: SDL_Init failed in init -- SDL_Error: %s", __FILE__, __LINE__,
         SDL_GetError());
    return;
  }

  window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                            WINDOW_HEIGHT, WINDOW_FLAGS);
  if (!window) {
    warn("%s:%d: SDL_CreateWindow failed in init -- SDL_Error: %s", __FILE__,
         __LINE__, SDL_GetError());
    return;
  }
}

void init_renderer() {
  renderer = SDL_CreateRenderer(window, -1, RENDERER_FLAGS);
  if (!renderer) {
    warn("%s:%d: SDL_CreateRenderer failed in init -- SDL_Error: %s", __FILE__,
         __LINE__, SDL_GetError());
    return;
  }
  SDL_RenderSetScale(renderer, RENDERER_SCALE, RENDERER_SCALE);
  if (SDL_GL_SetSwapInterval(VSYNC) == -1) {
    warn("%s:%d: Swap interval not supported -- SDL_GetError: %s", __FILE__,
         __LINE__, SDL_GetError());
    return;
  };
}

void init_bug() {
  bug = BugCreate(BUG_INIT_X, BUG_INIT_Y, BUG_SIZE, BUG_SIZE, renderer);
  if (!bug) {
    warn("%s:%d: BugCreate failed in init_bug", __FILE__, __LINE__);
    return;
  }
}

bool update() {
  if (!initialized)
    init();

  tick++;

  SDL_Event event;
  SDL_PollEvent(&event);
  switch (event.type) {
  case SDL_QUIT:
    return true;
  default:
    break;
  }

  const Uint8 *keys = SDL_GetKeyboardState(NULL);
  if (keys[SDL_SCANCODE_LEFT]) {
    BugMove(bug, LEFT, WIDTH, HEIGHT);
  }

  if (keys[SDL_SCANCODE_RIGHT]) {
    BugMove(bug, RIGHT, WIDTH, HEIGHT);
  }

  if (keys[SDL_SCANCODE_UP]) {
    BugMove(bug, UP, WIDTH, HEIGHT);
  }

  if (keys[SDL_SCANCODE_DOWN]) {
    BugMove(bug, DOWN, WIDTH, HEIGHT);
  }

  /* Move toward the cursor unless it's so close that we'd overshoot */
  int x, y, dx, dy;
  if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
    y = WINDOW_HEIGHT - y; // so y increases upward
    dx = x - (bug->x * RENDERER_SCALE);
    dy = y - (bug->y * RENDERER_SCALE);
    if (dx < 0 && dx < -BUG_SPEED)
      BugMove(bug, LEFT, WINDOW_WIDTH, WINDOW_HEIGHT);
    else if (dx > 0 && dx > BUG_SPEED)
      BugMove(bug, RIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (dy < 0 && dy < -BUG_SPEED)
      BugMove(bug, DOWN, WINDOW_WIDTH, WINDOW_HEIGHT);
    else if (dy > 0 && dy > BUG_SPEED)
      BugMove(bug, UP, WINDOW_WIDTH, WINDOW_HEIGHT);
  }

  return false;
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
  SDL_Rect *dst = (SDL_Rect *)malloc(sizeof(SDL_Rect));
  if (!dst) {
    warn("%s:%d: Allocating SDL_Rect failed in draw_bug", __FILE__, __LINE__);
  }
  dst->w = dst->h = BUG_SIZE;
  const int offset = -BUG_SIZE / 2;
  dst->x = bug->x + offset;
  dst->y = (HEIGHT - bug->y) + offset;
  const SDL_RendererFlip flip =
      bug->face == LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
  if (SDL_RenderCopyEx(renderer, bug->texture, NULL, dst, 0, 0, flip) < 0) {
    warn("%s:%d: SDL_RenderCopyEx failed in draw_bug -- SDL_Error: %s",
         __FILE__, __LINE__, SDL_GetError());
  }
  free(dst);
}
