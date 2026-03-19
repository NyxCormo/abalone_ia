#ifndef ABALONE_BOARD_H
#define ABALONE_BOARD_H
#include <array>
#include <vector>
#include <string>

#include "../geometry/Position.h"

enum class Cell {
    Empty,
    Black,
    White,
};

enum class Player {
    Black,
    White,
};

inline Player opponent(Player p) {
    return p == Player::Black ? Player::White : Player::Black;
}

inline Cell playerToCell(Player p) {
    return p == Player::Black ? Cell::Black : Cell::White;
}

class Board {
    std::array<std::array<Cell, 9>, 9> cells_;
    int black_count_;
    int white_count_;
    int black_ejected_;
    int white_ejected_;

public:
    Board();
    void setup();

    [[nodiscard]] Cell get(const Position& pos) const;
    void set(const Position& pos, Cell cell);

    void setBlackCount(int count);
    void setWhiteCount(int count);

    [[nodiscard]] bool hasMarble(const Position& pos, Player player) const;
    [[nodiscard]] bool isEmpty(const Position& pos) const;

    void ejectBlack() { black_ejected_++; black_count_--; }
    void ejectWhite() { white_ejected_++; white_count_--; }

    [[nodiscard]] int blackEjected() const { return black_ejected_; }
    [[nodiscard]] int whiteEjected() const { return white_ejected_; }

    [[nodiscard]] bool isGameOver() const;
    [[nodiscard]] Player winner() const;

    [[nodiscard]] int countMarbles(Player player) const;
    [[nodiscard]] std::vector<Position> getMarblePositions(Player player) const;

    [[nodiscard]] std::string toString() const;

private:
    static int toIndex(const int coord) { return coord + 4; }
};


#endif //ABALONE_BOARD_H