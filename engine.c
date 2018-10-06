#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "engine.h"
#include "logging.h"
#include "types.h"

static bool init_sdl(void);
static bool init_window(SDL_Window **window, const int width, const int height,
                        const char *title);
static bool init_renderer(SDL_Renderer **renderer, SDL_Window *window,
                          const int scale);
static bool init_image(void);
static bool init_controller(SDL_GameController **controller);

bool EngineInit(SDL_Window **window, SDL_Renderer **renderer,
                SDL_GameController **controller, const int width,
                const int height, const int scale, const char *title) {
  if (!init_sdl()) {
    WARN("%s:%d: init_sdl failed in init", __FILE__, __LINE__);
    return false;
  }
  if (!init_window(window, width, height, title)) {
    WARN("%s:%d: init_window failed in init", __FILE__, __LINE__);
    return false;
  }
  if (!init_renderer(renderer, *window, scale)) {
    WARN("%s:%d: init_renderer failed in init", __FILE__, __LINE__);
    return false;
  }
  if (!init_image()) {
    WARN("%s:%d: init_image failed in init", __FILE__, __LINE__);
    return false;
  }
  if (!init_controller(controller)) {
    WARN("%s:%d: init controller failed in init -- OK!", __FILE__, __LINE__);
  }
  return true;
}

void EngineQuit(SDL_Window *window, SDL_Renderer *renderer,
                SDL_GameController *controller) {
  if (controller)
    SDL_GameControllerClose(controller);
  if (renderer)
    SDL_DestroyRenderer(renderer);
  if (window)
    SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();
}

static bool init_sdl() {
  static const Uint32 flags = SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER;
  if (SDL_Init(flags) != 0) {
    WARN("%s:%d: SDL_Init failed in init_sdl -- SDL_Error: %s", __FILE__,
         __LINE__, SDL_GetError());
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
    WARN("%s:%d: SDL_CreateWindow failed in init_window -- SDL_Error: %s",
         __FILE__, __LINE__, SDL_GetError());
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
    WARN("%s:%d: SDL_CreateRenderer failed in init_renderer -- SDL_Error: "
         "%s",
         __FILE__, __LINE__, SDL_GetError());
    return false;
  }
  SDL_RenderSetScale(*renderer, scale, scale);
  static const int vsync = 1;
  if (SDL_GL_SetSwapInterval(vsync) == -1) {
    WARN("%s:%d: SDL_RenderSetScale failed in init_renderer -- Swap "
         "interval "
         "not supported -- SDL_GetError: %s",
         __FILE__, __LINE__, SDL_GetError());
    return false;
  };
  return true;
}

static bool init_image() {
  static const int IMAGE_FLAGS = IMG_INIT_PNG;
  const int flags = IMG_Init(IMAGE_FLAGS);
  if ((flags & IMAGE_FLAGS) != IMAGE_FLAGS) {
    WARN("%s:%d: IMG_Init failed in init_image -- IMG_Error: %s", __FILE__,
         __LINE__, IMG_GetError());
    return false;
  }
  return true;
}

static bool init_controller(SDL_GameController **controller) {
  *controller = NULL;
  if (SDL_NumJoysticks() < 0) {
    WARN("%s:%d: No joysticks found in init_controller -- OK!", __FILE__,
         __LINE__);
    return false;
  }
  static const int joystick1 = 0;
  *controller = SDL_GameControllerOpen(joystick1);
  if (!*controller) {
    WARN("%s:%d: SDL_JoystickOpen failed in init_controller -- SDL_Error: "
         "%s",
         __FILE__, __LINE__, SDL_GetError());
    return false;
  }
  return true;
}
