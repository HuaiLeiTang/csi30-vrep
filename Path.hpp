//Bruno Clemente e Thyago Stall
#ifndef CPPREMOTEAPI_COMMAND_H
#define CPPREMOTEAPI_COMMAND_H

#include <vector>
#include "PathFinder.hpp"

std::vector<int> to_commands(const std::vector<State> &path);

class Path {
public:
	static std::vector<int> getPath(PathFinder *finder);
private:
	static int toCommand(const State &a, const State &b);
};

#endif //CPPREMOTEAPI_COMMAND_H
