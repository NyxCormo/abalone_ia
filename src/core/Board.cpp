#include "Board.h"
#include <sstream>

Board::Board() : cells_{}, black_count_(0), white_count_(0), black_ejected_(0), white_ejected_(0) {
    for (auto& row : cells_) {
        row.fill(Cell::Empty);
    }
}

void Board::setup() {
    set(Position(-4, 4), Cell::Black);
    set(Position(-3, 4), Cell::Black);
    set(Position(-2, 4), Cell::Black);
    set(Position(-1, 4), Cell::Black);
    set(Position(0, 4), Cell::Black);

    set(Position(-4, 3), Cell::Black);
    set(Position(-3, 3), Cell::Black);
    set(Position(-2, 3), Cell::Black);
    set(Position(-1, 3), Cell::Black);
    set(Position(0, 3), Cell::Black);
    set(Position(1, 3), Cell::Black);

    set(Position(-2, 2), Cell::Black);
    set(Position(-1, 2), Cell::Black);
    set(Position(0, 2), Cell::Black);

    set(Position(0, -4), Cell::White);
    set(Position(1, -4), Cell::White);
    set(Position(2, -4), Cell::White);
    set(Position(3, -4), Cell::White);
    set(Position(4, -4), Cell::White);

    set(Position(-1, -3), Cell::White);
    set(Position(0, -3), Cell::White);
    set(Position(1, -3), Cell::White);
    set(Position(2, -3), Cell::White);
    set(Position(3, -3), Cell::White);
    set(Position(4, -3), Cell::White);

    set(Position(0, -2), Cell::White);
    set(Position(1, -2), Cell::White);
    set(Position(2, -2), Cell::White);

    black_count_ = 14;
    white_count_ = 14;
    black_ejected_ = 0;
    white_ejected_ = 0;
}

void Board::setBlackCount(int count) {
    black_count_ = count;
}

void Board::setWhiteCount(int count) {
    white_count_ = count;
}

Cell Board::get(const Position& pos) const {
    if (!pos.isValid()) {
        return Cell::Empty;
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
    return (player == Player::Black && cell == Cell::Black) ||
           (player == Player::White && cell == Cell::White);
}

bool Board::isEmpty(const Position& pos) const {
    return get(pos) == Cell::Empty;
}

bool Board::isGameOver() const {
    return black_ejected_ >= 6 || white_ejected_ >= 6;
}

Player Board::winner() const {
    if (black_ejected_ >= 6) return Player::White;
    if (white_ejected_ >= 6) return Player::Black;
    return black_count_ > white_count_ ? Player::Black : Player::White;
}

int Board::countMarbles(const Player player) const {
    return player == Player::Black ? black_count_ : white_count_;
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
    oss << "  Black: " << black_count_ << " billes (éjectées: " << black_ejected_ << ")\n";
    oss << "  White: " << white_count_ << " billes (éjectées: " << white_ejected_ << ")\n\n";

    for (int r = -4; r <= 4; r++) {
        int indent = std::abs(r);
        oss << std::string(indent * 2, ' ');

        int q_start = std::max(-4, -4 - r);
        int q_end = std::min(4, 4 - r);

        for (int q = q_start; q <= q_end; q++) {
            Position pos(q, r);
            Cell cell = get(pos);

            if (cell == Cell::Black) {
                oss << "● ";
            } else if (cell == Cell::White) {
                oss << "○ ";
            } else {
                oss << "· ";
            }
        }
        oss << "\n";
    }

    return oss.str();
}