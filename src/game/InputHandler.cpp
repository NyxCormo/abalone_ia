#include "InputHandler.h"
#include <algorithm>

#include "MoveGenerator.h"

InputHandler::InputHandler() = default;

void InputHandler::selectHex(Position pos, Player player, const Board& board)
{
    selectedMarbles_.clear();
    
    if(board.hasMarble(pos, player)) {
        selectedMarbles_.push_back(pos);
    }
}

void InputHandler::addToSelection(Position pos, Player player, const Board& board)
{
    if(!board.hasMarble(pos, player)) return;
    
    if(selectedMarbles_.empty()) {
        selectedMarbles_.push_back(pos);
        return;
    }
    
    auto it = std::find(selectedMarbles_.begin(), selectedMarbles_.end(), pos);
    if(it != selectedMarbles_.end()) {
        return;
    }

    if(selectedMarbles_.size() >= 3) return;

    std::vector<Position> testGroup = selectedMarbles_;
    testGroup.push_back(pos);

    if(isValidGroup(testGroup)) {
        selectedMarbles_.push_back(pos);
    }
}

void InputHandler::removeFromSelection(Position pos)
{
    auto it = std::find(selectedMarbles_.begin(), selectedMarbles_.end(), pos);
    if(it != selectedMarbles_.end()) {
        selectedMarbles_.erase(it);
    }
}

void InputHandler::clearSelection()
{
    selectedMarbles_.clear();
}

bool InputHandler::hasSelection() const
{
    return !selectedMarbles_.empty();
}

const std::vector<Position>& InputHandler::getSelectedMarbles() const
{
    return selectedMarbles_;
}

std::vector<Direction> InputHandler::getValidDirections(const Board& board, Player player) const
{
    std::vector<Direction> validDirs;

    if(selectedMarbles_.empty()) return validDirs;

    for(Direction dir : ALL_DIRECTIONS)
    {
        Move move(selectedMarbles_, dir);
        if(MoveGenerator::isLegalMove(board, player, move)) {
            validDirs.push_back(dir);
        }
    }

    return validDirs;
}

std::optional<Move> InputHandler::tryCreateMove(Direction dir, const Board& board, Player player) const
{
    if(selectedMarbles_.empty()) return std::nullopt;

    Move move(selectedMarbles_, dir);

    if(MoveGenerator::isLegalMove(board, player, move)) {
        return move;
    }

    return std::nullopt;
}

bool InputHandler::areAdjacent(const Position& a, const Position& b) {
    return a.distance(b) == 1;
}

bool InputHandler::areAligned() const
{
    if(selectedMarbles_.size() <= 2) return true;
    return isValidGroup(selectedMarbles_);
}

bool InputHandler::isValidGroup(const std::vector<Position>& group) const {
    if(group.empty() || group.size() > 3) return false;
    if(group.size() == 1) return true;

    if(group.size() == 2) {
        return areAdjacent(group[0], group[1]);}

    for(Direction dir : ALL_DIRECTIONS) {
        for(size_t startIdx = 0; startIdx < 3; startIdx++) {
            Position start = group[startIdx];
            Position next1 = start.neighbor(dir);
            Position next2 = next1.neighbor(dir);
            bool foundNext1 = false;
            bool foundNext2 = false;

            for(size_t i = 0; i < 3; i++) {
                if(i == startIdx) continue;
                if(group[i] == next1) foundNext1 = true;
                if(group[i] == next2) foundNext2 = true;
            }

            if(foundNext1 && foundNext2) {
                return true;
            }
        }
    }
    
    return false;
}