#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cctype>

#include "GameAppConsole.h"
#include "../game/GameEngine.h"

#define CLEAR_COMMAND "clear"

GameAppConsole::GameAppConsole() : state_() {
    state_.board.setup();
}

void GameAppConsole::run() {
    displayWelcome();

    bool running = true;
    while (running && !GameEngine::isGameOver(state_)) {
        displayBoard();
        displayStatus();

        running = processInput();
    }

    if (GameEngine::isGameOver(state_)) {
        displayBoard();
        displayGameOver();
    } else {
        std::cout << "\n=== Merci d'avoir joué ! ===\n";
    }
}

void GameAppConsole::displayBoard() const {
    clearScreen();

    std::cout << "\n=== PLATEAU ABALONE ===\n\n";

    for (int i = 0; i < 9; i++) {
        constexpr char rowLabels[] = {'I','H','G','F','E','D','C','B','A'};
        constexpr int rows[] = {-4,-3,-2,-1,0,1,2,3,4};
        constexpr int MAX_CELLS = 9;
        int r = rows[i];
        int qMin = std::max(-4, -4 - r);
        int qMax = std::min(4, 4 - r);
        int cellCount = qMax - qMin + 1;

        int padding = (MAX_CELLS - cellCount);

        std::cout << rowLabels[i] << " ";
        for (int s = 0; s < padding; s++) std::cout << " ";

        for (int q = qMin; q <= qMax; q++) {
            Position pos(q, r);
            char cell = getCellChar(pos);
            char symbol = (cell == 'W') ? 'O' : (cell == 'B') ? 'X' : '.';
            std::cout << symbol << " ";
        }
        std::cout << "\n";
    }

    std::cout << "  ";
    for (int col = 1; col <= 9; col++) {
        std::cout << col << " ";
    }
    std::cout << "\n\n";
}

void GameAppConsole::displayStatus() const {
    std::cout << "╔════════════════════════════════════════════╗\n";
    std::cout << "║ STATUT DU JEU                              ║\n";
    std::cout << "╠════════════════════════════════════════════╣\n";

    std::string currentPlayerStr = (state_.currentPlayer == Player::Black) ? "NOIR ⚪" : "BLANC ⚫";
    std::cout << "║ Joueur actuel : " << std::left << std::setw(23) << currentPlayerStr << "║\n";

    int blackScore = 14 - state_.board.whiteEjected();
    int whiteScore = 14 - state_.board.blackEjected();

    std::cout << "║ Score NOIR    : " << std::setw(27) << blackScore << "║\n";
    std::cout << "║ Score BLANC   : " << std::setw(27) << whiteScore << "║\n";
    std::cout << "║ Coups joués   : " << std::setw(27) << state_.moveCount << "║\n";

    std::ostringstream oss;
    oss << "Billes éjectées B:" << state_.board.blackEjected()
        << " / W:" << state_.board.whiteEjected();

    std::cout << "║ " << std::left << std::setw(42) << oss.str() << "║\n";
    std::cout << "╚════════════════════════════════════════════╝\n\n";
}

void GameAppConsole::displayHelp() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════╗\n";
    std::cout << "║ AIDE - COMMANDES                           ║\n";
    std::cout << "╠════════════════════════════════════════════╣\n";
    std::cout << "║ Format coup :                              ║\n";
    std::cout << "║   1 bille   : A1 E    (pos + direction)    ║\n";
    std::cout << "║   2 billes  : A1-B1 E (pos-pos + dir)      ║\n";
    std::cout << "║   3 billes  : A1-C1 E (pos-pos + dir)      ║\n";
    std::cout << "║                                            ║\n";
    std::cout << "║ Directions : E, W, NE, NW, SE, SW          ║\n";
    std::cout << "║                                            ║\n";
    std::cout << "║ Commandes :                                ║\n";
    std::cout << "║   /help  - Afficher cette aide             ║\n";
    std::cout << "║   /moves - Afficher tous les coups légaux  ║\n";
    std::cout << "║   /quit  - Quitter le jeu                  ║\n";
    std::cout << "╚════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

