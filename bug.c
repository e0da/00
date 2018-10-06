#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "bug.h"
#include "logging.h"
#include "types.h"

static const int bug_scale = 5;
static const int bug_image_width = 32;

const int BUG_SIZE = bug_image_width * bug_scale;
const int BUG_SPEED = 10;

Bug *BugCreate(int x, int y, int w, int h, SDL_Renderer *renderer) {
  Bug *bug = (Bug *)malloc(sizeof(Bug));
  if (!bug) {
    WARN("%s:%d: Allocating Bug failed", __FILE__, __LINE__);
    return NULL;
  }
  static const char *bug_image_asset = "assets/bug.png";
  SDL_Surface *surface = IMG_Load(bug_image_asset);
  if (!surface) {
    WARN("%s:%d: IMG_Load failed -- IMG_Error: %s", __FILE__, __LINE__,
         IMG_GetError());
    return NULL;
  }
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    WARN("%s:%d: SDL_CreateTextureFromSurface failed -- SDL_Error: %s",
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

void BugMove(Bug *bug, Direction direction, int width, int height) {
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
  const int half_w = bug->w / 2;
  const int half_h = bug->h / 2;
  if (bug->x - half_w < 0)
    bug->x = 0 + half_w;
  if (bug->x + half_w > width)
    bug->x = width - half_w;
  if (bug->y - half_h < 0)
    bug->y = 0 + half_h;
  if (bug->y + half_h > height)
    bug->y = height - half_h;
}
