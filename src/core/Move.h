#ifndef ABALONE_MOVE_H
#define ABALONE_MOVE_H

#include "Position.h"
#include "Direction.h"
#include <vector>
#include <string>

enum class MoveType {
    INLINE,
    SIDESTEP
};

class Move {
private:
    std::vector<Position> marbles_;
    Direction direction_;
    MoveType moveType_;

public:
    Move(const Position& marble, Direction dir);
    Move(const std::vector<Position>& marbles, Direction dir);

    const std::vector<Position>& marbles() const { return marbles_; }
    Direction direction() const { return direction_; }
    MoveType type() const { return moveType_; }
    int marbleCount() const { return marbles_.size(); }

    Position front() const { return marbles_.front(); }
    Position back() const { return marbles_.back(); }

    bool isInline() const { return moveType_ == MoveType::INLINE; }
    bool isSidestep() const { return moveType_ == MoveType::SIDESTEP; }

    std::vector<Position> destinations() const;
    std::string toString() const;

    bool operator==(const Move& other) const;
    bool operator!=(const Move& other) const { return !(*this == other); }

private:
    MoveType detectMoveType() const;
    bool areAligned(Direction dir) const;
};

#endif