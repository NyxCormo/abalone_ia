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
    for (const Position& marble : marbles) {
        Position behind = marble.neighbor(opposite(dir));
        bool isBehind = std::find(marbles.begin(), marbles.end(), behind) != marbles.end();

        if (!isBehind) {
            return marble;
        }
    }
    return marbles[0];
}

void Game_Rules::applyInlineMove(Board& board, const Move& move, Player player) {
    Direction dir = move.direction();
    const auto& marbles = move.marbles();

    Position front = findFrontMarble(marbles, dir);
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

        for (int i = opponentMarbles.size() - 2; i >= 0; i--) {
            Cell cell = board.get(opponentMarbles[i]);
            board.set(opponentMarbles[i+1], cell);
        }

        if (!opponentMarbles.empty()) {
            board.set(opponentMarbles[0], playerToCell(opponent(player)));
        }
    } else {
        Cell lastOpponentCell = board.get(opponentMarbles.back());
        board.set(afterOpponent, lastOpponentCell);

        for (int i = opponentMarbles.size() - 2; i >= 0; i--) {
            Cell cell = board.get(opponentMarbles[i]);
            board.set(opponentMarbles[i+1], cell);
        }
    }

    board.set(target, ourCell);

    for (const Position& marble : marbles) {
        if (marble != front) {
            Position pos = marble.neighbor(dir);
            board.set(pos, ourCell);
        }
    }

    for (const Position& marble : marbles) {
        Position behind = marble.neighbor(opposite(dir));
        bool hasBehind = std::find(marbles.begin(), marbles.end(), behind) != marbles.end();

        if (!hasBehind) {
            board.set(marble, Cell::Empty);
            break;
        }
    }
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