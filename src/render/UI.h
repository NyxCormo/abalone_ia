#ifndef ABALONE_UI_H
#define ABALONE_UI_H

#include <raylib.h>

#include "../game/GameState.h"

class UI {
private:

    static Texture2D backgroundGameOverTexture_;
    static Texture2D panelTexture_;
    static Texture2D borderPanelTexture_;
    static Texture2D buttonTexture_;


    static int panelX_;
    static int panelY_;
    static int panelWidth_;
    static int panelHeight_;

public:
    static void init();
    static void unload();

    static void drawHUD(const GameState& state);
    static void drawGameOver(const GameState& state);
};

#endif //ABALONE_UI_H