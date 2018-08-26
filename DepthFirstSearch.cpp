//Bruno Clemente e Thyago Stall
#include "DepthFirstSearch.hpp"
#include "Path.hpp"

std::vector<State> DepthFirstSearch::find() {
	depthSearch(start_state);
	return path;
}

bool DepthFirstSearch::depthSearch(State &state) {
	if (state == already_visited) {
		return false;
	}

	already_visited.push_back(state);

	path.push_back(state);
	if (state == end_states) {
		return true;
	}

	for (auto &next_state : state.expand()) {
		bool found = depthSearch(next_state);
		if (found) {
			return true;
		}
	}

	path.pop_back();
	return false;
}