void GameAppConsole::displayGameOver() const {
    clearScreen();
    std::cout << "╔════════════════════════════════════════════╗\n";
    std::cout << "║           PARTIE TERMINÉE !                ║\n";
    std::cout << "╠════════════════════════════════════════════╣\n";

    if (state_.winner.has_value()) {
        Player winner = state_.winner.value();
        std::string winnerStr = (winner == Player::Black) ? "NOIR ⚪" : "BLANC ⚫";
        std::cout << "║ Vainqueur   : " << std::left << std::setw(27) << winnerStr << "║\n";
    }

    int blackScore = 14 - state_.board.whiteEjected();
    int whiteScore = 14 - state_.board.blackEjected();

    std::cout << "║ Score final :                              ║\n";
    std::cout << "║   NOIR  : " << std::setw(31) << blackScore << "║\n";
    std::cout << "║   BLANC : " << std::setw(31) << whiteScore << "║\n";
    std::cout << "║ Coups joués : " << std::setw(27) << state_.moveCount << "║\n";
    std::cout << "╚════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

void GameAppConsole::displayWelcome() {
    clearScreen();
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════╗\n";
    std::cout << "║                                            ║\n";
    std::cout << "║            ABALONE - CONSOLE               ║\n";
    std::cout << "║                                            ║\n";
    std::cout << "╚════════════════════════════════════════════╝\n\n";
    std::cout << "Tapez '/help' pour afficher l'aide.\n";
    std::cout << "Appuyez sur Entrée pour commencer...\n";
    std::cin.get();
}

bool GameAppConsole::processInput() {
    std::cout << "Coup (ou '/help' ou '/moves' ou '/quit') > ";

    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) {
        return true;
    }

    std::transform(input.begin(), input.end(), input.begin(), ::tolower);

    if (input == "/quit") {
        return false;
    }

    if (input == "/help") {
        displayHelp();
        std::cout << "Appuyez sur Entrée pour continuer...";
        std::cin.get();
        return true;
    }

    if (input == "/moves") {
        std::vector<Move> legalMoves = GameEngine::getLegalMoves(state_);

        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════════╗\n";
        std::cout << "║ COUPS LÉGAUX (" << std::setw(2) << legalMoves.size() << " disponibles)             ║\n";
        std::cout << "╚════════════════════════════════════════════╝\n\n";

        int count = 0;
        for (const Move& move : legalMoves) {
            std::cout << std::setw(3) << (++count) << ". ";

            if (move.marbleCount() == 1) {
                std::cout << positionToNotation(move.marbles()[0]);
            } else {
                std::cout << positionToNotation(move.marbles()[0]) << "-"
                         << positionToNotation(move.marbles()[move.marbleCount() - 1]);
            }

            std::cout << " " << directionToString(move.direction());

            if ((count % 4) == 0) {
                std::cout << "\n";
            } else {
                std::cout << "\t";
            }
        }

        std::cout << "\n\nAppuyez sur Entrée pour continuer...";
        std::cin.get();
        return true;
    }

    auto move = parseMove(input);

    if (!move.has_value() || !executeMove(move.value())) {
        std::cout << "\nNope, coup invalide ! \n";
        std::cout << "Appuyez sur Entrée pour continuer...";
        std::cin.get();
        return true;
    }

    return true;
}

std::optional<Move> GameAppConsole::parseMove(const std::string& input) {
    std::istringstream iss(input);
    std::string positions, directionStr;

    iss >> positions >> directionStr;

    if (positions.empty() || directionStr.empty()) {
        return std::nullopt;
    }

    auto direction = parseDirection(directionStr);
    if (!direction.has_value()) {
        return std::nullopt;
    }

    size_t dashPos = positions.find('-');

    if (dashPos == std::string::npos) {
        auto pos = parsePosition(positions);
        if (!pos.has_value()) {
            return std::nullopt;
        }
        return Move(pos.value(), direction.value());
    } else {
        std::string pos1Str = positions.substr(0, dashPos);
        std::string pos2Str = positions.substr(dashPos + 1);

        auto pos1 = parsePosition(pos1Str);
        auto pos2 = parsePosition(pos2Str);

        if (!pos1.has_value() || !pos2.has_value()) {
            return std::nullopt;
        }

        std::vector<Position> marbles;

        int distance = pos1->distance(*pos2);

        if (distance == 1) {
            marbles = {pos1.value(), pos2.value()};
        } else if (distance == 2) {
            for (Direction testDir : ALL_DIRECTIONS) {
                Position mid = pos1->neighbor(testDir);
                if (mid == *pos2) continue;

                Position far = mid.neighbor(testDir);
                if (far == *pos2) {
                    marbles = {pos1.value(), mid, pos2.value()};
                    break;
                }
            }

            if (marbles.empty()) {
                for (Direction testDir : ALL_DIRECTIONS) {
                    Position mid = pos2->neighbor(testDir);
                    if (mid == *pos1) continue;

                    Position far = mid.neighbor(testDir);
                    if (far == *pos1) {
                        marbles = {pos2.value(), mid, pos1.value()};
                        break;
                    }
                }
            }
        }

        if (marbles.empty()) {
            return std::nullopt;
        }

        return Move(marbles, direction.value());
    }
}

