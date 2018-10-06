#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#include "engine.h"
#include "logging.h"

static bool init_sdl(void);
static bool init_window(SDL_Window **window, const int width, const int height,
                        const char *title);
static bool init_renderer(SDL_Renderer **renderer, SDL_Window *window,
                          const int scale);
static bool init_image(void);
static bool init_controller(SDL_GameController **controller);

Engine *engine_create(const int window_width, const int window_height,
                      const int renderer_scale, const char *window_title) {
  const int scaled_window_width = window_width * renderer_scale;
  const int scaled_window_height = window_height * renderer_scale;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_GameController *controller;
  if (!init_sdl()) {
    WARN("%s:%d: init_sdl failed", __FILE__, __LINE__);
    return NULL;
  }
  if (!init_window(&window, scaled_window_width, scaled_window_height,
                   window_title)) {
    WARN("%s:%d: init_window failed", __FILE__, __LINE__);
    return NULL;
  }
  if (!init_renderer(&renderer, window, renderer_scale)) {
    WARN("%s:%d: init_renderer failed", __FILE__, __LINE__);
    return NULL;
  }
  if (!init_image()) {
    WARN("%s:%d: init_image failed", __FILE__, __LINE__);
    return NULL;
  }
  if (!init_controller(&controller)) {
    WARN("%s:%d: init controller failed -- OK!", __FILE__, __LINE__);
  }
  const Engine initialEngine = {.window = window,
                                .renderer = renderer,
                                .controller = controller,
                                .window_width = window_width,
                                .window_height = window_height,
                                .renderer_scale = renderer_scale,
                                .scaled_window_width = scaled_window_width,
                                .scaled_window_height = scaled_window_height,
                                .window_title = window_title};
  Engine *engine = (Engine *)malloc(sizeof(Engine));
  if (!engine) {
    printf("%s:%d: Allocating Engine failed", __FILE__, __LINE__);
    return NULL;
  }
  memcpy(engine, &initialEngine, sizeof(Engine));
  return engine;
}

void engine_destroy(Engine *engine) {
  if (engine->controller) {
    SDL_GameControllerClose(engine->controller);
    engine->controller = NULL;
  }
  if (engine->renderer) {
    SDL_DestroyRenderer(engine->renderer);
    engine->renderer = NULL;
  }
  if (engine->window) {
    SDL_DestroyWindow(engine->window);
    engine->window = NULL;
  }
  IMG_Quit();
  SDL_Quit();
}

static bool init_sdl() {
  static const Uint32 flags = SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER;
  if (SDL_Init(flags) != 0) {
    WARN("%s:%d: SDL_Init failed -- SDL_Error: %s", __FILE__, __LINE__,
         SDL_GetError());
    return false;
  }
  return true;
}

static bool init_window(SDL_Window **window, const int width, const int height,
                        const char *title) {
  static const Uint32 flags = SDL_WINDOW_OPENGL;
  *window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, width, height, flags);
  if (!*window) {
    WARN("%s:%d: SDL_CreateWindow failed -- SDL_Error: %s", __FILE__, __LINE__,
         SDL_GetError());
    return false;
  }
  return true;
}

static bool init_renderer(SDL_Renderer **renderer, SDL_Window *window,
                          const int scale) {
  static const Uint32 flags =
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  *renderer = SDL_CreateRenderer(window, -1, flags);
  if (!*renderer) {
    WARN("%s:%d: SDL_CreateRenderer failed -- SDL_Error: %s", __FILE__,
         __LINE__, SDL_GetError());
    return false;
  }
  if (SDL_RenderSetScale(*renderer, scale, scale) != 0) {
    WARN("%s:%d: SDL_RenderSetScale failed -- SDL_Error: %s", __FILE__,
         __LINE__, SDL_GetError());
  }
  static const int vsync = 1;
  if (SDL_GL_SetSwapInterval(vsync) == -1) {
    WARN("%s:%d: SDL_GL_SetSwapInterval failed -- SDL_GetError: %s", __FILE__,
         __LINE__, SDL_GetError());
    return false;
  };
  return true;
}

static bool init_image() {
  static const int IMAGE_FLAGS = IMG_INIT_PNG;
  const int flags = IMG_Init(IMAGE_FLAGS);
  if ((flags & IMAGE_FLAGS) != IMAGE_FLAGS) {
    WARN("%s:%d: IMG_Init failed -- IMG_Error: %s", __FILE__, __LINE__,
         IMG_GetError());
    return false;
  }
  return true;
}

static bool init_controller(SDL_GameController **controller) {
  *controller = NULL;
  if (SDL_NumJoysticks() < 0) {
    WARN("%s:%d: No joysticks found -- OK!", __FILE__, __LINE__);
    return false;
  }
  static const int joystick1 = 0;
  *controller = SDL_GameControllerOpen(joystick1);
  if (!*controller) {
    WARN("%s:%d: SDL_JoystickOpen failed -- SDL_Error: %s", __FILE__, __LINE__,
         SDL_GetError());
    return false;
  }
  return true;
}
