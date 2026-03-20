#ifndef ABALONE_GAME_H
#define ABALONE_GAME_H

#include <memory>

#include "../game/InputHandler.h"
#include "../render/Renderer.h"
#include <vector>

#include "../game/GameState.h"

class GameApp3D {
public:
    GameApp3D();
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