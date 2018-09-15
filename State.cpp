//Bruno Clemente e Thyago Stall
#include "State.hpp"
#include "Map.hpp"
#include <algorithm>
#include <math.h>


std::vector<State> State::expand() {
	std::vector<State> result;

	if (this->theta == 0 && !isObstacle(this->x - 1, this->y)) {
		result.emplace_back(this->x - 1, this->y, this->theta, this->cost + 1);
	} else if (this->theta == 90 && !isObstacle(this->x, this->y - 1)) {
		result.emplace_back(this->x, this->y - 1, this->theta, this->cost + 1);
	} else if (this->theta == 270 && !isObstacle(this->x, this->y + 1)) {
		result.emplace_back(this->x, this->y + 1, this->theta, this->cost + 1);
	} else if (this->theta == 180 && !isObstacle(this->x + 1, this->y)) {
		result.emplace_back(this->x + 1, this->y, this->theta, this->cost + 1);
	}

	if (!isRightObstacle()) {
		result.emplace_back(this->x, this->y, (this->theta + 270) % 360, this->cost + 1);
	}

	if (!isLeftObstacle()) {
		result.emplace_back(this->x, this->y, (this->theta + 90) % 360, this->cost + 1);
	}

	return result;
}

bool State::isRightObstacle() {
	if (this->theta == 0) {
		return isObstacle(this->x, this->y + 1);
	} else if (this->theta == 90) {
		return isObstacle(this->x - 1, this->y);
	} else if (this->theta == 180) {
		return isObstacle(this->x, this->y - 1);
	} else if (this->theta == 270) {
		return isObstacle(this->x + 1, this->y);
	}
}

bool State::isLeftObstacle()  {
	if (this->theta == 0) {
		return isObstacle(this->x, this->y - 1);
	} else if (this->theta == 90) {
		return isObstacle(this->x + 1, this->y);
	} else if (this->theta == 180) {
		return isObstacle(this->x, this->y + 1);
	} else {
		return isObstacle(this->x - 1, this->y);
	}
}

bool State::isObstacle(int x, int y)  {
	if (x < 0 || y < 0) {
		return true;
	}

	if (x >= Map::getSize() || y >= Map::getSize()) {
		return true;
	}

	return Map::isObstacle(x, y);
}

bool operator==(const State &s, const std::vector<State> &rhs) {
	return find(rhs.begin(), rhs.end(), s) != rhs.end();
}

bool operator==(const State &s, const std::vector<State*> &rhs) {
	return find(rhs.begin(), rhs.end(), s) != rhs.end();
}

bool operator==(const State *p_s, const State &s) {
	return s.x == p_s->x &&
	       s.y == p_s->y &&
	       s.theta == p_s->theta;
}

bool State::operator==(const State &rhs) const {
	return x == rhs.x &&
		   y == rhs.y &&
		   theta == rhs.theta;
}

std::ostream& operator<<(std::ostream &o, const State &s) {
	return o << "state{ " << s.x << ", " << s.y << ", " << s.theta << " }";
}

bool State::operator<(const State &b) {
	State end_states[] = END_STATES;
	return this->cost + euclidianDistance(*this, end_states[0])  < b.cost + euclidianDistance((State&)b, end_states[0]);
}

double State::euclidianDistance(State &a, State &b) {
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

std::ostream& operator<<(std::ostream& out, const std::vector<State>& vector) {
	out << "[ ";
	for (int i = 0; i < (int) vector.size(); i++) {
		if (i) out << ", ";
		out << vector[i];
	}
	out << " ]";
	return out;
}
