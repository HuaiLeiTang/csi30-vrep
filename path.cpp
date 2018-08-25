#include "path.h"
#include <vector>
#include <iostream>
#include <stack>

using namespace std;

int current_command = 0;
int commands[] = {8, 4, 8, 6, 8, 6, 8, 4, 8, 4, 8, 8, 8, 8, 8, 8, 6, 8, 8, 8, 6, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8};

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

bool internal_depth_first_search(const state_t &state, stack<state_t> &path, const state_t &end_state, vector<state_t> &already_visited);

vector<state_t> possible_states(const state_t &state);
std::ostream& operator<<(std::ostream &o, const state_t &s);
std::ostream& operator<<(ostream& out, const vector<state_t>& vector);
bool operator==(const state_t &a, const state_t &b);

bool is_obstacle(int x, int y);
bool is_right_obstacle(const state_t &state);
bool is_left_obstacle(const state_t &state);

int get_next_command()
{
    return commands[current_command++];
}

void depth_first_search()
{
    stack<state_t> path;
    vector<state_t> already_visited;

    bool found = internal_depth_first_search(state_t(9, 9, 0), path, state_t(0, 9, 0), already_visited);
    if (found) {
        while (!path.empty()) {
            cout << path.top() << endl;
            path.pop();
        }
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

bool internal_depth_first_search(const state_t &state, stack<state_t> &path, const state_t &end_state, vector<state_t> &already_visited)
{
    if (is_already_visited(already_visited, state)) {
        return false;
    }

    already_visited.push_back(state);

    path.push(state);
    if (state == end_state) {
        return true;
    }

    for (auto &next_state : possible_states(state)) {
        bool found = internal_depth_first_search(next_state, path, end_state, already_visited);
        if (found) {
            return true;
        }
    }

    path.pop();
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