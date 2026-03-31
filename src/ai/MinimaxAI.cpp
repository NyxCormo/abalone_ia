#include "MinimaxAI.h"

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <limits>
#include <thread>
#include <vector>

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

int clampThreadCount(int threadCount) {
    return std::max(1, threadCount);
}

int clampWeight(int weight) {
    return std::max(0, weight);
}

std::uint64_t splitmix64(std::uint64_t value) {
    value += 0x9e3779b97f4a7c15ULL;
    value = (value ^ (value >> 30U)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27U)) * 0x94d049bb133111ebULL;
    return value ^ (value >> 31U);
}
}

MinimaxAI::MinimaxAI() : MinimaxAI(Settings{}) {}

MinimaxAI::MinimaxAI(Settings settings) : settings_(settings) {
    setSettings(settings);
}

std::optional<Move> MinimaxAI::chooseMove(const GameState& state) const {
    std::vector<Move> legalMoves = GameEngine::getLegalMoves(state);
    if (legalMoves.empty()) {
        return std::nullopt;
    }

    std::sort(legalMoves.begin(), legalMoves.end(), [](const Move& a, const Move& b) {
        return moveHeuristic(a) > moveHeuristic(b);
    });

    const Player player = state.currentPlayer;
    std::vector<int> scores(legalMoves.size(), std::numeric_limits<int>::min());
    const int workerCount = std::min<int>(settings_.threadCount, static_cast<int>(legalMoves.size()));
    std::atomic<std::size_t> nextMoveIndex{0};

    auto evaluateMoves = [&]() {
        TranspositionTable transpositionTable;
        transpositionTable.reserve(65536);

        while (true) {
            const std::size_t moveIndex = nextMoveIndex.fetch_add(1, std::memory_order_relaxed);
            if (moveIndex >= legalMoves.size()) {
                break;
            }

            transpositionTable.clear();
            const GameState nextState = GameEngine::applyMove(state, legalMoves[moveIndex]);
            scores[moveIndex] = -negamax(
                nextState,
                settings_.depth,
                std::numeric_limits<int>::min() + 1,
                std::numeric_limits<int>::max(),
                opponent(player),
                transpositionTable
            );
        }
    };

    std::vector<std::thread> workers;
    workers.reserve(static_cast<std::size_t>(std::max(0, workerCount - 1)));
    for (int i = 1; i < workerCount; ++i) {
        workers.emplace_back(evaluateMoves);
    }

    evaluateMoves();

    for (std::thread& worker : workers) {
        worker.join();
    }

    int bestScore = scores.front();
    std::optional<Move> bestMove = legalMoves.front();
    for (std::size_t i = 1; i < legalMoves.size(); ++i) {
        if (scores[i] > bestScore) {
            bestScore = scores[i];
            bestMove = legalMoves[i];
        }
    }

    return bestMove;
}

void MinimaxAI::setSettings(Settings settings) {
    settings.depth = clampDepth(settings.depth);
    if (settings.threadCount <= 0) {
        settings.threadCount = defaultThreadCount();
    } else {
        settings.threadCount = clampThreadCount(settings.threadCount);
    }
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

void MinimaxAI::setThreadCount(int threadCount) {
    if (threadCount <= 0) {
        settings_.threadCount = defaultThreadCount();
        return;
    }
    settings_.threadCount = clampThreadCount(threadCount);
}

int MinimaxAI::threadCount() const {
    return settings_.threadCount;
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

int MinimaxAI::evaluate(const GameState& state, Player player) const {
    if (GameEngine::isDraw(state)) {
        return 0;
    }

    if (state.winner.has_value()) {
        return state.winner.value() == player ? 100000 : -100000;
    }

    const Player opp = opponent(player);
    const Board& board = state.board;

    const int marbleDiff =
        (board.countMarbles(player) - board.countMarbles(opp)) * settings_.marbleWeight;
    const int ejectionDiff =
        (player == Player::Black
             ? board.whiteEjected() - board.blackEjected()
             : board.blackEjected() - board.whiteEjected()) * settings_.ejectionWeight;
    const int centerDiff =
        (centerControlScore(board, player) - centerControlScore(board, opp)) * settings_.centerWeight;

    return marbleDiff + ejectionDiff + centerDiff;
}

std::uint64_t MinimaxAI::hashState(const GameState& state) const {
    return splitmix64(GameEngine::hashPosition(state) ^ splitmix64(state.historySignature));
}

int MinimaxAI::defaultThreadCount() {
    const unsigned int detected = std::thread::hardware_concurrency();
    return detected == 0U ? 1 : static_cast<int>(detected);
}

int MinimaxAI::negamax(
    const GameState& state,
    int depth,
    int alpha,
    int beta,
    Player currentPlayer,
    TranspositionTable& transpositionTable
) const {
    if (depth <= 0 || GameEngine::isGameOver(state)) {
        return evaluate(state, currentPlayer);
    }

    const int alphaOriginal = alpha;
    const int betaOriginal = beta;
    const std::uint64_t hash = hashState(state);

    const auto cached = transpositionTable.find(hash);
    if (cached != transpositionTable.end() && cached->second.depth >= depth) {
        const TranspositionEntry& entry = cached->second;
        if (entry.bound == BoundType::Exact) {
            return entry.score;
        }
        if (entry.bound == BoundType::Lower) {
            alpha = std::max(alpha, entry.score);
        } else {
            beta = std::min(beta, entry.score);
        }
        if (alpha >= beta) {
            return entry.score;
        }
    }

    std::vector<Move> legalMoves = GameEngine::getLegalMoves(state);
    if (legalMoves.empty()) {
        return evaluate(state, currentPlayer);
    }

    std::sort(legalMoves.begin(), legalMoves.end(), [](const Move& a, const Move& b) {
        return moveHeuristic(a) > moveHeuristic(b);
    });

    int bestScore = std::numeric_limits<int>::min() + 1;
    for (const Move& move : legalMoves) {
        int score = -negamax(
            GameEngine::applyMove(state, move),
            depth - 1,
            -beta,
            -alpha,
            opponent(currentPlayer),
            transpositionTable
        );

        bestScore = std::max(bestScore, score);
        alpha = std::max(alpha, bestScore);
        if (alpha >= beta) {
            break;
        }
    }

    BoundType bound = BoundType::Exact;
    if (bestScore <= alphaOriginal) {
        bound = BoundType::Upper;
    } else if (bestScore >= betaOriginal) {
        bound = BoundType::Lower;
    }

    transpositionTable[hash] = TranspositionEntry{depth, bestScore, bound};

    return bestScore;
}
