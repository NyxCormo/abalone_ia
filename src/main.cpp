#include <iostream>
#include "core/Board.h"

int main() {
    std::cout << "=== Test Board ===" << std::endl;

    Board board;
    board.setup();

    std::cout << board.toString() << std::endl;

    std::cout << "Billes noires: " << board.countMarbles(Player::BLACK) << std::endl;
    std::cout << "Billes blanches: " << board.countMarbles(Player::WHITE) << std::endl;

    std::cout << "\nPartie terminée? " << (board.isGameOver() ? "Oui" : "Non") << std::endl;

    return 0;
}