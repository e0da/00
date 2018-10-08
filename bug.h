#ifndef BUG_H
#define BUG_H

#include "direction.h"

typedef struct Bug {
  int x, y;
  int w, h;
  int size;
  int speed;
  Direction face;
} Bug;

Bug *create_bug(int x, int y);
void destroy_bug(Bug *bug);

void move_bug(Bug *bug, Direction direction, int width, int height);

#endif
