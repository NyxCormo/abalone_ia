#ifndef ABALONE_MINIMAXAI_H
#define ABALONE_MINIMAXAI_H

#include <cstdint>
#include <optional>
#include <unordered_map>

#include "../game/GameState.h"
#include "../game/Move.h"

class MinimaxAI {
public:
    struct Settings {
        int depth = 2;
        int marbleWeight = 100;
        int ejectionWeight = 120;
        int centerWeight = 1;
    };

    MinimaxAI();
    explicit MinimaxAI(Settings settings);

    [[nodiscard]] std::optional<Move> chooseMove(const GameState& state) const;
    void setSettings(Settings settings);
    [[nodiscard]] const Settings& settings() const;
    void setDepth(int depth);
    [[nodiscard]] int depth() const;
    void setMarbleWeight(int weight);
    [[nodiscard]] int marbleWeight() const;
    void setEjectionWeight(int weight);
    [[nodiscard]] int ejectionWeight() const;
    void setCenterWeight(int weight);
    [[nodiscard]] int centerWeight() const;

private:
    enum class BoundType {
        Exact,
        Lower,
        Upper,
    };

    struct TranspositionEntry {
        int depth = 0;
        int score = 0;
        BoundType bound = BoundType::Exact;
    };

    Settings settings_;
    mutable std::unordered_map<std::uint64_t, TranspositionEntry> transpositionTable_;

    [[nodiscard]] int evaluate(const GameState& state, Player player) const;
    [[nodiscard]] std::uint64_t hashState(const GameState& state) const;
    [[nodiscard]] int negamax(
        const GameState& state,
        int depth,
        int alpha,
        int beta,
        Player currentPlayer
    ) const;
};

#endif // ABALONE_MINIMAXAI_H
