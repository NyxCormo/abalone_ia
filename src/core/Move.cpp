#include "Move.h"
#include <sstream>
#include <algorithm>

Move::Move(const Position &marble, Direction dir)
    : marbles_({marble}), direction_(dir), moveType_(MoveType::INLINE) {
}

Move::Move(const std::vector<Position>& marbles, Direction dir)
    : marbles_(marbles), direction_(dir) {

    moveType_ = detectMoveType();

    if (moveType_ == MoveType::INLINE) {
        sortMarblesInDirection(dir);
    } else {
        std::sort(marbles_.begin(), marbles_.end(),
                  [](const Position& a, const Position& b) {
                      if (a.q() != b.q()) return a.q() < b.q();
                      return a.r() < b.r();
                  });
    }
}

void Move::sortMarblesInDirection(Direction dir) {
    std::sort(marbles_.begin(), marbles_.end(),
              [dir](const Position& a, const Position& b) {
                  int proj_a = 0, proj_b = 0;

                  switch(dir) {
                      case Direction::EAST:
                          proj_a = a.q();
                          proj_b = b.q();
                          break;
                      case Direction::WEST:
                          proj_a = -a.q();
                          proj_b = -b.q();
                          break;
                      case Direction::NORTH_EAST:
                          proj_a = a.q() - a.r();
                          proj_b = b.q() - b.r();
                          break;
                      case Direction::SOUTH_WEST:
                          proj_a = -(a.q() - a.r());
                          proj_b = -(b.q() - b.r());
                          break;
                      case Direction::NORTH_WEST:
                          proj_a = -a.r();
                          proj_b = -b.r();
                          break;
                      case Direction::SOUTH_EAST:
                          proj_a = a.r();
                          proj_b = b.r();
                          break;
                  }

                  return proj_a < proj_b;
              });
}

MoveType Move::detectMoveType() const {
    if (marbles_.size() == 1) {
        return MoveType::INLINE;
    }

    Direction alignmentDir = findAlignmentDirection();

    if (alignmentDir == direction_ || alignmentDir == opposite(direction_)) {
        return MoveType::INLINE;
    } else {
        return MoveType::SIDESTEP;
    }
}

Direction Move::findAlignmentDirection() const {
    if (marbles_.size() < 2) {
        return direction_;
    }

    for (Direction testDir : ALL_DIRECTIONS) {
        if (areAlignedInDirection(testDir)) {
            return testDir;
        }
    }

    return direction_;
}

bool Move::areAlignedInDirection(Direction dir) const {
    if (marbles_.size() < 2) {
        return true;
    }

    std::vector<Position> sorted = marbles_;
    std::sort(sorted.begin(), sorted.end(),
              [dir](const Position& a, const Position& b) {
                  int proj_a = 0, proj_b = 0;

                  switch(dir) {
                      case Direction::EAST:
                          proj_a = a.q();
                          proj_b = b.q();
                          break;
                      case Direction::WEST:
                          proj_a = -a.q();
                          proj_b = -b.q();
                          break;
                      case Direction::NORTH_EAST:
                          proj_a = a.q() - a.r();
                          proj_b = b.q() - b.r();
                          break;
                      case Direction::SOUTH_WEST:
                          proj_a = -(a.q() - a.r());
                          proj_b = -(b.q() - b.r());
                          break;
                      case Direction::NORTH_WEST:
                          proj_a = -a.r();
                          proj_b = -b.r();
                          break;
                      case Direction::SOUTH_EAST:
                          proj_a = a.r();
                          proj_b = b.r();
                          break;
                  }

                  return proj_a < proj_b;
              });

    for (size_t i = 0; i < sorted.size() - 1; i++) {
        Position expected = sorted[i].neighbor(dir);
        if (expected != sorted[i + 1]) {
            return false;
        }
    }

    return true;
}

bool Move::areAligned(Direction dir) const {
    return areAlignedInDirection(dir);
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