std::optional<Position> GameAppConsole::parsePosition(const std::string& posStr) {
    if (posStr.length() != 2) {
        return std::nullopt;
    }

    char rowChar = std::toupper(posStr[0]);
    char colChar = posStr[1];

    if (rowChar < 'A' || rowChar > 'I') {
        return std::nullopt;
    }

    if (colChar < '1' || colChar > '9') {
        return std::nullopt;
    }

    int rowIndex = 'I' - rowChar;
    int r = -4 + rowIndex;

    int col = colChar - '1';

    int qMin = std::max(-4, -4 - r);
    int q = qMin + col;

    Position pos(q, r);

    if (!pos.isValid()) {
        return std::nullopt;
    }

    return pos;
}

std::optional<Direction> GameAppConsole::parseDirection(const std::string& dirStr) {
    std::string upper = dirStr;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

    if (upper == "E" || upper == "EAST") return Direction::EAST;
    if (upper == "W" || upper == "WEST") return Direction::WEST;
    if (upper == "NE" || upper == "NORTHEAST") return Direction::NORTH_EAST;
    if (upper == "NW" || upper == "NORTHWEST") return Direction::NORTH_WEST;
    if (upper == "SE" || upper == "SOUTHEAST") return Direction::SOUTH_EAST;
    if (upper == "SW" || upper == "SOUTHWEST") return Direction::SOUTH_WEST;

    return std::nullopt;
}

std::string GameAppConsole::positionToNotation(const Position& pos) {
    int rowIndex = -4 - pos.r();
    char rowChar = 'I' + rowIndex;

    int qMin = std::max(-4, -4 - pos.r());
    int col = pos.q() - qMin;
    char colChar = '1' + col;

    return std::string(1, rowChar) + colChar;
}

std::string GameAppConsole::directionToString(Direction dir) {
    switch (dir) {
        case Direction::EAST: return "E";
        case Direction::WEST: return "W";
        case Direction::NORTH_EAST: return "NE";
        case Direction::NORTH_WEST: return "NW";
        case Direction::SOUTH_EAST: return "SE";
        case Direction::SOUTH_WEST: return "SW";
        default: return "?";
    }
}

char GameAppConsole::getCellChar(const Position& pos) const {
    Cell cell = state_.board.get(pos);

    if (cell == Cell::Black) return 'B';
    if (cell == Cell::White) return 'W';
    return '.';
}

void GameAppConsole::clearScreen() {
    std::cout << std::flush;
    system("clear");
}

bool GameAppConsole::executeMove(const Move& move) {
    std::vector<Move> legalMoves = GameEngine::getLegalMoves(state_);

    bool isLegal = false;
    for (const Move& legalMove : legalMoves) {
        if (legalMove == move) {
            isLegal = true;
            break;
        }
    }

    if (!isLegal) {
        return false;
    }

    state_ = GameEngine::applyMove(state_, move);

    return true;
}

char GameAppConsole::getRowLetter(int r) {
    int rowIndex = -4 - r;
    return 'I' + rowIndex;
}

int GameAppConsole::getColumnNumber(const Position& pos) {
    int qMin = std::max(-4, -4 - pos.r());
    return pos.q() - qMin + 1;
}