#include "UI.h"

#include <raylib.h>

#include "game/GameEngine.h"

Texture2D UI::backgroundGameOverTexture_ = {};
Texture2D UI::panelTexture_ = {};
Texture2D UI::borderPanelTexture_ = {};
Texture2D UI::buttonTexture_ = {};

int UI::panelX_ = 0;
int UI::panelY_ = 0;
int UI::panelWidth_ = 0;
int UI::panelHeight_ = 0;

void UI::init() {
    backgroundGameOverTexture_ = LoadTexture("../assets/textures/whiteMarble.png");
    panelTexture_ = LoadTexture("../assets/textures/hexWood.png");
    borderPanelTexture_ = LoadTexture("../assets/textures/blackMarble.png");
    buttonTexture_ = LoadTexture("../assets/textures/tableTexture.png");
    
    panelX_ = 0;
    panelY_ = 0;
    panelWidth_ = 1400;
    panelHeight_ = 900;
}

void UI::unload() {
    UnloadTexture(backgroundGameOverTexture_);
}

void UI::drawHUD(const GameState& state) {
    DrawText("ABALONE", 10, 10, 24, BLACK);
    DrawText(TextFormat("Move: %d", state.moveCount), 10, 45, 20, DARKGRAY);
    DrawText(TextFormat("Current Player: %s",
             state.currentPlayer == Player::Black ? "BLACK" : "WHITE"), 10, 75, 20,
             state.currentPlayer == Player::Black ? BLACK : GRAY);
    DrawText(TextFormat("Black: %d (%d ejected)",
             state.board.countMarbles(Player::Black), state.board.blackEjected()), 10, 105, 18, BLACK);
    DrawText(TextFormat("White: %d (%d ejected)",
             state.board.countMarbles(Player::White), state.board.whiteEjected()), 10, 130, 18, DARKGRAY);

    DrawText("Click/Drag to select marbles", 10, 180, 16, DARKGRAY);
    DrawText("Right-click: Clear selection", 10, 200, 16, DARKGRAY);
    DrawText("R: Reset", 10, 220, 16, DARKGRAY);
    DrawText("LEFT/RIGHT: Rotate camera horizontally", 10, 240, 16, DARKGRAY);
    DrawText("UP/DOWN: Rotate camera vertically", 10, 260, 16, DARKGRAY);
}

void UI::drawGameOver(const GameState& state) {
    DrawTexturePro(
        backgroundGameOverTexture_,
        {0, 0, static_cast<float>(backgroundGameOverTexture_.width), static_cast<float>(backgroundGameOverTexture_.height)},
        {
            static_cast<float>(panelX_),
            static_cast<float>(panelY_),
            static_cast<float>(panelWidth_),
            static_cast<float>(panelHeight_)
        },
        {0, 0},
        0.0f,
        WHITE
    );

    if (backgroundGameOverTexture_.id == 0) {
        DrawRectangle(panelX_, panelY_, panelWidth_, panelHeight_, {100, 100, 100, 100});
    }

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;

    int panelWidth_ = 600;
    int panelHeight_ = 300;
    int panelX_ = centerX - panelWidth_ / 2;
    int panelY_ = centerY - panelHeight_ / 2;

    //DrawRectangle(panelX, panelY, panelWidth, panelHeight, {139, 90, 43, 255});
    //DrawRectangle(panelX + 10, panelY + 10, panelWidth - 20, panelHeight - 20, {101, 67, 33, 255});
    //DrawRectangle(panelX + 15, panelY + 15, panelWidth - 30, panelHeight - 30, {120, 81, 45, 255});


    DrawTexturePro(
        buttonTexture_,
        {0, 0, static_cast<float>(buttonTexture_.width), static_cast<float>(buttonTexture_.height)},
{
            static_cast<float>(panelX_),
            static_cast<float>(panelY_),
            static_cast<float>(panelWidth_),
            static_cast<float>(panelHeight_)
        },
        {0, 0},
        0.0f,
        WHITE
    );
    DrawTexturePro(
        panelTexture_,
        {0, 0, static_cast<float>(panelTexture_.width), static_cast<float>(panelTexture_.height)},
{
            static_cast<float>(panelX_+20),
            static_cast<float>(panelY_+20),
            static_cast<float>(panelWidth_-40),
            static_cast<float>(panelHeight_-40)
        },
        {0, 0},
        0.0f,
        WHITE
    );

    const char* gameOverText = "GAME OVER!";
    int gameOverWidth = MeasureText(gameOverText, 60);
    DrawText(gameOverText, centerX - gameOverWidth / 2 + 3, centerY - 100 + 3, 60, {0, 0, 0, 100});
    DrawText(gameOverText, centerX - gameOverWidth / 2, centerY - 100, 60, {220, 50, 50, 255});

    const char* outcomeLabel = GameEngine::isDraw(state) ? "Result:" : "Winner:";
    int outcomeLabelWidth = MeasureText(outcomeLabel, 40);
    DrawText(outcomeLabel, centerX - outcomeLabelWidth / 2 + 2, centerY - 20 + 2, 40, {0, 0, 0, 100});
    DrawText(outcomeLabel, centerX - outcomeLabelWidth / 2, centerY - 20, 40, {50, 200, 50, 255});

    const char* outcomeName = "DRAW";
    Color outcomeColor = {240, 240, 240, 255};
    if (!GameEngine::isDraw(state)) {
        const Player winner = state.winner.value();
        outcomeName = winner == Player::Black ? "BLACK" : "WHITE";
        outcomeColor = winner == Player::Black ? BLACK : Color{240, 240, 240, 255};
    }

    int outcomeNameWidth = MeasureText(outcomeName, 48);
    DrawText(outcomeName, centerX - outcomeNameWidth / 2 + 2, centerY + 30 + 2, 48, {0, 0, 0, 120});
    DrawText(outcomeName, centerX - outcomeNameWidth / 2, centerY + 30, 48, outcomeColor);

    const char* restartText = "Press R to restart";
    int restartWidth = MeasureText(restartText, 28);
    DrawText(restartText, centerX - restartWidth / 2 + 2, centerY + 100 + 2, 28, {0, 0, 0, 100});
    DrawText(restartText, centerX - restartWidth / 2, centerY + 100, 28, {200, 200, 200, 255});
}
