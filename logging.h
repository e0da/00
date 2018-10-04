#ifndef LOGGING_H
#define LOGGING_H

#include <err.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define warn(...) emscripten_log(EM_LOG_WARN, __VA_ARGS__)
#endif

#endif
