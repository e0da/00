#include <SDL2/SDL.h>
#include <stdio.h>

#define WINDOW_TITLE "00: o hai windoe"
#define WIDTH 1024
#define HEIGHT 768

enum errorCodes {
  ERROR_SDL_INIT,
  ERROR_SDL_CREATE_WINDOW,
  ERROR_SDL_GET_WINDOW_SURFACE
};

void printSDLError(const char *error);

int main() {

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printSDLError("SDL_Init");
    return ERROR_SDL_INIT;
  }

  SDL_Window *window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT,
                                        SDL_WINDOW_SHOWN);
  if (!window) {
    printSDLError("SDL_CreateWindow");
    return ERROR_SDL_CREATE_WINDOW;
  }

  SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
  if (!screenSurface) {
    printSDLError("SDL_GetWindowSurface");
    return ERROR_SDL_GET_WINDOW_SURFACE;
  }

  const unsigned int cyan = SDL_MapRGB(screenSurface->format, 0x00, 0xff, 0xff);
  SDL_FillRect(screenSurface, NULL, cyan);
  SDL_UpdateWindowSurface(window);
  SDL_Delay(2000);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

void printSDLError(const char *error) {
  printf("Context: %s\nSDL_Error: %s\n", error, SDL_GetError());
}
