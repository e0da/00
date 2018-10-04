#ifndef ENGINE_H
#define ENGINE_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "logging.h"
#include "types.h"

#define INIT_FLAGS (SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER)
#define WINDOW_FLAGS SDL_WINDOW_OPENGL
#define RENDERER_FLAGS (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
#define VSYNC 1
#define IMAGE_FLAGS IMG_INIT_PNG
#define JOYSTICK_1 0

bool EngineInit(SDL_Window **window, SDL_Renderer **renderer,
                SDL_GameController **controller, const int width,
                const int height, const int scale, const char *title);

void EngineQuit(SDL_Window *window, SDL_Renderer *renderer,
                SDL_GameController *controller);

#endif
