#ifndef ABALONE_GAMEAPPCONSOLE_H
#define ABALONE_GAMEAPPCONSOLE_H

#include "../game/GameState.h"
#include "../game/Move.h"
#include <string>
#include <optional>

class GameAppConsole {
private:

    GameState state_;

public:

    GameAppConsole();
    ~GameAppConsole() = default;

    void run();

private:

    void displayBoard() const;
    void displayStatus() const;
    static void displayHelp() ;
    void displayGameOver() const;
    static void displayWelcome() ;

    bool processInput();
    static std::optional<Move> parseMove(const std::string& input);
    [[nodiscard]] static std::optional<Position> parsePosition(const std::string& posStr) ;
    [[nodiscard]] static std::optional<Direction> parseDirection(const std::string& dirStr) ;

    [[nodiscard]] static std::string positionToNotation(const Position& pos) ;
    [[nodiscard]] static std::string directionToString(Direction dir) ;
    [[nodiscard]] char getCellChar(const Position& pos) const;

    static void clearScreen() ;
    bool executeMove(const Move& move);

    [[nodiscard]] static char getRowLetter(int r) ;
    [[nodiscard]] static int getColumnNumber(const Position& pos) ;
};

#endif //ABALONE_GAMEAPPCONSOLE_H