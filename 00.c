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

void printSDLError(char *error);

int main() {
  SDL_Window *window = NULL;

  SDL_Surface *screenSurface = NULL;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printSDLError("SDL_Init");
    return ERROR_SDL_INIT;
  }

  window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT,
                            SDL_WINDOW_SHOWN);
  if (window == NULL) {
    printSDLError("SDL_CreateWindow");
    return ERROR_SDL_CREATE_WINDOW;
  }

  screenSurface = SDL_GetWindowSurface(window);
  if (screenSurface == NULL) {
    printSDLError("SDL_GetWindowSurface");
    return ERROR_SDL_GET_WINDOW_SURFACE;
  }
  SDL_FillRect(screenSurface, NULL,
               SDL_MapRGB(screenSurface->format, 0xff, 0xff, 0xff));
  SDL_UpdateWindowSurface(window);
  SDL_Delay(2000);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

void printSDLError(char *error) {
  printf("Context: %s\nSDL_Error: %s\n", error, SDL_GetError());
}
