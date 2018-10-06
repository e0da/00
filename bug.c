#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "bug.h"
#include "logging.h"
#include "types.h"

Bug *BugCreate(int x, int y, int w, int h, SDL_Renderer *renderer) {
  Bug *bug = (Bug *)malloc(sizeof(Bug));
  if (!bug) {
    WARN("%s:%d: Allocating Bug failed", __FILE__, __LINE__);
    return NULL;
  }
  SDL_Surface *surface = IMG_Load(BUG_IMAGE_ASSET);
  if (!surface) {
    WARN("%s:%d: IMG_Load failed in BugCreate -- IMG_Error: %s", __FILE__,
         __LINE__, IMG_GetError());
    return NULL;
  }
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    WARN("%s:%d: SDL_CreateTextureFromSurface failed in BugCreate -- "
         "SDL_Error: %s",
         __FILE__, __LINE__, SDL_GetError());
    return NULL;
  }
  SDL_FreeSurface(surface);
  bug->x = x;
  bug->y = y;
  bug->w = w;
  bug->h = h;
  bug->face = RIGHT;
  bug->texture = texture;
  return bug;
}

void BugDestroy(Bug *bug) {
  SDL_DestroyTexture(bug->texture);
  free(bug);
}

void BugMove(Bug *bug, direction direction, int width, int height) {
  switch (direction) {
  case RIGHT:
    bug->face = RIGHT;
    bug->x += BUG_SPEED;
    break;
  case DOWN:
    bug->y -= BUG_SPEED;
    break;
  case LEFT:
    bug->face = LEFT;
    bug->x -= BUG_SPEED;
    break;
  case UP:
    bug->y += BUG_SPEED;
    break;
  }
  int half_w = bug->w / 2;
  int half_h = bug->h / 2;
  if (bug->x - half_w < 0)
    bug->x = 0 + half_w;
  if (bug->x + half_w > width)
    bug->x = width - half_w;
  if (bug->y - half_h < 0)
    bug->y = 0 + half_h;
  if (bug->y + half_h > height)
    bug->y = height - half_h;
}
