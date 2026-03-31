#include "MinimaxAI.h"

#include <algorithm>
#include <limits>

#include "../game/GameEngine.h"

namespace {
int centerControlScore(const Board& board, Player player) {
    int score = 0;
    for (const Position& pos : board.getMarblePositions(player)) {
        score += 4 - pos.distance(Position(0, 0));
    }
    return score;
}

int moveHeuristic(const Move& move) {
    int score = move.marbleCount() * 10;
    if (move.isInline()) {
        score += 8;
    }
    return score;
}

int clampDepth(int depth) {
    return std::max(1, depth);
}

int clampWeight(int weight) {
    return std::max(0, weight);
}
}

MinimaxAI::MinimaxAI() : MinimaxAI(Settings{}) {}

MinimaxAI::MinimaxAI(Settings settings) : settings_(settings) {
    settings_.depth = clampDepth(settings_.depth);
    settings_.marbleWeight = clampWeight(settings_.marbleWeight);
    settings_.ejectionWeight = clampWeight(settings_.ejectionWeight);
    settings_.centerWeight = clampWeight(settings_.centerWeight);
}

std::optional<Move> MinimaxAI::chooseMove(const GameState& state) const {
    std::vector<Move> legalMoves = GameEngine::getLegalMoves(state);
    if (legalMoves.empty()) {
        return std::nullopt;
    }

    std::sort(legalMoves.begin(), legalMoves.end(), [](const Move& a, const Move& b) {
        return moveHeuristic(a) > moveHeuristic(b);
    });

    const Player maximizingPlayer = state.currentPlayer;
    int bestScore = std::numeric_limits<int>::min();
    std::optional<Move> bestMove = legalMoves.front();

    for (const Move& move : legalMoves) {
        const GameState nextState = GameEngine::applyMove(state, move);
        const int score = minimax(
            nextState,
            settings_.depth - 1,
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            false,
            maximizingPlayer
        );

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

void MinimaxAI::setSettings(Settings settings) {
    settings.depth = clampDepth(settings.depth);
    settings.marbleWeight = clampWeight(settings.marbleWeight);
    settings.ejectionWeight = clampWeight(settings.ejectionWeight);
    settings.centerWeight = clampWeight(settings.centerWeight);
    settings_ = settings;
}

const MinimaxAI::Settings& MinimaxAI::settings() const {
    return settings_;
}

void MinimaxAI::setDepth(int depth) {
    settings_.depth = clampDepth(depth);
}

int MinimaxAI::depth() const {
    return settings_.depth;
}

void MinimaxAI::setMarbleWeight(int weight) {
    settings_.marbleWeight = clampWeight(weight);
}

int MinimaxAI::marbleWeight() const {
    return settings_.marbleWeight;
}

void MinimaxAI::setEjectionWeight(int weight) {
    settings_.ejectionWeight = clampWeight(weight);
}

int MinimaxAI::ejectionWeight() const {
    return settings_.ejectionWeight;
}

void MinimaxAI::setCenterWeight(int weight) {
    settings_.centerWeight = clampWeight(weight);
}

int MinimaxAI::centerWeight() const {
    return settings_.centerWeight;
}

int MinimaxAI::evaluate(const GameState& state, Player maximizingPlayer) const {
    if (state.winner.has_value()) {
        return state.winner.value() == maximizingPlayer ? 100000 : -100000;
    }

    const Player minimizingPlayer = opponent(maximizingPlayer);
    const Board& board = state.board;

    const int marbleDiff =
        (board.countMarbles(maximizingPlayer) - board.countMarbles(minimizingPlayer)) * settings_.marbleWeight;
    const int ejectionDiff =
        (maximizingPlayer == Player::Black
             ? board.whiteEjected() - board.blackEjected()
             : board.blackEjected() - board.whiteEjected()) * settings_.ejectionWeight;
    const int centerDiff =
        (centerControlScore(board, maximizingPlayer) - centerControlScore(board, minimizingPlayer)) * settings_.centerWeight;

    return marbleDiff + ejectionDiff + centerDiff;
}

int MinimaxAI::minimax(
    const GameState& state,
    int depth,
    int alpha,
    int beta,
    bool maximizing,
    Player maximizingPlayer
) const {
    if (depth <= 0 || GameEngine::isGameOver(state)) {
        return evaluate(state, maximizingPlayer);
    }

    std::vector<Move> legalMoves = GameEngine::getLegalMoves(state);
    if (legalMoves.empty()) {
        return evaluate(state, maximizingPlayer);
    }

    std::sort(legalMoves.begin(), legalMoves.end(), [](const Move& a, const Move& b) {
        return moveHeuristic(a) > moveHeuristic(b);
    });

    if (maximizing) {
        int bestScore = std::numeric_limits<int>::min();
        for (const Move& move : legalMoves) {
            bestScore = std::max(
                bestScore,
                minimax(GameEngine::applyMove(state, move), depth - 1, alpha, beta, false, maximizingPlayer)
            );
            alpha = std::max(alpha, bestScore);
            if (beta <= alpha) {
                break;
            }
        }
        return bestScore;
    }

    int bestScore = std::numeric_limits<int>::max();
    for (const Move& move : legalMoves) {
        bestScore = std::min(
            bestScore,
            minimax(GameEngine::applyMove(state, move), depth - 1, alpha, beta, true, maximizingPlayer)
        );
        beta = std::min(beta, bestScore);
        if (beta <= alpha) {
            break;
        }
    }
    return bestScore;
}
