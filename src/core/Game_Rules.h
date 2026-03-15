//
// Created by nyx on 3/14/26.
//

#ifndef ABALONE_GAME_RULES_H
#define ABALONE_GAME_RULES_H

#include "Board.h"
#include "Move.h"


class Game_Rules {
public:

    static void applyMove(Board& board, const Move& move, Player player);

    static bool isGameOver(const Board& board);
    static Player getWinner(const Board& board);

private:

    static void applyInlineMove(Board& board, const Move& move, Player player);
    static void applySideStepMove(Board& board, const Move& move, Player player);

    static Position findFrontMarble(const std::vector<Position>& marbles, Direction dir);
};


#endif //ABALONE_GAME_RULES_H