#include "bug.h"
#include "direction.h"
#include "logging.h"
#include <stdlib.h>
#include <string.h>

static const int BUG_SIZE = 32;
static const int BUG_SPEED = 2;

static const Bug initialBug = {.x = 0,
                               .y = 0,
                               .w = BUG_SIZE,
                               .h = BUG_SIZE,
                               .size = BUG_SIZE,
                               .speed = BUG_SPEED,
                               .face = RIGHT};

Bug *bug_create(int x, int y) {
  Bug *bug = (Bug *)malloc(sizeof(Bug));
  if (!bug) {
    WARN("%s:%d: Allocating Bug failed", __FILE__, __LINE__);
    return NULL;
  }
  memcpy(bug, &initialBug, sizeof(Bug));
  bug->x = x;
  bug->y = y;
  return bug;
}

void bug_destroy(Bug *bug) { free(bug); }

void bug_move(Bug *bug, Direction direction, int world_width,
              int world_height) {
  switch (direction) {
  case NO_DIRECTION:
    break;
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
  if (bug->x + half_w > world_width)
    bug->x = world_width - half_w;
  if (bug->y - half_h < 0)
    bug->y = 0 + half_h;
  if (bug->y + half_h > world_height)
    bug->y = world_height - half_h;
}
