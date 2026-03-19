#ifndef ABALONE_UI_H
#define ABALONE_UI_H

#include "../core/GameState.h"

class UI {
public:
    static void drawHUD(const GameState& state);
    static void drawGameOver(const GameState& state);
};

#endif //ABALONE_UI_H