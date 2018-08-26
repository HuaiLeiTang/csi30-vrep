#include "command.h"

#define WALK 8
#define RIGHT 6
#define LEFT 4

int to_command(const state_t &a, const state_t &b);

std::vector<int> to_commands(const std::vector<state_t> &path)
{
    std::vector<int> result;

    for (int i = 1; i < path.size(); ++i) {
        result.push_back(to_command(path[i - 1], path[i]));
    }

    return result;
}

int to_command(const state_t &a, const state_t &b)
{
    if (a.x != b.x || a.y != b.y) {
        return WALK;
    }

    if (a.theta == 0 && b.theta == 90) {
        return LEFT;
    } else if (a.theta == 0 && b.theta == 270) {
        return RIGHT;
    } else if (a.theta == 90 && b.theta == 180) {
        return LEFT;
    } else if (a.theta == 90 && b.theta == 0) {
        return RIGHT;
    } else if (a.theta == 180 && b.theta == 270) {
        return LEFT;
    } else if (a.theta == 180 && b.theta == 90) {
        return RIGHT;
    } else if (a.theta == 270 && b.theta == 0) {
        return LEFT;
    } else if (a.theta == 270 && b.theta == 180) {
        return RIGHT;
    }
}
