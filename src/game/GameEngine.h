#ifndef ABALONE_GAMEENGINE_H
#define ABALONE_GAMEENGINE_H

#include "GameState.h"
#include "Move.h"
#include <cstdint>
#include <vector>

class GameEngine {
public:
    static void initializeState(GameState& state);

    static std::vector<Move> getLegalMoves(const GameState& state);

    static GameState applyMove(const GameState& state, const Move& move);

    static bool isGameOver(const GameState& state);
    static bool isDraw(const GameState& state);

    static Player getWinner(const GameState& state);
    static std::uint64_t hashPosition(const GameState& state);
};

#endif //ABALONE_GAMEENGINE_H