#include "Game_Rules.h"
#include <algorithm>

void Game_Rules::applyMove(Board& board, const Move& move, Player player) {
    if (move.isInline()) {
        applyInlineMove(board, move, player);
    } else {
        applySideStepMove(board, move, player);
    }
}

bool Game_Rules::isGameOver(const Board& board) {
    return board.isGameOver();
}

Player Game_Rules::getWinner(const Board& board) {
    return board.winner();
}

Position Game_Rules::findFrontMarble(const std::vector<Position>& marbles, Direction dir) {
    return marbles.back();
}

void Game_Rules::applyInlineMove(Board& board, const Move& move, Player player) {
    Direction dir = move.direction();
    const auto& marbles = move.marbles();

    Position front = marbles.back();
    Position back = marbles.front();

    Cell ourCell = playerToCell(player);
    Position target = front.neighbor(dir);

    if (board.isEmpty(target)) {
        for (const Position& marble : marbles) {
            board.set(marble, Cell::Empty);
        }
        for (const Position& marble : marbles) {
            Position pos = marble.neighbor(dir);
            board.set(pos, ourCell);
        }
        return;
    }

    std::vector<Position> opponentMarbles;
    Position checkPos = target;

    while (checkPos.isValid() && board.hasMarble(checkPos, opponent(player))) {
        opponentMarbles.push_back(checkPos);
        checkPos = checkPos.neighbor(dir);
    }

    Position afterOpponent = checkPos;

    if (!afterOpponent.isValid()) {
        if (player == Player::Black) {
            board.ejectWhite();
        } else {
            board.ejectBlack();
        }

        for (int i = static_cast<int>(opponentMarbles.size()) - 2; i >= 0; i--) {
            Cell cell = board.get(opponentMarbles[i]);
            board.set(opponentMarbles[i+1], cell);
        }

        if (!opponentMarbles.empty()) {
            board.set(opponentMarbles[0], ourCell);
        }
    } else {
        Cell lastOpponentCell = board.get(opponentMarbles.back());
        board.set(afterOpponent, lastOpponentCell);

        for (int i = static_cast<int>(opponentMarbles.size()) - 2; i >= 0; i--) {
            Cell cell = board.get(opponentMarbles[i]);
            board.set(opponentMarbles[i+1], cell);
        }

        board.set(target, ourCell);
    }

    for (size_t i = 0; i < marbles.size() - 1; i++) {
        Position pos = marbles[i].neighbor(dir);
        board.set(pos, ourCell);
    }

    board.set(back, Cell::Empty);
}

void Game_Rules::applySideStepMove(Board& board, const Move& move, Player player) {
    const auto& marbles = move.marbles();
    Direction dir = move.direction();

    Cell ourCell = playerToCell(player);

    for (const Position& marble : marbles) {
        board.set(marble, Cell::Empty);
    }

    for (const Position& marble : marbles) {
        Position pos = marble.neighbor(dir);
        board.set(pos, ourCell);
    }
}