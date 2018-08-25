#include "path.h"
#include <vector>
#include <iostream>

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

vector<state_t> possible_states(const state_t &state);
std::ostream& operator<<(ostream& out, const vector<state_t>& vector);

bool is_obstacle(int x, int y);
bool is_right_obstacle(const state_t &state);
bool is_left_obstacle(const state_t &state);

int get_next_command()
{
    return commands[current_command++];
}

void depth_first_search()
{
    cout << possible_states(state_t(4, 8, 180)) << endl;
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