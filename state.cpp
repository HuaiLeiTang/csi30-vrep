#include "path.h"
#include "state.h"

std::ostream& operator<<(std::ostream &o, const state_t &s)
{
    return o << "state{ " << s.x << ", " << s.y << ", " << s.theta << " }";
}

std::ostream& operator<<(std::ostream& out, const std::vector<state_t>& vector)
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