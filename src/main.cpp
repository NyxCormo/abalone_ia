#include <iostream>
#include <raylib.h>

#include "core/Board.h"
#include "core/MoveGenerator.h"
#include "core/Game_Rules.h"
#include "core/InputHandler.h"
#include "render/Renderer.h"

int main() {
    InitWindow(1400, 900, "Abalone 3D - Interactive");
    SetTargetFPS(60);

    Board board;
    board.setup();

    Player currentPlayer = Player::Black;
    Renderer renderer;
    InputHandler input;

    bool showWheel = false;
    Position wheelCenter;
    std::vector<Direction> validDirections;

    int moveCount = 0;
    bool gameOver = false;

    std::cout << "=== ABALONE INTERACTIVE ===" << std::endl;
    std::cout << "Click on your marbles to select (up to 3 aligned)" << std::endl;
    std::cout << "Click on direction wheel to move" << std::endl;
    std::cout << "R: Reset game" << std::endl;

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_R)) {
            board.setup();
            currentPlayer = Player::Black;
            input.clearSelection();
            showWheel = false;
            moveCount = 0;
            gameOver = false;
            std::cout << "\n=== GAME RESET ===" << std::endl;
        }

        if (!gameOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();

            if (showWheel) {
                auto clickedDir = renderer.getClickedDirection(wheelCenter, mousePos);

                if (clickedDir.has_value()) {
                    auto move = input.tryCreateMove(clickedDir.value(), board, currentPlayer);

                    if (move.has_value()) {
                        std::cout << "\n[Move " << (moveCount + 1) << "] "
                                  << (currentPlayer == Player::Black ? "BLACK" : "WHITE")
                                  << " plays: " << move->toString() << std::endl;

                        Game_Rules::applyMove(board, move.value(), currentPlayer);
                        moveCount++;

                        if (Game_Rules::isGameOver(board)) {
                            gameOver = true;
                            Player winner = Game_Rules::getWinner(board);
                            std::cout << "\n=== GAME OVER ===" << std::endl;
                            std::cout << "Winner: " << (winner == Player::Black ? "BLACK" : "WHITE") << std::endl;
                        } else {
                            currentPlayer = opponent(currentPlayer);
                        }

                        input.clearSelection();
                        showWheel = false;
                    }
                } else {
                    showWheel = false;
                }
            } else {
                auto clickedHex = renderer.getClickedHex(mousePos);

                if (clickedHex.has_value()) {
                    Position pos = clickedHex.value();

                    if (board.hasMarble(pos, currentPlayer)) {
                        if (IsKeyDown(KEY_LEFT_SHIFT)) {
                            input.addToSelection(pos, currentPlayer, board);
                        } else {
                            input.selectHex(pos, currentPlayer, board);
                        }

                        if (input.hasSelection()) {
                            validDirections = input.getValidDirections(board, currentPlayer);

                            if (!validDirections.empty()) {
                                showWheel = true;
                                wheelCenter = input.getSelectedMarbles()[0];
                            }
                        }
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground({200, 200, 200, 255});

        renderer.draw(board);

        if (input.hasSelection()) {
            for (const auto& pos : input.getSelectedMarbles()) {
                Vector3 worldPos = {
                    1.2f * std::sqrt(3.0f) * (pos.q() + pos.r()/2.0f),
                    0.0f,
                    1.2f * 1.5f * pos.r()
                };
                worldPos.y = 1.0f;

                Vector2 screenPos = GetWorldToScreen(worldPos, renderer.camera);
                DrawCircleLines(screenPos.x, screenPos.y, 35, YELLOW);
                DrawCircleLines(screenPos.x, screenPos.y, 36, YELLOW);
            }
        }

        if (showWheel) {
            renderer.drawSelectionWheel(wheelCenter, validDirections);
        }

        DrawText("ABALONE 3D - INTERACTIVE", 10, 10, 24, BLACK);
        DrawText(TextFormat("Move: %d", moveCount), 10, 45, 20, DARKGRAY);
        DrawText(TextFormat("Current Player: %s",
                 currentPlayer == Player::Black ? "BLACK" : "WHITE"), 10, 75, 20,
                 currentPlayer == Player::Black ? BLACK : GRAY);
        DrawText(TextFormat("Black: %d (%d ejected)",
                 board.countMarbles(Player::Black), board.blackEjected()), 10, 105, 18, BLACK);
        DrawText(TextFormat("White: %d (%d ejected)",
                 board.countMarbles(Player::White), board.whiteEjected()), 10, 130, 18, DARKGRAY);

        if (gameOver) {
            DrawRectangle(0, 0, 1400, 900, {0, 0, 0, 180});

            Player winner = Game_Rules::getWinner(board);

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
            DrawText(gameOverText, centerX - gameOverWidth / 2 + 3, centerY - 100 + 3, 60, {0, 0, 0, 100}); // Shadow
            DrawText(gameOverText, centerX - gameOverWidth / 2, centerY - 100, 60, {220, 50, 50, 255});

            const char* winnerLabel = "Winner:";
            int winnerLabelWidth = MeasureText(winnerLabel, 40);
            DrawText(winnerLabel, centerX - winnerLabelWidth / 2 + 2, centerY - 20 + 2, 40, {0, 0, 0, 100}); // Shadow
            DrawText(winnerLabel, centerX - winnerLabelWidth / 2, centerY - 20, 40, {50, 200, 50, 255});

            const char* winnerName = winner == Player::Black ? "BLACK" : "WHITE";
            Color winnerColor = winner == Player::Black ? BLACK : Color{240, 240, 240, 255};
            int winnerNameWidth = MeasureText(winnerName, 48);
            DrawText(winnerName, centerX - winnerNameWidth / 2 + 2, centerY + 30 + 2, 48, {0, 0, 0, 120}); // Shadow
            DrawText(winnerName, centerX - winnerNameWidth / 2, centerY + 30, 48, winnerColor);

            const char* restartText = "Press R to restart";
            int restartWidth = MeasureText(restartText, 28);
            DrawText(restartText, centerX - restartWidth / 2 + 2, centerY + 100 + 2, 28, {0, 0, 0, 100}); // Shadow
            DrawText(restartText, centerX - restartWidth / 2, centerY + 100, 28, {200, 200, 200, 255});
        }

        DrawText("Click marble to select", 10, 180, 16, DARKGRAY);
        DrawText("SHIFT+Click for multi-select", 10, 200, 16, DARKGRAY);
        DrawText("R: Reset", 10, 220, 16, DARKGRAY);
        DrawText("LEFT/RIGHT: Rotate camera horizontally", 10, 240, 16, DARKGRAY);
        DrawText("UP/DOWN: Rotate camera vertically", 10, 260, 16, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}