//
// Created by nyx on 3/13/26.
//

#include "../geometry/Position.h"

#include <iosfwd>

static const int DIRECTION_VECTORS[6][2] = {
    {+1,  0},  // EAST
    {+1, -1},  // NORTH_EAST
    { 0, -1},  // NORTH_WEST
    {-1,  0},  // WEST
    {-1, +1},  // SOUTH_WEST
    { 0, +1}   // SOUTH_EAST
};

Position Position::neighbor(Direction dir) const {
    int dir_index = static_cast<int>(dir);
    return {
        q_ + DIRECTION_VECTORS[dir_index][0],
        r_ + DIRECTION_VECTORS[dir_index][1]
    };
}

std::ostream& operator<<(std::ostream& os, const Position& pos) {
    os << "(" << pos.q() << ", " << pos.r() << ")";
    return os;
}
