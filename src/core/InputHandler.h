#ifndef ABALONE_INPUTHANDLER_H
#define ABALONE_INPUTHANDLER_H

#include "Board.h"
#include "Position.h"
#include "Direction.h"
#include "Move.h"
#include <vector>
#include <optional>

class InputHandler {
public:
    InputHandler();

    void selectHex(Position pos, Player player, const Board& board);
    void addToSelection(Position pos, Player player, const Board& board);
    void clearSelection();
    
    [[nodiscard]] bool hasSelection() const;
    [[nodiscard]] const std::vector<Position>& getSelectedMarbles() const;
    [[nodiscard]] std::vector<Direction> getValidDirections(const Board& board, Player player) const;
    
    [[nodiscard]] std::optional<Move> tryCreateMove(Direction dir, const Board& board, Player player) const;

private:
    std::vector<Position> selectedMarbles_;

    static bool areAdjacent(const Position& a, const Position& b);
    [[nodiscard]] bool areAligned() const;
};

#endif