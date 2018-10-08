#ifndef BUG_H
#define BUG_H

#include "direction.h"
#include <SDL2/SDL.h>

typedef struct Bug {
  int x, y;
  int w, h;
  int size;
  int speed;
  Direction face;
  SDL_Texture *texture;
} Bug;

Bug *bug_create(int x, int y, SDL_Renderer *renderer);
void bug_destroy(Bug *bug);

void bug_move(Bug *bug, Direction direction, int width, int height);

#endif
