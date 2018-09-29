#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>

#define WINDOW_TITLE "00: o hai windoe"
#define WIDTH 1024
#define HEIGHT 768

void init(SDL_Window **window, SDL_Surface **surface, bool *running);
void iterate(bool *running, SDL_Surface *surface, SDL_Window *window);
void quit(SDL_Window **window);

void handle_events(bool *running);
void draw_background(SDL_Surface *surface);

void warn_if_sdl_error(const char *warning, bool condition);

int main() {
  SDL_Window *window;
  SDL_Surface *surface;
  bool running = false;

  init(&window, &surface, &running);
  while (running) {
    iterate(&running, surface, window);
  }
  quit(&window);

  return 0;
}

void init(SDL_Window **window, SDL_Surface **surface, bool *running) {
  bool init = !SDL_Init(SDL_INIT_VIDEO); // SDL_Init returns 0 on success
  warn_if_sdl_error("SDL_Init failed", init);

  *window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT,
                             SDL_WINDOW_SHOWN);
  warn_if_sdl_error("SDL_CreateWindow failed", *window);

  *surface = SDL_GetWindowSurface(*window);
  warn_if_sdl_error("SDL_GetWindowSurface failed", *surface);

  if (init && *window && *surface) {
    *running = true;
  }
}

void iterate(bool *running, SDL_Surface *surface, SDL_Window *window) {
  handle_events(running);
  draw_background(surface);
  SDL_UpdateWindowSurface(window);
}

void quit(SDL_Window **window) {
  SDL_DestroyWindow(*window);
  SDL_Quit();
}

void handle_events(bool *running) {
  SDL_Event event;
  SDL_PollEvent(&event);
  switch (event.type) {
  case SDL_QUIT:
    *running = false;
    break;
  default:
    break;
  }
}

void draw_background(SDL_Surface *surface) {
  const unsigned int cyan = SDL_MapRGB(surface->format, 0x00, 0xff, 0xff);
  SDL_FillRect(surface, NULL, cyan);
}

void warn_if_sdl_error(const char *warning, bool condition) {
  if (!condition) {
    printf("WARN: %s\nSDL_Error: %s\n", warning, SDL_GetError());
  }
}
