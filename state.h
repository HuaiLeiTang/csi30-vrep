#ifndef CPPREMOTEAPI_STATE_H
#define CPPREMOTEAPI_STATE_H

#include <iostream>

typedef struct state_t {
    int x;
    int y;
    int theta;
} state_t;

std::ostream& operator<<(std::ostream &o, const state_t &s);
std::ostream& operator<<(std::ostream& out, const std::vector<state_t>& vector);
bool operator==(const state_t &a, const state_t &b);

#endif //CPPREMOTEAPI_STATE_H
