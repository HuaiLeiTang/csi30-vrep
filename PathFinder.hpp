//Bruno Clemente e Thyago Stall
#ifndef CPPREMOTEAPI_PATHFINDER_HPP
#define CPPREMOTEAPI_PATHFINDER_HPP

#include <vector>
#include "State.hpp"

class PathFinder {
public:
	PathFinder(State start_state, std::vector<State> end_states): start_state(start_state), end_states(end_states) {}

	virtual std::vector<State> find() = 0;

protected:
	std::vector<State> end_states;
	State start_state;

};


#endif //CPPREMOTEAPI_PATHFINDER_HPP
