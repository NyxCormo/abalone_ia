#ifndef ABALONE_MINIMAXAI_H
#define ABALONE_MINIMAXAI_H

#include <optional>

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
    Settings settings_;

    [[nodiscard]] int evaluate(const GameState& state, Player maximizingPlayer) const;
    [[nodiscard]] int minimax(
        const GameState& state,
        int depth,
        int alpha,
        int beta,
        bool maximizing,
        Player maximizingPlayer
    ) const;
};

#endif // ABALONE_MINIMAXAI_H
