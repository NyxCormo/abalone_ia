//
// Created by nyx on 3/13/26.
//

#include "Board.h"
#include <sstream>
#include <iomanip>

Board::Board() : black_count_(0), white_count_(0) {
    for (auto& row : cells_) {
        row.fill(Cell::EMPTY);
    }
}

void Board::setup() {
    set(Position(-4, 4), Cell::BLACK);
    set(Position(-3, 4), Cell::BLACK);
    set(Position(-2, 4), Cell::BLACK);
    set(Position(-1, 4), Cell::BLACK);
    set(Position(0, 4), Cell::BLACK);

    set(Position(-4, 3), Cell::BLACK);
    set(Position(-3, 3), Cell::BLACK);
    set(Position(-2, 3), Cell::BLACK);
    set(Position(-1, 3), Cell::BLACK);
    set(Position(0, 3), Cell::BLACK);
    set(Position(1, 3), Cell::BLACK);

    set(Position(-2, 2), Cell::BLACK);
    set(Position(-1, 2), Cell::BLACK);
    set(Position(0, 2), Cell::BLACK);


    set(Position(0, -4), Cell::WHITE);
    set(Position(1, -4), Cell::WHITE);
    set(Position(2, -4), Cell::WHITE);
    set(Position(3, -4), Cell::WHITE);
    set(Position(4, -4), Cell::WHITE);

    set(Position(-1, -3), Cell::WHITE);
    set(Position(0, -3), Cell::WHITE);
    set(Position(1, -3), Cell::WHITE);
    set(Position(2, -3), Cell::WHITE);
    set(Position(3, -3), Cell::WHITE);
    set(Position(4, -3), Cell::WHITE);

    set(Position(0, -2), Cell::WHITE);
    set(Position(1, -2), Cell::WHITE);
    set(Position(2, -2), Cell::WHITE);

    setBlackCount(14);
    setWhiteCount(14);
}

void Board::setBlackCount(int count) {
    black_count_ = count;
}

void Board::setWhiteCount(int count) {
    white_count_ = count;
}

Cell Board::get(const Position& pos) const {
    if (!pos.isValid()) {
        return Cell::EMPTY;
    }
    return cells_[toIndex(pos.q())][toIndex(pos.r())];
}

void Board::set(const Position& pos, Cell cell) {
    if (pos.isValid()) {
        cells_[toIndex(pos.q())][toIndex(pos.r())] = cell;
    }
}

bool Board::hasMarble(const Position& pos, const Player player) const {
    Cell cell = get(pos);
    return (player == Player::BLACK && cell == Cell::BLACK) ||
           (player == Player::WHITE && cell == Cell::WHITE);
}

bool Board::isEmpty(const Position& pos) const {
    return get(pos) == Cell::EMPTY;
}

bool Board::isGameOver() const {
    return black_count_ == 8 || white_count_ == 8;
}

Player Board::winner() const {
    return black_count_ > white_count_ ? Player::BLACK : Player::WHITE;
}

int Board::countMarbles(const Player player) const {
    int count = 0;
    const Cell target = playerToCell(player);

    for (int q = -4; q <= 4; q++) {
        for (int r = -4; r <= 4; r++) {
            Position pos(q, r);
            if (pos.isValid() && get(pos) == target) {
                count++;
            }
        }
    }
    return count;
}

std::vector<Position> Board::getMarblePositions(const Player player) const {
    std::vector<Position> positions;
    const Cell target = playerToCell(player);

    for (int q = -4; q <= 4; q++) {
        for (int r = -4; r <= 4; r++) {
            Position pos(q, r);
            if (pos.isValid() && get(pos) == target) {
                positions.push_back(pos);
            }
        }
    }
    return positions;
}

std::string Board::toString() const {
    std::ostringstream oss;

    oss << "\n  Abalone Board\n";
    oss << "  Black: " << (black_count_) << " billes (éjectées: " << 14 - black_count_ << ")\n";
    oss << "  White: " << (white_count_) << " billes (éjectées: " << 14 - white_count_ << ")\n\n";

    // Affichage du plateau hexagonal
    // r va de -4 à +4, mais on affiche de haut en bas
    for (int r = -4; r <= 4; r++) {
        // Indentation pour forme hexagonale
        int indent = std::abs(r);
        oss << std::string(indent * 2, ' ');

        // Afficher les cellules de cette rangée
        int q_start = std::max(-4, -4 - r);
        int q_end = std::min(4, 4 - r);

        for (int q = q_start; q <= q_end; q++) {
            Position pos(q, r);
            Cell cell = get(pos);

            if (cell == Cell::BLACK) {
                oss << "● ";
            } else if (cell == Cell::WHITE) {
                oss << "○ ";
            } else {
                oss << "· ";
            }
        }
        oss << "\n";
    }

    return oss.str();
}
