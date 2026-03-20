#include "GameApp.h"
#include <raylib.h>
#include <iostream>

#include "../game/GameEngine.h"
#include "../render/UI.h"

GameApp::GameApp()
    : renderer_{nullptr},
      input_{},
      showWheel_{false},
      wheelCenter_{},
      validDirections_{},
      state_{},
      gameOver_{false},
      isDragging_{false}
{
    state_.board.setup();
}

void GameApp::run() {
    InitWindow(1400, 900, "Abalone");
    SetTargetFPS(60);

    renderer_ = std::make_unique<Renderer>();

    while (!WindowShouldClose()) {
        handleInput();
        update();
        render();
    }

    CloseWindow();
}

void GameApp::handleInput() {
    if (IsKeyPressed(KEY_R)) {
        reset();
        return;
    }

    if (gameOver_) {
        return;
    }

    Vector2 mousePos = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        input_.clearSelection();
        showWheel_ = false;
        isDragging_ = false;
        return;
    }

    if (showWheel_ && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        auto clickedDir = renderer_->getClickedDirection(wheelCenter_, mousePos);

        if (clickedDir.has_value()) {
            auto move = input_.tryCreateMove(clickedDir.value(), state_.board, state_.currentPlayer);

            if (move.has_value()) {
                std::cout << "\n[Move " << (state_.moveCount + 1) << "] "
                          << (state_.currentPlayer == Player::Black ? "BLACK" : "WHITE")
                          << " plays: " << move->toString() << std::endl;

                state_ = GameEngine::applyMove(state_, move.value());

                if (GameEngine::isGameOver(state_)) {
                    gameOver_ = true;
                    Player winner = state_.winner.value();
                    std::cout << "\n=== GAME OVER ===" << std::endl;
                    std::cout << "Winner: " << (winner == Player::Black ? "BLACK" : "WHITE") << std::endl;
                }

                input_.clearSelection();
                showWheel_ = false;
            }
        } else {
            showWheel_ = false;
        }
        return;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !showWheel_) {
        isDragging_ = true;

        auto clickedHex = renderer_->getClickedHex(mousePos);
        if (clickedHex.has_value() && state_.board.hasMarble(clickedHex.value(), state_.currentPlayer)) {
            input_.addToSelection(clickedHex.value(), state_.currentPlayer, state_.board);
        }
    }

    if (isDragging_ && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        auto hoveredHex = renderer_->getClickedHex(mousePos);
        if (hoveredHex.has_value() && state_.board.hasMarble(hoveredHex.value(), state_.currentPlayer)) {
            input_.addToSelection(hoveredHex.value(), state_.currentPlayer, state_.board);
        }
    }

    if (isDragging_ && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        isDragging_ = false;

        if (input_.hasSelection()) {
            validDirections_ = input_.getValidDirections(state_.board, state_.currentPlayer);

            if (!validDirections_.empty()) {
                showWheel_ = true;
                wheelCenter_ = input_.getSelectedMarbles()[0];
            } else {
                // Invalid selection, clear it
                input_.clearSelection();
            }
        }
    }
}

void GameApp::update() {
    // TODO
}

void GameApp::render() const {
    BeginDrawing();
    ClearBackground({200, 200, 200, 255});

    renderer_->draw(state_.board);

    if (input_.hasSelection()) {
        for (const auto& pos : input_.getSelectedMarbles()) {
            Vector3 worldPos = {
                1.2f * std::sqrt(3.0f) * (static_cast<float>(pos.q()) + static_cast<float>(pos.r())/2.0f),
                0.0f,
                1.2f * 1.5f * static_cast<float>(pos.r())
            };
            worldPos.y = 1.0f;

            Vector2 screenPos = GetWorldToScreen(worldPos, renderer_->camera);
            DrawCircleLines(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y), 35.0f, YELLOW);
            DrawCircleLines(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y), 36.0f, YELLOW);
        }
    }

    if (showWheel_) {
        renderer_->drawSelectionWheel(wheelCenter_, validDirections_);
    }

    UI::drawHUD(state_);

    if (gameOver_) {
        UI::drawGameOver(state_);
    }

    EndDrawing();
}

void GameApp::reset() {
    state_ = GameState();
    state_.board.setup();
    gameOver_ = false;

    input_.clearSelection();
    showWheel_ = false;
    isDragging_ = false;
    std::cout << "\n=== GAME RESET ===" << std::endl;
}