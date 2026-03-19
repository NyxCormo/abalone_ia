#ifndef ABALONE_GAME_H
#define ABALONE_GAME_H

#include <memory>

#include "core/Board.h"
#include "core/InputHandler.h"
#include "render/Renderer.h"
#include <vector>

class Game {
public:
    Game();
    void run();

private:
    void handleInput();
    void update();
    void render();
    void reset();
    void drawUI();
    void drawGameOverScreen();

    Board board_;
    std::unique_ptr<Renderer> renderer_;
    InputHandler input_;

    Player currentPlayer_;
    bool showWheel_;
    Position wheelCenter_;
    std::vector<Direction> validDirections_;

    int moveCount_;
    bool gameOver_;
};

#endif //ABALONE_GAME_H