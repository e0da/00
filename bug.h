#ifndef BUG_H
#define BUG_H

#include <SDL2/SDL.h>

#include "types.h"

extern const int BUG_SIZE;
extern const int BUG_SPEED;

struct Bug {
  SDL_Texture *texture;
  int x, y;
  int w, h;
  Direction face;
};

Bug *bug_create(int x, int y, int w, int h, SDL_Renderer *renderer);
void bug_destroy(Bug *bug);

void bug_move(Bug *bug, Direction direction, int width, int height);

#endif
