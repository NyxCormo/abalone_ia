#ifndef ABALONE_GAME_H
#define ABALONE_GAME_H

#include <memory>

#include "core/InputHandler.h"
#include "render/Renderer.h"
#include <vector>

#include "core/GameState.h"

class Game {
public:
    Game();
    void run();

private:
    void handleInput();
    void update();
    void render() const;
    void reset();

    std::unique_ptr<Renderer> renderer_;
    InputHandler input_;

    bool showWheel_;
    Position wheelCenter_;
    std::vector<Direction> validDirections_;

    GameState state_;
    bool gameOver_;

    bool isDragging_;
};

#endif //ABALONE_GAME_H