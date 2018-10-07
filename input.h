#ifndef INPUT_H
#define INPUT_H

#include "state.h"

void input_handle_events(State *state, void (*quit_callback)(void));

#endif
