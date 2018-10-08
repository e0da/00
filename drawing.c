#include "drawing.h"
#include "logging.h"
#include "state.h"
#include <SDL2/SDL.h>

static const uint32_t GREEN = 0xa4ce56ff;

static uint8_t r(uint32_t color) { return (color & 0xff000000) >> 24; }
static uint8_t g(uint32_t color) { return (color & 0x00ff0000) >> 16; }
static uint8_t b(uint32_t color) { return (color & 0x0000ff00) >> 8; }
static uint8_t a(uint32_t color) { return (color & 0x000000ff); }

static void draw_background(SDL_Renderer *renderer);
static void draw_bug(SDL_Renderer *renderer, Bug *bug, SDL_Texture *bug_texture,
                     int window_height);

void draw(State *state) {
  SDL_Renderer *renderer = state->engine->renderer;
  draw_background(renderer);
  draw_bug(renderer, state->bug, state->bug_texture,
           state->engine->window_height);
  SDL_RenderPresent(renderer);
}

static void draw_background(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, r(GREEN), g(GREEN), b(GREEN), a(GREEN));
  SDL_RenderClear(renderer);
}

static void draw_bug(SDL_Renderer *renderer, Bug *bug, SDL_Texture *bug_texture,
                     int window_height) {
  const int offset = -bug->w / 2;
  SDL_Rect dst = {.w = bug->w,
                  .h = bug->h,
                  .x = bug->x + offset,
                  .y = (window_height - bug->y) + offset};
  const SDL_RendererFlip flip =
      bug->face == LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
  if (SDL_RenderCopyEx(renderer, bug_texture, NULL, &dst, 0, 0, flip) < 0) {
    WARN("%s:%d: SDL_RenderCopyEx failed -- SDL_Error: %s", __FILE__, __LINE__,
         SDL_GetError());
  }
}
