//Bruno Clemente e Thyago Stall
#ifndef CPPREMOTEAPI_ASTARSEARCH_HPP
#define CPPREMOTEAPI_ASTARSEARCH_HPP

#include "PathFinder.hpp"

class AStarSearch : public PathFinder{
private:
	std::vector<State> createVector(State *state);
	void addState(std::vector<State *> &open_states, State *cursor_state, const State &s) const;
public:
	AStarSearch(const State &start_state, const std::vector<State> &end_states): PathFinder(start_state,
	                                                                                        end_states) {}

	std::vector<State> find() override;

};


#endif //CPPREMOTEAPI_ASTARSEARCH_HPP
