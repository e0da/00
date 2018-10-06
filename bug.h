#ifndef BUG_H
#define BUG_H

#include <SDL2/SDL.h>

#include "types.h"

extern const int BUG_SIZE;
extern const int BUG_SPEED;

typedef struct {
  SDL_Texture *texture;
  int x, y;
  int w, h;
  Direction face;
  char unused[4];
} Bug;

Bug *BugCreate(int x, int y, int w, int h, SDL_Renderer *renderer);
void BugDestroy(Bug *bug);

void BugMove(Bug *bug, Direction direction, int width, int height);

#endif
