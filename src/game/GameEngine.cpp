#include "GameEngine.h"

#include <array>
#include <cstdint>

#include "MoveGenerator.h"
#include "Game_Rules.h"

namespace {
constexpr int kPlayableCells = 61;
constexpr int kMaxEjectedMarbles = 6;
constexpr int kTurnIndex = 2 * kPlayableCells + 2 * (kMaxEjectedMarbles + 1);

std::uint64_t splitmix64(std::uint64_t value) {
    value += 0x9e3779b97f4a7c15ULL;
    value = (value ^ (value >> 30U)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27U)) * 0x94d049bb133111ebULL;
    return value ^ (value >> 31U);
}

std::uint64_t zobristValue(std::uint64_t index) {
    return splitmix64(index + 1ULL);
}

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

std::uint64_t extendHistorySignature(std::uint64_t signature, std::uint64_t positionHash) {
    return splitmix64(signature ^ splitmix64(positionHash));
}

int countOccurrences(const std::vector<std::uint64_t>& history, std::uint64_t positionHash) {
    int occurrences = 0;
    for (std::uint64_t previousHash : history) {
        if (previousHash == positionHash) {
            ++occurrences;
        }
    }
    return occurrences;
}
}

void GameEngine::initializeState(GameState& state) {
    state.currentPlayer = Player::Black;
    state.moveCount = 0;
    state.winner.reset();
    state.draw = false;
    state.positionHistory.clear();
    state.historySignature = 0;

    const std::uint64_t initialHash = hashPosition(state);
    state.positionHistory.push_back(initialHash);
    state.historySignature = extendHistorySignature(state.historySignature, initialHash);
}

std::vector<Move> GameEngine::getLegalMoves(const GameState& state) {
    return MoveGenerator::generateMoves(state.board, state.currentPlayer);
}

GameState GameEngine::applyMove(const GameState& state, const Move& move) {
    GameState newState = state;

    Game_Rules::applyMove(newState.board, move, state.currentPlayer);

    newState.currentPlayer = opponent(state.currentPlayer);
    newState.moveCount++;
    newState.winner.reset();
    newState.draw = false;

    if (Game_Rules::isGameOver(newState.board)) {
        newState.winner = Game_Rules::getWinner(newState.board);
        return newState;
    }

    const std::uint64_t positionHash = hashPosition(newState);
    newState.positionHistory.push_back(positionHash);
    newState.historySignature = extendHistorySignature(newState.historySignature, positionHash);
    if (countOccurrences(newState.positionHistory, positionHash) >= 3) {
        newState.draw = true;
    }

    return newState;
}

bool GameEngine::isGameOver(const GameState& state) {
    return state.winner.has_value() || state.draw;
}

bool GameEngine::isDraw(const GameState& state) {
    return state.draw;
}

Player GameEngine::getWinner(const GameState& state) {
    return state.winner.value_or(Game_Rules::getWinner(state.board));
}

std::uint64_t GameEngine::hashPosition(const GameState& state) {
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
