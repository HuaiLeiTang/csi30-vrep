#ifndef CPPREMOTEAPI_COMMAND_H
#define CPPREMOTEAPI_COMMAND_H

#include "state.h"

#include <vector>

std::vector<int> to_commands(const std::vector<state_t> &path);

#endif //CPPREMOTEAPI_COMMAND_H
