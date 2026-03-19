//
// Created by nyx on 3/13/26.
//

#ifndef ABALONE_DIRECTION_H
#define ABALONE_DIRECTION_H
#include <array>


enum class Direction {
    EAST,
    NORTH_EAST,
    NORTH_WEST,
    WEST,
    SOUTH_WEST,
    SOUTH_EAST,
};

constexpr std::array<Direction, 6> ALL_DIRECTIONS = {
    Direction::EAST,
    Direction::NORTH_EAST,
    Direction::NORTH_WEST,
    Direction::WEST,
    Direction::SOUTH_WEST,
    Direction::SOUTH_EAST
};

inline Direction opposite(Direction d) {
    return static_cast<Direction>((static_cast<int>(d) + 3) % 6);
}

#endif //ABALONE_DIRECTION_H