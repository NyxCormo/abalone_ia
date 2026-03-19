#include "GameEngine.h"
#include "MoveGenerator.h"
#include "Game_Rules.h"

std::vector<Move> GameEngine::getLegalMoves(const GameState& state) {
    return MoveGenerator::generateMoves(state.board, state.currentPlayer);
}

GameState GameEngine::applyMove(const GameState& state, const Move& move) {
    GameState newState = state;

    Game_Rules::applyMove(newState.board, move, state.currentPlayer);

    newState.currentPlayer = opponent(state.currentPlayer);
    newState.moveCount++;

    if (Game_Rules::isGameOver(newState.board)) {
        newState.winner = Game_Rules::getWinner(newState.board);
    }

    return newState;
}

bool GameEngine::isGameOver(const GameState& state) {
    return state.winner.has_value();
}

Player GameEngine::getWinner(const GameState& state) {
    return Game_Rules::getWinner(state.board);
}