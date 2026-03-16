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
        selectedMarbles_.erase(it);
        return;
    }
    
    if(selectedMarbles_.size() >= 3) return;
    
    bool isAdjacent = false;
    for(const auto& selected : selectedMarbles_) {
        if(areAdjacent(selected, pos)) {
            isAdjacent = true;
            break;
        }
    }
    
    if(!isAdjacent) return;
    
    selectedMarbles_.push_back(pos);
    
    if(!areAligned()) {
        selectedMarbles_.pop_back();
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
    
    for(Direction dir : ALL_DIRECTIONS)
    {
        bool aligned = true;
        
        for(size_t i = 0; i < selectedMarbles_.size() - 1; i++)
        {
            Position expected = selectedMarbles_[i].neighbor(dir);
            
            bool found = false;
            for(size_t j = i + 1; j < selectedMarbles_.size(); j++)
            {
                if(selectedMarbles_[j] == expected) {
                    found = true;
                    break;
                }
            }
            
            if(!found) {
                aligned = false;
                break;
            }
        }
        
        if(aligned) return true;
    }
    
    return false;
}