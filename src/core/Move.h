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

    [[nodiscard]] const std::vector<Position>& marbles() const { return marbles_; }
    [[nodiscard]] Direction direction() const { return direction_; }
    [[nodiscard]] MoveType type() const { return moveType_; }
    [[nodiscard]] int marbleCount() const { return static_cast<int>(marbles_.size()); }

    [[nodiscard]] Position front() const { return marbles_.front(); }
    [[nodiscard]] Position back() const { return marbles_.back(); }

    [[nodiscard]] bool isInline() const { return moveType_ == MoveType::INLINE; }
    [[nodiscard]] bool isSidestep() const { return moveType_ == MoveType::SIDESTEP; }

    [[nodiscard]] std::vector<Position> destinations() const;
    [[nodiscard]] std::string toString() const;

    bool operator==(const Move& other) const;
    bool operator!=(const Move& other) const { return !(*this == other); }

private:

    [[nodiscard]] MoveType detectMoveType() const;
    [[nodiscard]] Direction findAlignmentDirection() const;
    [[nodiscard]] bool areAligned(Direction dir) const;
    [[nodiscard]] bool areAlignedInDirection(Direction dir) const;
    void sortMarblesInDirection(Direction dir);
};

#endif