#ifndef LOGGING_H
#define LOGGING_H

#include <err.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define WARN(...) emscripten_log(EM_LOG_WARN, __VA_ARGS__)
#else
#define WARN(...) warnx(__VA_ARGS__)
#endif

#endif
