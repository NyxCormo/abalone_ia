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
            DrawRectangle(0, 0, 1400, 900, {0, 0, 0, 150});
            DrawText("GAME OVER!", 600, 400, 48, RED);
            Player winner = Game_Rules::getWinner(board);
            DrawText(TextFormat("Winner: %s", winner == Player::Black ? "BLACK" : "WHITE"),
                     550, 460, 32, winner == Player::Black ? BLACK : WHITE);
            DrawText("Press R to restart", 550, 520, 24, WHITE);
        }

        DrawText("Click marble to select", 10, 180, 16, DARKGRAY);
        DrawText("SHIFT+Click for multi-select", 10, 200, 16, DARKGRAY);
        DrawText("R: Reset", 10, 220, 16, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}