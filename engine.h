#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>

#include "types.h"

// TODO Create Engine struct to hole all this stuff

bool engine_init(SDL_Window **window, SDL_Renderer **renderer,
                 SDL_GameController **controller, const int width,
                 const int height, const int scale, const char *title);

void engine_quit(SDL_Window *window, SDL_Renderer *renderer,
                 SDL_GameController *controller);

#endif
