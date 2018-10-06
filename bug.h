#ifndef BUG_H
#define BUG_H

#include "direction.h"
#include <SDL2/SDL.h>

extern const int BUG_SIZE;
extern const int BUG_SPEED;

typedef struct Bug {
  SDL_Texture *texture;
  int x, y;
  int w, h;
  Direction face;
} Bug;

Bug *bug_create(int x, int y, int w, int h, SDL_Renderer *renderer);
void bug_destroy(Bug *bug);

void bug_move(Bug *bug, Direction direction, int width, int height);

#endif
