#ifndef ABALONE_GAMEENGINE_H
#define ABALONE_GAMEENGINE_H

#include "GameState.h"
#include "Move.h"
#include <vector>

class GameEngine {
public:
    static std::vector<Move> getLegalMoves(const GameState& state);

    static GameState applyMove(const GameState& state, const Move& move);

    static bool isGameOver(const GameState& state);

    static Player getWinner(const GameState& state);
};

#endif //ABALONE_GAMEENGINE_H