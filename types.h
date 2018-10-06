#ifndef TYPES_H
#define TYPES_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "types.h"

typedef Uint32 Tick;

/* The order is RIGHT, DOWN, LEFT, UP so that
  1. it's clockwise
  2. RIGHT is 0, so facing right is the default like god intended */
typedef enum { RIGHT, DOWN, LEFT, UP } Direction;

typedef struct {
  SDL_Texture *texture;
  int x, y;
  int w, h;
  Direction face;
} Bug;

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_GameController *controller;
  const int window_width;
  const int window_height;
  const int renderer_scale;
  const int scaled_window_width;
  const int scaled_window_height;
  const char *window_title;
} Engine;

typedef struct {
  Tick tick;
  Engine *engine;
  Bug *bug;
} State;

#endif
