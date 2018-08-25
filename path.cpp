#include "path.h"
#include <vector>
#include <iostream>
#include <stack>

using namespace std;

int current_command = 0;
vector<int> commands;

typedef struct state_t {
    int x;
    int y;
    int theta;

    state_t(int x, int y, int theta)
    {
        this->x = x;
        this->y = y;
        this->theta = theta;
    }

} state_t;

#define WALK 8
#define RIGHT 6
#define LEFT 4

#define S 10
#define D 20

#define X 3

#define MAP_SIZE 10

int game_map[MAP_SIZE][MAP_SIZE] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, D},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, X, X, X, X, X, X},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, S},
};

bool internal_depth_first_search(const state_t &state, vector<state_t> &path, const state_t &end_state, vector<state_t> &already_visited);

vector<state_t> possible_states(const state_t &state);
std::ostream& operator<<(std::ostream &o, const state_t &s);
std::ostream& operator<<(ostream& out, const vector<state_t>& vector);
bool operator==(const state_t &a, const state_t &b);

bool is_obstacle(int x, int y);
bool is_right_obstacle(const state_t &state);
bool is_left_obstacle(const state_t &state);

void set_commands(const vector<int> &c)
{
    commands = c;
    current_command = 0;
}

int get_next_command()
{
    if (current_command < commands.size()) {
        return commands[current_command++];
    }
    return -1;
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

vector<int> to_commands(const vector<state_t> &path)
{
    vector<int> result;

    for (int i = 1; i < path.size(); ++i) {
        result.push_back(to_command(path[i - 1], path[i]));
    }

    return result;
}

vector<int> depth_first_search()
{

    vector<state_t> path;
    vector<state_t> already_visited;
    bool found;

    found = internal_depth_first_search(state_t(9, 9, 0), path, state_t(0, 9, 270), already_visited);
    if (found) {
        return to_commands(path);
    }
}

bool is_already_visited(const vector<state_t> &already_visited, const state_t &state)
{
    for (auto &v : already_visited) {
        if (v == state) {
            return true;
        }
    }
    return false;
}

bool internal_depth_first_search(const state_t &state, vector<state_t> &path, const state_t &end_state, vector<state_t> &already_visited)
{
    if (is_already_visited(already_visited, state)) {
        return false;
    }

    already_visited.push_back(state);

    path.push_back(state);
    if (state == end_state) {
        return true;
    }

    for (auto &next_state : possible_states(state)) {
        bool found = internal_depth_first_search(next_state, path, end_state, already_visited);
        if (found) {
            return true;
        }
    }

    path.pop_back();
    return false;
}

std::ostream& operator<<(std::ostream &o, const state_t &s)
{
    return o << "state{ " << s.x << ", " << s.y << ", " << s.theta << " }";
}

std::ostream& operator<<(ostream& out, const vector<state_t>& vector)
{
    out << "[ ";
    for (int i = 0; i < vector.size(); i++) {
        if (i) out << ", ";
        out << vector[i];
    }
    out << " ]";
    return out;
}

bool operator==(const state_t &a, const state_t &b)
{
    return a.x == b.x && a.y == b.y && a.theta == b.theta;
}

vector<state_t> possible_states(const state_t &state)
{
    vector<state_t> result;

    if (state.theta == 0 && !is_obstacle(state.x - 1, state.y)) {
        result.push_back(state_t(state.x - 1, state.y, state.theta));
    } else if (state.theta == 90 && !is_obstacle(state.x, state.y - 1)) {
        result.push_back(state_t(state.x, state.y - 1, state.theta));
    } else if (state.theta == 270 && !is_obstacle(state.x, state.y + 1)) {
        result.push_back(state_t(state.x, state.y + 1, state.theta));
    } else if (state.theta == 180 && !is_obstacle(state.x + 1, state.y)) {
        result.push_back(state_t(state.x + 1, state.y, state.theta));
    }

    if (!is_right_obstacle(state)) {
        result.push_back(state_t(state.x, state.y, (state.theta + 270) % 360));
    }

    if (!is_left_obstacle(state)) {
        result.push_back(state_t(state.x, state.y, (state.theta + 90) % 360));
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

    return game_map[x][y] == X;
}