//
// Created by nyx on 3/13/26.
//

#ifndef ABALONE_POSITION_H
#define ABALONE_POSITION_H

#include <cmath>
#include <iostream>
#include "Direction.h"


class Position {

    int q_;
    int r_;

public:

    Position() : q_(0), r_(0) {}

    Position(int q, int r) : q_(q), r_(r) {}

    int q() const { return q_; }
    int r() const { return r_; }
    int s() const { return -q_ - r_; }

    bool isValid() const {
        int s = -q_ - r_;
        return std::abs(q_) <= 4 && std::abs(r_) <= 4 && std::abs(s) <= 4;
    }
    Position neighbor(Direction dir) const;

    int distance(const Position& other) const {
        return (std::abs(q_ - other.q_) +
                std::abs(r_ - other.r_) +
                std::abs(s() - other.s())) / 2;
    }

    bool operator==(const Position& other) const {
        return q_ == other.q_ && r_ == other.r_;
    }

    bool operator!=(const Position& other) const {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& os, const Position& pos);
};

namespace std {
    template<>
    struct hash<Position> {
        size_t operator()(const Position& pos) const {
            return hash<int>()(pos.q()) ^ (hash<int>()(pos.r()) << 1);
        }
    };
}


#endif //ABALONE_POSITION_H