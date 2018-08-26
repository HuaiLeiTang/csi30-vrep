#include "path.h"
#include "state.h"
#include "command.h"

#include <vector>
#include <stack>

using namespace std;

#define X 3

#define MAP_SIZE 10

int map_representation[MAP_SIZE][MAP_SIZE] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, X, X, X, X, X, X},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

state_t create_start_state();
vector<state_t> create_end_states();

bool internal_depth_first_search(const state_t &state, const vector<state_t> &end_states, vector<state_t> &path,
                                 vector<state_t> &already_visited);

vector<state_t> expand_state(const state_t &state);

bool is_obstacle(int x, int y);
bool is_right_obstacle(const state_t &state);
bool is_left_obstacle(const state_t &state);

vector<int> depth_first_search()
{
    vector<state_t> path;
    vector<state_t> already_visited;
    bool found;

    const state_t &start_state = create_start_state();
    const vector<state_t> end_states = create_end_states();

    found = internal_depth_first_search(start_state, end_states, path, already_visited);
    if (found) {
        return to_commands(path);
    }
}

state_t create_start_state()
{
    return {9, 9, 0};
}

vector<state_t> create_end_states()
{
    return {{0, 9, 0}, {0, 9, 90}, {0, 9, 270}, {0, 9, 180}};
}

bool internal_depth_first_search(const state_t &state, const vector<state_t> &end_states, vector<state_t> &path,
                                 vector<state_t> &already_visited)
{
    if (find(already_visited.begin(), already_visited.end(), state) != already_visited.end()) {
        return false;
    }

    already_visited.push_back(state);

    path.push_back(state);
    if (find(end_states.begin(), end_states.end(), state) != end_states.end()) {
        return true;
    }

    for (auto &next_state : expand_state(state)) {
        bool found = internal_depth_first_search(next_state, end_states, path, already_visited);
        if (found) {
            return true;
        }
    }

    path.pop_back();
    return false;
}

vector<state_t> expand_state(const state_t &state)
{
    vector<state_t> result;

    if (state.theta == 0 && !is_obstacle(state.x - 1, state.y)) {
        result.push_back({state.x - 1, state.y, state.theta});
    } else if (state.theta == 90 && !is_obstacle(state.x, state.y - 1)) {
        result.push_back({state.x, state.y - 1, state.theta});
    } else if (state.theta == 270 && !is_obstacle(state.x, state.y + 1)) {
        result.push_back({state.x, state.y + 1, state.theta});
    } else if (state.theta == 180 && !is_obstacle(state.x + 1, state.y)) {
        result.push_back({state.x + 1, state.y, state.theta});
    }

    if (!is_right_obstacle(state)) {
        result.push_back({state.x, state.y, (state.theta + 270) % 360});
    }

    if (!is_left_obstacle(state)) {
        result.push_back({state.x, state.y, (state.theta + 90) % 360});
    }

    return result;
}

bool is_right_obstacle(const state_t &state)
{
    if (state.theta == 0) {
        return is_obstacle(state.x, state.y + 1);
    } else if (state.theta == 90) {
        return is_obstacle(state.x - 1, state.y);
    } else if (state.theta == 180) {
        return is_obstacle(state.x, state.y - 1);
    } else if (state.theta == 270) {
        return is_obstacle(state.x + 1, state.y);
    }
}

bool is_left_obstacle(const state_t &state)
{
    if (state.theta == 0) {
        return is_obstacle(state.x, state.y - 1);
    } else if (state.theta == 90) {
        return is_obstacle(state.x + 1, state.y);
    } else if (state.theta == 180) {
        return is_obstacle(state.x, state.y + 1);
    } else if (state.theta == 270) {
        return is_obstacle(state.x - 1, state.y);
    }
}

bool is_obstacle(int x, int y)
{
    if (x < 0 || y < 0) {
        return true;
    }

    if (x >= MAP_SIZE || y >= MAP_SIZE) {
        return true;
    }

    return map_representation[x][y] == X;
}