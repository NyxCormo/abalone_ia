#ifndef ABALONE_GAMESTATE_H
#define ABALONE_GAMESTATE_H

#include <optional>

#include "Board.h"

struct GameState {
    Board board;
    Player currentPlayer;
    int moveCount;
    std::optional<Player> winner;

    GameState()
            : board(),
              currentPlayer(Player::Black),
              moveCount(0),
              winner(std::nullopt) {}};

#endif //ABALONE_GAMESTATE_H