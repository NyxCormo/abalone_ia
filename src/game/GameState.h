#ifndef ABALONE_GAMESTATE_H
#define ABALONE_GAMESTATE_H

#include <cstdint>
#include <optional>
#include <vector>

#include "Board.h"

struct GameState {
    Board board;
    Player currentPlayer;
    int moveCount;
    std::optional<Player> winner;
    bool draw;
    std::vector<std::uint64_t> positionHistory;
    std::uint64_t historySignature;

    GameState()
            : board(),
              currentPlayer(Player::Black),
              moveCount(0),
              winner(std::nullopt),
              draw(false),
              positionHistory(),
              historySignature(0) {}};

#endif //ABALONE_GAMESTATE_H
