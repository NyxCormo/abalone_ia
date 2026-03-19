#include "UI.h"

#include <raylib.h>

#include "game/GameEngine.h"

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
    DrawRectangle(0, 0, 1400, 900, {0, 0, 0, 180});

    Player winner = state.winner.value();

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;

    int panelWidth = 600;
    int panelHeight = 300;
    int panelX = centerX - panelWidth / 2;
    int panelY = centerY - panelHeight / 2;

    DrawRectangle(panelX, panelY, panelWidth, panelHeight, {139, 90, 43, 255});
    DrawRectangle(panelX + 10, panelY + 10, panelWidth - 20, panelHeight - 20, {101, 67, 33, 255});
    DrawRectangle(panelX + 15, panelY + 15, panelWidth - 30, panelHeight - 30, {120, 81, 45, 255});

    const char* gameOverText = "GAME OVER!";
    int gameOverWidth = MeasureText(gameOverText, 60);
    DrawText(gameOverText, centerX - gameOverWidth / 2 + 3, centerY - 100 + 3, 60, {0, 0, 0, 100});
    DrawText(gameOverText, centerX - gameOverWidth / 2, centerY - 100, 60, {220, 50, 50, 255});

    const char* winnerLabel = "Winner:";
    int winnerLabelWidth = MeasureText(winnerLabel, 40);
    DrawText(winnerLabel, centerX - winnerLabelWidth / 2 + 2, centerY - 20 + 2, 40, {0, 0, 0, 100});
    DrawText(winnerLabel, centerX - winnerLabelWidth / 2, centerY - 20, 40, {50, 200, 50, 255});

    const char* winnerName = winner == Player::Black ? "BLACK" : "WHITE";
    Color winnerColor = winner == Player::Black ? BLACK : Color{240, 240, 240, 255};
    int winnerNameWidth = MeasureText(winnerName, 48);
    DrawText(winnerName, centerX - winnerNameWidth / 2 + 2, centerY + 30 + 2, 48, {0, 0, 0, 120});
    DrawText(winnerName, centerX - winnerNameWidth / 2, centerY + 30, 48, winnerColor);

    const char* restartText = "Press R to restart";
    int restartWidth = MeasureText(restartText, 28);
    DrawText(restartText, centerX - restartWidth / 2 + 2, centerY + 100 + 2, 28, {0, 0, 0, 100});
    DrawText(restartText, centerX - restartWidth / 2, centerY + 100, 28, {200, 200, 200, 255});
}