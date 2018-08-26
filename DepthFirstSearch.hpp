#ifndef CPPREMOTEAPI_PATH_H
#define CPPREMOTEAPI_PATH_H

#include <vector>
#include "PathFinder.hpp"

class DepthFirstSearch : public PathFinder {
public:

	DepthFirstSearch(const State &start_state, const std::vector<State> &end_states) : PathFinder(start_state,
	                                                                                                end_states) {}

	std::vector<State> find() override;

private:
	std::vector<State> path;
	std::vector<State> already_visited;

	bool depthSearch(State &state);
};

#endif //CPPREMOTEAPI_PATH_H
