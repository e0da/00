#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>

#include "types.h"

bool EngineInit(SDL_Window **window, SDL_Renderer **renderer,
                SDL_GameController **controller, const int width,
                const int height, const int scale, const char *title);

void EngineQuit(SDL_Window *window, SDL_Renderer *renderer,
                SDL_GameController *controller);

#endif
