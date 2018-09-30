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
#define FPS 60

static SDL_Window *window;
static SDL_Surface *surface;
static bool running;

void init(void);
void iterate(void);
void quit(void);

void handle_events(void);
void draw_background(void);

void warn_if_sdl_error(const char *warning, bool condition);

int main() {
  init();

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(iterate, FPS, 1);
#else
  while (running) {
    iterate();
    SDL_Delay(1000 / FPS);
  }
#endif

  quit();

  return 0;
}

void init() {
  bool init = !SDL_Init(SDL_INIT_VIDEO); // SDL_Init returns 0 on success
  warn_if_sdl_error("SDL_Init failed", init);

  window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT,
                            SDL_WINDOW_SHOWN);
  warn_if_sdl_error("SDL_CreateWindow failed", window);

  surface = SDL_GetWindowSurface(window);
  warn_if_sdl_error("SDL_GetWindowSurface failed", surface);

  running = init && window && surface;
}

void iterate() {
  handle_events();
  draw_background();
  SDL_UpdateWindowSurface(window);
}

void quit() {
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void handle_events() {
  SDL_Event event;
  SDL_PollEvent(&event);
  switch (event.type) {
  case SDL_QUIT:
    running = false;
    break;
  default:
    break;
  }
}

void draw_background() {
  const unsigned int cyan = SDL_MapRGB(surface->format, 0x00, 0xff, 0xff);
  SDL_FillRect(surface, NULL, cyan);
}

void warn_if_sdl_error(const char *warning, bool condition) {
  if (!condition) {
    printf("WARN: %s\nSDL_Error: %s\n", warning, SDL_GetError());
  }
}
