#include <SDL2/SDL.h>
#include <stdio.h>

#define WINDOW_TITLE "00: o hai windoe"
#define WIDTH 1024
#define HEIGHT 768

void warn_unless(const char *warning, void *const ref);

int main() {
  int init = SDL_Init(SDL_INIT_VIDEO);
  warn_unless("SDL_Init failed", &init);

  SDL_Window *const window = SDL_CreateWindow(
      WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH,
      HEIGHT, SDL_WINDOW_SHOWN);
  warn_unless("SDL_CreateWindow failed", window);

  SDL_Surface *const surface = SDL_GetWindowSurface(window);
  warn_unless("SDL_GetWindowSurface failed", surface);

  const unsigned int cyan = SDL_MapRGB(surface->format, 0x00, 0xff, 0xff);
  SDL_FillRect(surface, NULL, cyan);

  SDL_UpdateWindowSurface(window);
  SDL_Delay(2000);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

void warn_unless(const char *warning, void *const ref) {
  if (!ref) {
    printf("WARN: %s\nSDL_Error: %s\n", warning, SDL_GetError());
  }
}
