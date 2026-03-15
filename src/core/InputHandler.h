#ifndef ABALONE_INPUTHANDLER_H
#define ABALONE_INPUTHANDLER_H

#include "Board.h"
#include "Position.h"
#include "Direction.h"
#include "Move.h"
#include "MoveGenerator.h"
#include <vector>
#include <optional>

class InputHandler {
public:
    InputHandler();

    void selectHex(Position pos, Player player, const Board& board);
    void addToSelection(Position pos, Player player, const Board& board);
    void clearSelection();
    
    bool hasSelection() const;
    const std::vector<Position>& getSelectedMarbles() const;
    std::vector<Direction> getValidDirections(const Board& board, Player player) const;
    
    std::optional<Move> tryCreateMove(Direction dir, const Board& board, Player player) const;

private:
    std::vector<Position> selectedMarbles_;
    
    bool areAdjacent(const Position& a, const Position& b) const;
    bool areAligned() const;
};

#endif