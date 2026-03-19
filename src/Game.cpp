#include "Game.h"
#include "core/MoveGenerator.h"
#include "core/Game_Rules.h"
#include <raylib.h>
#include <iostream>

Game::Game()
    : board_{},
      renderer_{nullptr},
      input_{},
      currentPlayer_{Player::Black},
      showWheel_{false},
      wheelCenter_{},
      validDirections_{},
      moveCount_{0},
      gameOver_{false}
{
    board_.setup();

    std::cout << "=== ABALONE INTERACTIVE ===" << std::endl;
    std::cout << "Click on your marbles to select (up to 3 aligned)" << std::endl;
    std::cout << "Click on direction wheel to move" << std::endl;
    std::cout << "R: Reset game" << std::endl;
}

void Game::run() {
    InitWindow(1400, 900, "Abalone 3D - Interactive");
    SetTargetFPS(60);

    renderer_ = std::make_unique<Renderer>();

    while (!WindowShouldClose()) {
        handleInput();
        update();
        render();
    }

    CloseWindow();
}

void Game::handleInput() {
    if (IsKeyPressed(KEY_R)) {
        reset();
        return;
    }

    if (gameOver_ || !IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return;
    }

    Vector2 mousePos = GetMousePosition();

    if (showWheel_) {
        auto clickedDir = renderer_->getClickedDirection(wheelCenter_, mousePos);

        if (clickedDir.has_value()) {
            auto move = input_.tryCreateMove(clickedDir.value(), board_, currentPlayer_);

            if (move.has_value()) {
                std::cout << "\n[Move " << (moveCount_ + 1) << "] "
                          << (currentPlayer_ == Player::Black ? "BLACK" : "WHITE")
                          << " plays: " << move->toString() << std::endl;

                Game_Rules::applyMove(board_, move.value(), currentPlayer_);
                moveCount_++;

                if (Game_Rules::isGameOver(board_)) {
                    gameOver_ = true;
                    Player winner = Game_Rules::getWinner(board_);
                    std::cout << "\n=== GAME OVER ===" << std::endl;
                    std::cout << "Winner: " << (winner == Player::Black ? "BLACK" : "WHITE") << std::endl;
                } else {
                    currentPlayer_ = opponent(currentPlayer_);
                }

                input_.clearSelection();
                showWheel_ = false;
            }
        } else {
            showWheel_ = false;
        }
    } else {
        auto clickedHex = renderer_->getClickedHex(mousePos);

        if (clickedHex.has_value()) {
            Position pos = clickedHex.value();

            if (board_.hasMarble(pos, currentPlayer_)) {
                if (IsKeyDown(KEY_LEFT_SHIFT)) {
                    input_.addToSelection(pos, currentPlayer_, board_);
                } else {
                    input_.selectHex(pos, currentPlayer_, board_);
                }

                if (input_.hasSelection()) {
                    validDirections_ = input_.getValidDirections(board_, currentPlayer_);

                    if (!validDirections_.empty()) {
                        showWheel_ = true;
                        wheelCenter_ = input_.getSelectedMarbles()[0];
                    }
                }
            }
        }
    }
}

void Game::update() {
    // TODO
}

void Game::render() {
    BeginDrawing();
    ClearBackground({200, 200, 200, 255});

    renderer_->draw(board_);

    if (input_.hasSelection()) {
        for (const auto& pos : input_.getSelectedMarbles()) {
            Vector3 worldPos = {
                1.2f * std::sqrt(3.0f) * (pos.q() + pos.r()/2.0f),
                0.0f,
                1.2f * 1.5f * pos.r()
            };
            worldPos.y = 1.0f;

            Vector2 screenPos = GetWorldToScreen(worldPos, renderer_->camera);
            DrawCircleLines(screenPos.x, screenPos.y, 35, YELLOW);
            DrawCircleLines(screenPos.x, screenPos.y, 36, YELLOW);
        }
    }

    if (showWheel_) {
        renderer_->drawSelectionWheel(wheelCenter_, validDirections_);
    }

    drawUI();

    if (gameOver_) {
        drawGameOverScreen();
    }

    EndDrawing();
}

void Game::reset() {
    board_.setup();
    currentPlayer_ = Player::Black;
    input_.clearSelection();
    showWheel_ = false;
    moveCount_ = 0;
    gameOver_ = false;
    std::cout << "\n=== GAME RESET ===" << std::endl;
}

void Game::drawUI() {
    DrawText("ABALONE 3D - INTERACTIVE", 10, 10, 24, BLACK);
    DrawText(TextFormat("Move: %d", moveCount_), 10, 45, 20, DARKGRAY);
    DrawText(TextFormat("Current Player: %s",
             currentPlayer_ == Player::Black ? "BLACK" : "WHITE"), 10, 75, 20,
             currentPlayer_ == Player::Black ? BLACK : GRAY);
    DrawText(TextFormat("Black: %d (%d ejected)",
             board_.countMarbles(Player::Black), board_.blackEjected()), 10, 105, 18, BLACK);
    DrawText(TextFormat("White: %d (%d ejected)",
             board_.countMarbles(Player::White), board_.whiteEjected()), 10, 130, 18, DARKGRAY);

    DrawText("Click marble to select", 10, 180, 16, DARKGRAY);
    DrawText("SHIFT+Click for multi-select", 10, 200, 16, DARKGRAY);
    DrawText("R: Reset", 10, 220, 16, DARKGRAY);
    DrawText("LEFT/RIGHT: Rotate camera horizontally", 10, 240, 16, DARKGRAY);
    DrawText("UP/DOWN: Rotate camera vertically", 10, 260, 16, DARKGRAY);
}

void Game::drawGameOverScreen() {
    DrawRectangle(0, 0, 1400, 900, {0, 0, 0, 180});
    
    Player winner = Game_Rules::getWinner(board_);
    
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