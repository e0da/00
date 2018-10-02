#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

// TODO I don't like how calculating RENDERER_SCALE works
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define RENDERER_SCALE 1
#else
#define RENDERER_SCALE 2
#endif

#define WINDOW_TITLE "00: o hai windoe"
#define WIDTH 1024
#define HEIGHT 768
#define WINDOW_WIDTH (WIDTH * RENDERER_SCALE)
#define WINDOW_HEIGHT (HEIGHT * RENDERER_SCALE)
#define WINDOW_FLAGS                                                           \
  (SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)
#define RENDERER_FLAGS (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
#define VSYNC 1

#define BUG_IMAGE_ASSET "assets/bug.png"
#define BUG_INIT_X WIDTH / 2
#define BUG_INIT_Y HEIGHT / 2
#define BUG_SCALE 5
#define BUG_SIZE 32 * BUG_SCALE
#define BUG_SPEED 10

#ifdef __EMSCRIPTEN__
#define USE_REQUEST_ANIMATION_FRAME 0
#define SIMULATE_INFINITE_LOOP 1
#define warn(...) emscripten_log(EM_LOG_WARN, __VA_ARGS__)
#endif

typedef Uint32 TICK;

typedef enum { LEFT, RIGHT } direction;

typedef SDL_Point point;

typedef struct {
  SDL_Texture *texture;
  int x, y;
  int w, h;
  direction face;
  char unused[4];
} Bug;

Bug *CreateBug(int x, int y, int w, int h, SDL_Renderer *renderer);
void DestroyBug(Bug *bug);
point BugPosition(TICK tick);

Bug *CreateBug(int x, int y, int w, int h, SDL_Renderer *renderer) {
  Bug *bug = (Bug *)malloc(sizeof(Bug));
  if (!bug) {
    warn("%s:%d: Allocating Bug failed", __FILE__, __LINE__);
    return NULL;
  }
  SDL_Surface *surface = IMG_Load(BUG_IMAGE_ASSET);
  if (!surface) {
    warn("%s:%d: IMG_Load failed in CreateBug -- IMG_Error: %s", __FILE__,
         __LINE__, IMG_GetError());
    return NULL;
  }
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    warn("%s:%d: SDL_CreateTextureFromSurface failed in CreateBug -- "
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

void DestroyBug(Bug *bug) {
  SDL_DestroyTexture(bug->texture);
  free(bug);
}

point BugPosition(TICK tick) {
  /*
   Currently makes him go in a wobbly path.
   This theta determines the rotation speed. Whatever feels good.
  */
  const double theta = (double)(tick) / (BUG_SPEED * 3);
  const int radius = 200; // displacement from origin
  const double xWrinkle = 1.5;
  const double yWrinkle = 0.8;
  point pos = {.x = (WIDTH / 2) + (int)(radius * cos(theta * xWrinkle)),
               .y = (HEIGHT / 2) + (int)(radius * sin(theta * yWrinkle))};
  return pos;
}

static TICK tick;
static SDL_Window *window;
static SDL_Renderer *renderer;
static Bug *bug;

void init(void);
bool iterate(void); // returns true if this is the last iteration
void quit(void);

#ifdef __EMSCRIPTEN__
void emscripten_iterate(void);
#endif

void init_game_state(void);
void init_window(void);
void init_renderer(void);
void init_bug(void);

bool handle_events(void); // returns true if a quit event is received

void update(void);
void update_game_state(void);
void update_bug(void);

void draw(void);
void draw_background(void);
void draw_bug(void);

int main() {
#ifndef __EMSCRIPTEN__
  init();
  while (true) {
    if (iterate())
      break;
  }
#else
  emscripten_set_main_loop(emscripten_iterate, USE_REQUEST_ANIMATION_FRAME,
                           SIMULATE_INFINITE_LOOP);
#endif
  quit();
  return 0;
}

void init() {
  init_game_state();
  init_window();
  init_renderer();
  init_bug();
}

bool iterate() {
  const bool quit = handle_events();
  update();
  draw();
  return quit;
}

void quit() {
  DestroyBug(bug);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

#ifdef __EMSCRIPTEN__
/*
XXX The requirements are a little different for setting up SDL in
native vs Emscripten.
- The iterator must return void
- Rendering textures from surfaces created with IMG_Load doesn't seem to work
  if I initialize the texture before the first iteration, so we make sure init
  is called after Emscripten sets up its main loop callback.
*/
void emscripten_iterate(void) {
  if (!window)
    init();
  iterate();
}
#endif

void init_game_state() { tick = 0; }

void init_window() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    warn("%s:%d: SDL_Init failed in init -- SDL_Error: %s", __FILE__, __LINE__,
         SDL_GetError());
    return;
  }

  window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                            WINDOW_HEIGHT, WINDOW_FLAGS);
  if (!window) {
    warn("%s:%d: SDL_CreateWindow failed in init -- SDL_Error: %s", __FILE__,
         __LINE__, SDL_GetError());
    return;
  }
}

void init_renderer() {
  renderer = SDL_CreateRenderer(window, -1, RENDERER_FLAGS);
  if (!renderer) {
    warn("%s:%d: SDL_CreateRenderer failed in init -- SDL_Error: %s", __FILE__,
         __LINE__, SDL_GetError());
    return;
  }
  SDL_RenderSetScale(renderer, RENDERER_SCALE, RENDERER_SCALE);
  if (SDL_GL_SetSwapInterval(VSYNC) == -1) {
    warn("%s:%d: Swap interval not supported -- SDL_GetError: %s", __FILE__,
         __LINE__, SDL_GetError());
    return;
  };
}

void init_bug() {
  bug = CreateBug(BUG_INIT_X, BUG_INIT_Y, BUG_SIZE, BUG_SIZE, renderer);
  if (!bug) {
    warn("%s:%d: CreateBug failed in init_bug", __FILE__, __LINE__);
    return;
  }
}

bool handle_events() {
  SDL_Event event;
  SDL_PollEvent(&event);
  switch (event.type) {
  case SDL_QUIT:
    return true;
  default:
    break;
  }
  return false;
}

void update() {
  update_game_state();
  update_bug();
}

void update_game_state() { tick++; }

void update_bug() {
  point pos = BugPosition(tick);
  bug->x = pos.x;
  bug->y = pos.y;
  if (tick % 100 == 0) {
    bug->face = bug->face == RIGHT ? LEFT : RIGHT;
  }
}

void draw() {
  draw_background();
  draw_bug();
  SDL_RenderPresent(renderer);
}

void draw_background() {
  SDL_SetRenderDrawColor(renderer, 164, 206, 86, 255);
  SDL_RenderClear(renderer);
}

void draw_bug() {
  SDL_Rect *dst = (SDL_Rect *)malloc(sizeof(SDL_Rect));
  if (!dst) {
    warn("%s:%d: Allocating SDL_Rect failed in draw_bug", __FILE__, __LINE__);
  }
  dst->w = dst->h = BUG_SIZE;
  const int offset = -BUG_SIZE / 2;
  dst->x = bug->x + offset;
  dst->y = bug->y + offset;
  const SDL_RendererFlip flip =
      bug->face == LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
  if (SDL_RenderCopyEx(renderer, bug->texture, NULL, dst, 0, 0, flip) < 0) {
    warn("%s:%d: SDL_RenderCopyEx failed in draw_bug -- SDL_Error: %s",
         __FILE__, __LINE__, SDL_GetError());
  }
  free(dst);
}
