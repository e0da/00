#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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
#define WINDOW_WIDTH (WIDTH) * (RENDERER_SCALE)
#define WINDOW_HEIGHT (HEIGHT) * (RENDERER_SCALE)
#define FPS 60
#define RENDERER_FLAGS SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
#define VSYNC 1

#define BUG_SURFACE_ASSET "assets/bug.png"
#define BUG_INIT_X WIDTH / 2
#define BUG_INIT_Y HEIGHT / 2
#define BUG_SCALE 5
#define BUG_SIZE 32 * BUG_SCALE
#define BUG_SPEED 10

#ifdef __EMSCRIPTEN__
#define USE_REQUEST_ANIMATION_FRAME 0
#define SIMULATE_INFINITE_LOOP 1
#else
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
  if (!bug)
    return NULL;
  SDL_Surface *surface = IMG_Load(BUG_SURFACE_ASSET);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
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
  free(bug->texture);
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

void warn_if_sdl_error(const char *warning, bool condition);

int main() {

// XXX See emscripten_iterate implementation
#ifndef __EMSCRIPTEN__
  init();
  while (true) {
    if (iterate())
      break;
    SDL_Delay(1000 / FPS);
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

void update() {
  update_game_state();
  update_bug();
}

void update_game_state() { tick++; }

void update_bug() {
  point pos = BugPosition(tick);
  bug->x = pos.x;
  bug->y = pos.y;
}

void draw() {
  draw_background();
  draw_bug();
  SDL_RenderPresent(renderer);
}

#ifdef __EMSCRIPTEN__
/*
XXX The requirements are a little different for setting up SDL in
native vs Emscripten.
- The iterator must return void
- Init has to happen in the first iteration
  - The renderer has to be created after emscripten_set_main_loop
  - Rendering textures from surfaces created with IMG_Load doesn't seem
    to work if I initialize the texture before the firts iteration
*/
void emscripten_iterate(void) {
  if (!window)
    init();
  iterate();
}
#endif

void init_game_state() { tick = 0; }

void init_window() {
  const bool init = !SDL_Init(SDL_INIT_VIDEO); // SDL_Init returns 0 on success
  warn_if_sdl_error("SDL_Init failed", init);

  window = SDL_CreateWindow(
      WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      WINDOW_WIDTH, WINDOW_HEIGHT,
      SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  warn_if_sdl_error("SDL_CreateWindow failed", window);
}

void init_renderer() {
  renderer = SDL_CreateRenderer(window, -1, RENDERER_FLAGS);
  warn_if_sdl_error("SDL_CreateRenderer failed", window);
  SDL_RenderSetScale(renderer, RENDERER_SCALE, RENDERER_SCALE);
  SDL_GL_SetSwapInterval(VSYNC);
}

void init_bug() {
  bug = CreateBug(BUG_INIT_X, BUG_INIT_Y, BUG_SIZE, BUG_SIZE, renderer);
}

void quit() {
  DestroyBug(bug);
  SDL_DestroyWindow(window);
  SDL_Quit();
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

void draw_background() {
  SDL_SetRenderDrawColor(renderer, 164, 206, 86, 255);
  SDL_RenderClear(renderer);
}

void draw_bug() {
  SDL_Rect *dst = (SDL_Rect *)malloc(sizeof(SDL_Rect));
  dst->w = dst->h = BUG_SIZE;
  const int offset = -BUG_SIZE / 2;
  dst->x = bug->x + offset;
  dst->y = bug->y + offset;
  SDL_RenderCopy(renderer, bug->texture, NULL, dst);
  free(dst);
}

void warn_if_sdl_error(const char *warning, bool condition) {
  if (!condition) {
    printf("WARN: %s\nSDL_Error: %s\n", warning, SDL_GetError());
  }
}
