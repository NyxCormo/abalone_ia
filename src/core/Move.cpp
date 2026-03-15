#include "Move.h"
#include <sstream>
#include <algorithm>

Move::Move(const Position &marble, Direction dir)
    : marbles_({marble}), direction_(dir), moveType_(MoveType::INLINE) {
}

Move::Move(const std::vector<Position>& marbles, Direction dir)
    : marbles_(marbles), direction_(dir) {

    std::sort(marbles_.begin(), marbles_.end(),
              [](const Position& a, const Position& b) {
                  if (a.q() != b.q()) return a.q() < b.q();
                  return a.r() < b.r();
              });

    moveType_ = detectMoveType();
}

MoveType Move::detectMoveType() const {
    if (marbles_.size() == 1) {
        return MoveType::INLINE;
    }

    if (areAligned(direction_)) {
        return MoveType::INLINE;
    }

    return MoveType::SIDESTEP;
}

bool Move::areAligned(Direction dir) const {
    if (marbles_.size() < 2) {
        return true;
    }

    for (size_t i = 0; i < marbles_.size() - 1; i++) {
        Position expected = marbles_[i].neighbor(dir);

        bool found = false;
        for (size_t j = i + 1; j < marbles_.size(); j++) {
            if (marbles_[j] == expected) {
                found = true;
                break;
            }
        }

        if (!found) {
            return false;
        }
    }

    return true;
}

std::vector<Position> Move::destinations() const {
    std::vector<Position> destinations;
    destinations.reserve(marbles_.size());

    for (const Position& marble : marbles_) {
        destinations.push_back(marble.neighbor(direction_));
    }

    return destinations;
}

std::string Move::toString() const {
    std::ostringstream oss;

    oss << (moveType_ == MoveType::INLINE ? "INLINE" : "SIDESTEP");
    oss << " [";

    for (size_t i = 0; i < marbles_.size(); i++) {
        oss << marbles_[i];
        if (i < marbles_.size() - 1) oss << ", ";
    }

    oss << "] -> ";

    switch (direction_) {
        case Direction::EAST:       oss << "EAST"; break;
        case Direction::NORTH_EAST: oss << "NORTH_EAST"; break;
        case Direction::NORTH_WEST: oss << "NORTH_WEST"; break;
        case Direction::WEST:       oss << "WEST"; break;
        case Direction::SOUTH_WEST: oss << "SOUTH_WEST"; break;
        case Direction::SOUTH_EAST: oss << "SOUTH_EAST"; break;
    }

    return oss.str();
}

bool Move::operator==(const Move& other) const {
    if (marbles_.size() != other.marbles_.size()) return false;
    if (direction_ != other.direction_) return false;

    for (size_t i = 0; i < marbles_.size(); i++) {
        if (marbles_[i] != other.marbles_[i]) return false;
    }

    return true;
}