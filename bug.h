#ifndef BUG_H
#define BUG_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "logging.h"
#include "types.h"

#define BUG_IMAGE_ASSET "assets/bug.png"
#define BUG_INIT_X WIDTH / 2
#define BUG_INIT_Y HEIGHT / 2
#define BUG_SCALE 5
#define BUG_SIZE 32 * BUG_SCALE
#define BUG_SPEED 10

typedef struct {
  SDL_Texture *texture;
  int x, y;
  int w, h;
  direction face;
  char unused[4];
} Bug;

Bug *BugCreate(int x, int y, int w, int h, SDL_Renderer *renderer);
void BugDestroy(Bug *bug);

void BugMove(Bug *bug, direction direction, int width, int height);

#endif
