#ifndef DRAWING_H
#define DRAWING_H

#include "bug.h"
#include "state.h"
#include <SDL2/SDL.h>

void draw(State *state);
void draw_background(SDL_Renderer *renderer);
void draw_bug(SDL_Renderer *renderer, Bug *bug, int window_height);

#endif
