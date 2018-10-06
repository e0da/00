#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>

typedef struct Engine {
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

Engine *engine_create(const int window_width, const int window_height,
                      const int renderer_scale, const char *window_title);
void engine_destroy(Engine *engine);

#endif
