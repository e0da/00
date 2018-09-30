#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define WINDOW_TITLE "00: o hai windoe"
#define WIDTH 1024
#define HEIGHT 768
#define VSYNC 1
#define WINDOW_FLAGS SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
#define RENDERER_FLAGS SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
#define MAX_DELTA_TIME 1000
#define BUG_SCALE 10
#define BUG_SIZE 32 * BUG_SCALE
#define BUG_SPEED 0.15L

#ifdef __EMSCRIPTEN__
#define USE_REQUEST_ANIMATION_FRAME 0
#define SIMULATE_INFINITE_LOOP 1
#endif

typedef enum { LEFT = -1, RIGHT = 1 } Direction;

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *bugTexture;
static SDL_Rect bugRect;
static Direction facing;
static long double deltaTime;
static long double thisFrame;
static long double previousFrame;
static Uint64 frequency;

void init(void);
bool iterate(void); // returns true if this is the last iteration
void quit(void);

#ifdef __EMSCRIPTEN__
void emscripten_iterate(void);
#endif

void update_timing(void);
bool handle_events(void); // returns true if a quit event is received
void update_bug(void);
void draw_background(void);
void draw_bug(void);

void warn_if_sdl_error(const char *warning, bool condition);

int main() {
  init();

#ifndef __EMSCRIPTEN__
  while (true) {
    if (iterate())
      break;
  }
#else
  // TODO How do you break out of the emscripten main loop?
  emscripten_set_main_loop(emscripten_iterate, USE_REQUEST_ANIMATION_FRAME,
                           SIMULATE_INFINITE_LOOP);
#endif

  quit();

  return 0;
}

void init() {
  const bool init = !SDL_Init(SDL_INIT_VIDEO); // SDL_Init returns 0 on success
  warn_if_sdl_error("SDL_Init failed", init);

  window =
      SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, WINDOW_FLAGS);
  warn_if_sdl_error("SDL_CreateWindow failed", window);

  renderer = SDL_CreateRenderer(window, -1, RENDERER_FLAGS);
  warn_if_sdl_error("SDL_CreateRenderer failed", renderer);

#ifndef __EMSCRIPTEN__
  SDL_GL_SetSwapInterval(VSYNC);
#endif

  SDL_Surface *bugTextureSurface = IMG_Load("assets/bug.png");
  bugTexture = SDL_CreateTextureFromSurface(renderer, bugTextureSurface);
  SDL_FreeSurface(bugTextureSurface);

  bugRect.w = BUG_SIZE;
  bugRect.h = BUG_SIZE;
  bugRect.x = WIDTH / 2;
  bugRect.y = HEIGHT / 2;

  frequency = SDL_GetPerformanceFrequency();

  facing = RIGHT;
  deltaTime = 0;
  thisFrame = 0;
  previousFrame = 0;
}

bool iterate() {
  // update_timing();
  const bool quit = handle_events();
  // update_bug();
  // draw_background();
  // draw_bug();
  // SDL_RenderPresent(renderer);
  return quit;
}

#ifdef __EMSCRIPTEN__
/* wrap the iterator  because it must return void, but ours does not */
void emscripten_iterate(void) { iterate(); }
#endif

void quit() {
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void update_timing() {
  previousFrame = thisFrame;
  thisFrame = SDL_GetPerformanceCounter();
  deltaTime = ((thisFrame - previousFrame) * 1000) / frequency;
  deltaTime = deltaTime > MAX_DELTA_TIME ? MAX_DELTA_TIME : deltaTime;
}

bool handle_events() {
  SDL_Event event;
  SDL_PollEvent(&event);
  switch (event.type) {
  case SDL_QUIT:
    return true;
  default:
    break;
  }
  return false;
}

void update_bug() {
  if (bugRect.x < 0)
    facing = RIGHT;
  if (bugRect.x > WIDTH)
    facing = LEFT;
  bugRect.x += BUG_SPEED * facing * deltaTime;
}

void draw_background() {
  SDL_SetRenderDrawColor(renderer, 116, 163, 97, 0xff); // green
  SDL_RenderClear(renderer);
}

void draw_bug(void) {
  SDL_Rect drawRect;
  drawRect.x = bugRect.x - (BUG_SIZE / 2);
  drawRect.y = bugRect.y - (BUG_SIZE / 2);
  drawRect.w = bugRect.w;
  drawRect.h = bugRect.h;
  Uint32 flags = 0;
  if (facing == LEFT)
    flags = SDL_FLIP_HORIZONTAL;
  SDL_RenderCopyEx(renderer, bugTexture, NULL, &drawRect, 0, NULL, flags);
}

void warn_if_sdl_error(const char *warning, bool condition) {
  if (!condition) {
    printf("WARN: %s\nSDL_Error: %s\n", warning, SDL_GetError());
  }
}
