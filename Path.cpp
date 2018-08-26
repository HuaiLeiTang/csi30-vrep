#include "Path.hpp"

#define WALK 8
#define RIGHT 6
#define LEFT 4

std::vector<int> Path::getPath(PathFinder *finder) {
	std::vector<int> result;
	auto path = finder->find();

	for (int i = 1; i < (int) path.size(); ++i) {
		result.push_back(toCommand(path[i - 1], path[i]));
	}

	return result;
}

int Path::toCommand(const State &a, const State &b) {
	if (a.getX() != b.getX() || a.getY() != b.getY()) {
		return WALK;
	}

	if (a.getTheta() == 0 && b.getTheta() == 90) {
		return LEFT;
	}
	if (a.getTheta() == 90 && b.getTheta() == 180) {
		return LEFT;
	}
	if (a.getTheta() == 180 && b.getTheta() == 270) {
		return LEFT;
	}
	if (a.getTheta() == 270 && b.getTheta() == 0) {
		return LEFT;
	}
	if (a.getTheta() == 0 && b.getTheta() == 270) {
		return RIGHT;
	}
	if (a.getTheta() == 90 && b.getTheta() == 0) {
		return RIGHT;
	}
	if (a.getTheta() == 180 && b.getTheta() == 90) {
		return RIGHT;
	}
	if (a.getTheta() == 270 && b.getTheta() == 180) {
		return RIGHT;
	}
	return 0;
}
