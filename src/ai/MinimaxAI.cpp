#include "MinimaxAI.h"

#include <algorithm>
#include <array>
#include <cstdint>
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

std::uint64_t splitmix64(std::uint64_t value) {
    value += 0x9e3779b97f4a7c15ULL;
    value = (value ^ (value >> 30U)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27U)) * 0x94d049bb133111ebULL;
    return value ^ (value >> 31U);
}

std::uint64_t zobristValue(std::uint64_t index) {
    return splitmix64(index + 1ULL);
}

constexpr int kPlayableCells = 61;
constexpr int kMaxEjectedMarbles = 6;
constexpr int kTurnIndex = 2 * kPlayableCells + 2 * (kMaxEjectedMarbles + 1);

std::uint64_t cellIndex(const Position& pos) {
    return static_cast<std::uint64_t>((pos.q() + 4) * 9 + (pos.r() + 4));
}

const std::array<std::uint64_t, kPlayableCells> kBlackCellHashes = [] {
    std::array<std::uint64_t, kPlayableCells> values{};
    for (int i = 0; i < kPlayableCells; ++i) {
        values[i] = zobristValue(static_cast<std::uint64_t>(i));
    }
    return values;
}();

const std::array<std::uint64_t, kPlayableCells> kWhiteCellHashes = [] {
    std::array<std::uint64_t, kPlayableCells> values{};
    for (int i = 0; i < kPlayableCells; ++i) {
        values[i] = zobristValue(static_cast<std::uint64_t>(kPlayableCells + i));
    }
    return values;
}();

const std::array<std::uint64_t, kMaxEjectedMarbles + 1> kBlackEjectedHashes = [] {
    std::array<std::uint64_t, kMaxEjectedMarbles + 1> values{};
    for (int i = 0; i <= kMaxEjectedMarbles; ++i) {
        values[i] = zobristValue(static_cast<std::uint64_t>(2 * kPlayableCells + i));
    }
    return values;
}();

const std::array<std::uint64_t, kMaxEjectedMarbles + 1> kWhiteEjectedHashes = [] {
    std::array<std::uint64_t, kMaxEjectedMarbles + 1> values{};
    for (int i = 0; i <= kMaxEjectedMarbles; ++i) {
        values[i] = zobristValue(static_cast<std::uint64_t>(2 * kPlayableCells + (kMaxEjectedMarbles + 1) + i));
    }
    return values;
}();

const std::uint64_t kWhiteToMoveHash = zobristValue(static_cast<std::uint64_t>(kTurnIndex));
}

MinimaxAI::MinimaxAI() : MinimaxAI(Settings{}) {}

MinimaxAI::MinimaxAI(Settings settings) : settings_(settings) {
    settings_.depth = clampDepth(settings_.depth);
    settings_.marbleWeight = clampWeight(settings_.marbleWeight);
    settings_.ejectionWeight = clampWeight(settings_.ejectionWeight);
    settings_.centerWeight = clampWeight(settings_.centerWeight);
    transpositionTable_.reserve(65536);
}

std::optional<Move> MinimaxAI::chooseMove(const GameState& state) const {
    transpositionTable_.clear();

    std::vector<Move> legalMoves = GameEngine::getLegalMoves(state);
    if (legalMoves.empty()) {
        return std::nullopt;
    }

    std::sort(legalMoves.begin(), legalMoves.end(), [](const Move& a, const Move& b) {
        return moveHeuristic(a) > moveHeuristic(b);
    });

    const Player player = state.currentPlayer;
    int bestScore = std::numeric_limits<int>::min();
    std::optional<Move> bestMove = legalMoves.front();

    for (const Move& move : legalMoves) {
        const GameState nextState = GameEngine::applyMove(state, move);
        const int score = -negamax(
            nextState,
            settings_.depth - 1,
            std::numeric_limits<int>::min() + 1,
            std::numeric_limits<int>::max(),
            opponent(player)
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

int MinimaxAI::evaluate(const GameState& state, Player player) const {
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
    std::uint64_t hash = 0;

    for (int q = -4; q <= 4; ++q) {
        for (int r = -4; r <= 4; ++r) {
            const Position pos(q, r);
            if (!pos.isValid()) {
                continue;
            }

            const Cell cell = state.board.get(pos);
            if (cell == Cell::Empty) {
                continue;
            }

            const std::size_t index = static_cast<std::size_t>(cellIndex(pos));
            hash ^= cell == Cell::Black ? kBlackCellHashes[index] : kWhiteCellHashes[index];
        }
    }

    hash ^= kBlackEjectedHashes[static_cast<std::size_t>(state.board.blackEjected())];
    hash ^= kWhiteEjectedHashes[static_cast<std::size_t>(state.board.whiteEjected())];

    if (state.currentPlayer == Player::White) {
        hash ^= kWhiteToMoveHash;
    }

    return hash;
}

int MinimaxAI::negamax(
    const GameState& state,
    int depth,
    int alpha,
    int beta,
    Player currentPlayer
) const {
    if (depth <= 0 || GameEngine::isGameOver(state)) {
        return evaluate(state, currentPlayer);
    }

    const int alphaOriginal = alpha;
    const int betaOriginal = beta;
    const std::uint64_t hash = hashState(state);

    const auto cached = transpositionTable_.find(hash);
    if (cached != transpositionTable_.end() && cached->second.depth >= depth) {
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
            opponent(currentPlayer)
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

    transpositionTable_[hash] = TranspositionEntry{depth, bestScore, bound};

    return bestScore;
}
