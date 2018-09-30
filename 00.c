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

#ifdef __EMSCRIPTEN__
#define USE_REQUEST_ANIMATION_FRAME 0
#define SIMULATE_INFINITE_LOOP 1
#endif

static SDL_Window *window;
static SDL_Renderer *renderer;

void init(void);
bool iterate(void); // returns true if this is the last iteration
void quit(void);

#ifdef __EMSCRIPTEN__
void emscripten_iterate(void);
#endif

bool handle_events(void); // returns true if a quit event is received
void draw_background(void);

void warn_if_sdl_error(const char *warning, bool condition);

int main() {
  init();

#ifndef __EMSCRIPTEN__
  while (true) {
    if (iterate())
      break;
  }
#else
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

  SDL_GL_SetSwapInterval(VSYNC);
}

bool iterate() {
  const bool quit = handle_events();
  draw_background();
  SDL_RenderPresent(renderer);
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

void draw_background() {
  SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0xff, 0xff); // cyan
  SDL_RenderClear(renderer);
}

void warn_if_sdl_error(const char *warning, bool condition) {
  if (!condition) {
    printf("WARN: %s\nSDL_Error: %s\n", warning, SDL_GetError());
  }
}
