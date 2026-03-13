//
// Created by nyx on 3/13/26.
//

#ifndef ABALONE_BOARD_H
#define ABALONE_BOARD_H
#include <array>
#include <vector>

#include "Position.h"

enum class Cell {
    EMPTY,
    BLACK,
    WHITE,
};

enum class Player {
    BLACK,
    WHITE,
};

inline Player opponent(Player p) {
    return p == Player::BLACK ? Player::WHITE : Player::BLACK;
}

inline Cell playerToCell(Player p) {
    return p == Player::BLACK ? Cell::BLACK : Cell::WHITE;
}

class Board {

    std::array<std::array<Cell, 9>, 9> cells_;

    int black_count_;
    int white_count_;

public:
    Board();

    void setup();

    Cell get(const Position& pos) const;
    void set(const Position& pos, Cell cell);

    void setBlackCount(int count);
    void setWhiteCount(int count);

    bool hasMarble(const Position& pos, Player player) const;

    bool isEmpty(const Position& pos) const;

    int ejectMarble(Player player);

    bool isGameOver() const;
    Player winner() const;

    int countMarbles(Player player) const ;

    std::vector<Position> getMarblePositions(Player player) const;

    std::string toString() const;

private:

    static int toIndex(const int coord) { return coord + 4; }

};


#endif //ABALONE_BOARD_H