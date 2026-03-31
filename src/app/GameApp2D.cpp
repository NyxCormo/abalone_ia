#include "GameApp2D.h"

#include <algorithm>
#include <array>
#include <cmath>

#include <raylib.h>

#include "../game/GameEngine.h"

namespace {
constexpr Color kBackground = {239, 232, 220, 255};
constexpr Color kBoardFill = {210, 187, 145, 255};
constexpr Color kBoardOutline = {96, 68, 43, 255};
constexpr Color kBlackMarble = {38, 41, 46, 255};
constexpr Color kWhiteMarble = {245, 242, 233, 255};
constexpr Color kSelection = {255, 199, 0, 255};
constexpr Color kHover = {92, 151, 255, 255};
constexpr Color kPanel = {252, 248, 241, 255};
constexpr Color kAccent = {166, 104, 65, 255};
constexpr Color kButton = {232, 217, 197, 255};

std::array<Direction, 6> kDirections = {
    Direction::EAST,
    Direction::NORTH_EAST,
    Direction::NORTH_WEST,
    Direction::WEST,
    Direction::SOUTH_WEST,
    Direction::SOUTH_EAST
};
}

GameApp2D::GameApp2D()
    : state_{},
      input_{},
      blackController_{ControllerType::Human},
      whiteController_{ControllerType::Human},
      screenState_{ScreenState::Setup},
      validDirections_{},
      hoveredHex_{std::nullopt},
      blackDropdownOpen_{false},
      whiteDropdownOpen_{false} {
    state_.board.setup();
}

void GameApp2D::run() {
    InitWindow(screenWidth_, screenHeight_, "Abalone 2D");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        handleInput();
        update();
        render();
    }

    CloseWindow();
}

void GameApp2D::resetGame() {
    state_ = GameState();
    state_.board.setup();
    state_.winner.reset();
    input_.clearSelection();
    validDirections_.clear();
    hoveredHex_.reset();
    blackDropdownOpen_ = false;
    whiteDropdownOpen_ = false;
    screenState_ = ScreenState::Playing;
}

void GameApp2D::handleInput() {
    switch (screenState_) {
        case ScreenState::Setup:
            handleSetupInput();
            break;
        case ScreenState::Playing:
            handlePlayingInput();
            break;
        case ScreenState::GameOver:
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_R)) {
                screenState_ = ScreenState::Setup;
                input_.clearSelection();
                validDirections_.clear();
                hoveredHex_.reset();
                blackDropdownOpen_ = false;
                whiteDropdownOpen_ = false;
            }
            break;
    }
}

void GameApp2D::handleSetupInput() {
    const Vector2 mouse = GetMousePosition();
    const Rectangle blackDropdown = {100.0f, 250.0f, 260.0f, 48.0f};
    const Rectangle blackHuman = {100.0f, 298.0f, 260.0f, 44.0f};
    const Rectangle whiteDropdown = {100.0f, 440.0f, 260.0f, 48.0f};
    const Rectangle whiteHuman = {100.0f, 488.0f, 260.0f, 44.0f};
    const Rectangle startButton = {100.0f, 650.0f, 320.0f, 70.0f};

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, blackDropdown)) {
            blackDropdownOpen_ = !blackDropdownOpen_;
            whiteDropdownOpen_ = false;
            return;
        }
        if (CheckCollisionPointRec(mouse, whiteDropdown)) {
            whiteDropdownOpen_ = !whiteDropdownOpen_;
            blackDropdownOpen_ = false;
            return;
        }
        if (blackDropdownOpen_ && CheckCollisionPointRec(mouse, blackHuman)) {
            blackController_ = ControllerType::Human;
            blackDropdownOpen_ = false;
            return;
        }
        if (whiteDropdownOpen_ && CheckCollisionPointRec(mouse, whiteHuman)) {
            whiteController_ = ControllerType::Human;
            whiteDropdownOpen_ = false;
            return;
        }
        if (CheckCollisionPointRec(mouse, startButton)) {
            resetGame();
            return;
        }

        blackDropdownOpen_ = false;
        whiteDropdownOpen_ = false;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        resetGame();
    }
}

void GameApp2D::handlePlayingInput() {
    hoveredHex_ = getHexAtScreenPosition(GetMouseX(), GetMouseY());

    if (!currentPlayerIsHuman()) {
        return;
    }

    if (IsKeyPressed(KEY_R)) {
        screenState_ = ScreenState::Setup;
        return;
    }

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        input_.clearSelection();
        validDirections_.clear();
        return;
    }

    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) tryPlayDirection(Direction::WEST);
    if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) tryPlayDirection(Direction::NORTH_WEST);
    if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) tryPlayDirection(Direction::NORTH_EAST);
    if (IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_KP_4)) tryPlayDirection(Direction::SOUTH_WEST);
    if (IsKeyPressed(KEY_FIVE) || IsKeyPressed(KEY_KP_5)) tryPlayDirection(Direction::SOUTH_EAST);
    if (IsKeyPressed(KEY_SIX) || IsKeyPressed(KEY_KP_6)) tryPlayDirection(Direction::EAST);

    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return;
    }

    const Vector2 mouse = GetMousePosition();
    const float buttonY = static_cast<float>(screenHeight_ - 130);
    const float buttonStartX = 70.0f;
    const float buttonSize = 72.0f;

    for (int i = 0; i < static_cast<int>(kDirections.size()); ++i) {
        const Rectangle button = {
            buttonStartX + i * (buttonSize + 10.0f),
            buttonY,
            buttonSize,
            buttonSize
        };
        if (CheckCollisionPointRec(mouse, button)) {
            tryPlayDirection(kDirections[static_cast<size_t>(i)]);
            return;
        }
    }

    if (!hoveredHex_.has_value()) {
        input_.clearSelection();
        validDirections_.clear();
        return;
    }

    const Position clicked = hoveredHex_.value();
    const auto& selected = input_.getSelectedMarbles();
    if (std::find(selected.begin(), selected.end(), clicked) != selected.end()) {
        input_.removeFromSelection(clicked);
    } else {
        input_.addToSelection(clicked, state_.currentPlayer, state_.board);
    }

    validDirections_ = input_.getValidDirections(state_.board, state_.currentPlayer);
}

void GameApp2D::update() {
    if (screenState_ != ScreenState::Playing || currentPlayerIsHuman() || GameEngine::isGameOver(state_)) {
        return;
    }
}

void GameApp2D::render() const {
    BeginDrawing();
    ClearBackground(kBackground);

    switch (screenState_) {
        case ScreenState::Setup:
            renderSetup();
            break;
        case ScreenState::Playing:
            renderBoard();
            renderSidebar();
            break;
        case ScreenState::GameOver:
            renderBoard();
            renderSidebar();
            renderGameOver();
            break;
    }

    EndDrawing();
}

void GameApp2D::renderSetup() const {
    DrawText("ABALONE 2D", 70, 70, 48, BLACK);
    DrawText("Configuration de partie", 70, 130, 28, kAccent);

    DrawRectangleRounded({70.0f, 190.0f, 660.0f, 430.0f}, 0.06f, 12, kPanel);
    DrawText("Noir", 100, 215, 28, BLACK);
    drawDropdown({100.0f, 250.0f, 260.0f, 48.0f}, controllerLabel(blackController_), blackDropdownOpen_);
    if (blackDropdownOpen_) {
        DrawRectangleRounded({100.0f, 298.0f, 260.0f, 44.0f}, 0.12f, 6, kButton);
        DrawText("Joueur", 122, 309, 22, BLACK);
    }

    DrawText("Blanc", 100, 405, 28, BLACK);
    drawDropdown({100.0f, 440.0f, 260.0f, 48.0f}, controllerLabel(whiteController_), whiteDropdownOpen_);
    if (whiteDropdownOpen_) {
        DrawRectangleRounded({100.0f, 488.0f, 260.0f, 44.0f}, 0.12f, 6, kButton);
        DrawText("Joueur", 122, 499, 22, BLACK);
    }

    DrawText("Modes possibles", 760, 220, 32, BLACK);
    DrawText("- Humain", 760, 270, 24, DARKGRAY);

    DrawRectangleRounded({100.0f, 650.0f, 320.0f, 70.0f}, 0.2f, 8, kAccent);
    DrawText("Lancer la partie", 145, 672, 28, RAYWHITE);

    DrawText("Configuration : Joueur vs Joueur", 70, 780, 24, BLACK);
}

void GameApp2D::renderBoard() const {
    for (int q = -4; q <= 4; ++q) {
        for (int r = -4; r <= 4; ++r) {
            const Position pos(q, r);
            if (!pos.isValid()) {
                continue;
            }

            const Vector2 center = toScreen(pos);
            Color hexColor = kBoardFill;
            if (hoveredHex_.has_value() && hoveredHex_.value() == pos) {
                hexColor = {224, 208, 171, 255};
            }

            drawHexagon(center.x, center.y, boardRadius(), hexColor);
            DrawPolyLines(center, 6, boardRadius(), 30.0f, kBoardOutline);

            const auto& selected = input_.getSelectedMarbles();
            if (std::find(selected.begin(), selected.end(), pos) != selected.end()) {
                DrawCircleLinesV(center, marbleRadius() + 8.0f, kSelection);
                DrawCircleLinesV(center, marbleRadius() + 9.0f, kSelection);
            }

            switch (state_.board.get(pos)) {
                case Cell::Black:
                    DrawCircleV(center, marbleRadius(), kBlackMarble);
                    DrawCircleLinesV(center, marbleRadius(), Fade(BLACK, 0.4f));
                    break;
                case Cell::White:
                    DrawCircleV(center, marbleRadius(), kWhiteMarble);
                    DrawCircleLinesV(center, marbleRadius(), GRAY);
                    break;
                case Cell::Empty:
                    DrawCircleV(center, 4.0f, Fade(kBoardOutline, 0.6f));
                    break;
            }
        }
    }

    drawDirectionButtons();
}

void GameApp2D::renderSidebar() const {
    DrawRectangleRounded({880.0f, 40.0f, 340.0f, 820.0f}, 0.05f, 12, kPanel);

    DrawText("Partie", 920, 80, 34, BLACK);
    DrawText(TextFormat("Tour : %d", state_.moveCount + 1), 920, 140, 24, DARKGRAY);
    DrawText(
        TextFormat("Actuel : %s", playerLabel(state_.currentPlayer)),
        920,
        175,
        24,
        state_.currentPlayer == Player::Black ? BLACK : GRAY
    );

    DrawText(TextFormat("Noir : %s", controllerLabel(blackController_)), 920, 240, 24, BLACK);
    DrawText(TextFormat("Blanc : %s", controllerLabel(whiteController_)), 920, 275, 24, BLACK);

    DrawText("Score", 920, 340, 30, BLACK);
    DrawText(TextFormat("Billes noires : %d", state_.board.countMarbles(Player::Black)), 920, 390, 22, BLACK);
    DrawText(TextFormat("Billes blanches : %d", state_.board.countMarbles(Player::White)), 920, 420, 22, BLACK);
    DrawText(TextFormat("Noires ejectees : %d", state_.board.blackEjected()), 920, 470, 22, BLACK);
    DrawText(TextFormat("Blanches ejectees : %d", state_.board.whiteEjected()), 920, 500, 22, BLACK);

    DrawText("Controles", 920, 640, 30, BLACK);
    DrawText("Clic gauche : selection", 920, 690, 20, DARKGRAY);
    DrawText("Clic droit : annuler", 920, 720, 20, DARKGRAY);
    DrawText("1..6 ou boutons : jouer", 920, 750, 20, DARKGRAY);
    DrawText("R : retour menu", 920, 780, 20, DARKGRAY);
}

void GameApp2D::renderGameOver() const {
    DrawRectangle(0, 0, screenWidth_, screenHeight_, Fade(BLACK, 0.35f));
    DrawRectangleRounded({350.0f, 260.0f, 580.0f, 220.0f}, 0.08f, 12, kPanel);
    DrawText("PARTIE TERMINEE", 425, 300, 40, BLACK);
    DrawText(TextFormat("Vainqueur : %s", playerLabel(state_.winner.value())), 435, 360, 30, kAccent);
    DrawText("ENTREE ou R pour revenir au menu", 395, 420, 24, DARKGRAY);
}

bool GameApp2D::currentPlayerIsHuman() const {
    return (state_.currentPlayer == Player::Black ? blackController_ : whiteController_) == ControllerType::Human;
}

void GameApp2D::tryPlayDirection(Direction direction) {
    if (std::find(validDirections_.begin(), validDirections_.end(), direction) == validDirections_.end()) {
        return;
    }

    const std::optional<Move> move = input_.tryCreateMove(direction, state_.board, state_.currentPlayer);
    if (!move.has_value()) {
        return;
    }

    state_ = GameEngine::applyMove(state_, move.value());
    input_.clearSelection();
    validDirections_.clear();
    hoveredHex_ = std::nullopt;

    if (GameEngine::isGameOver(state_)) {
        screenState_ = ScreenState::GameOver;
    }
}

std::optional<Position> GameApp2D::getHexAtScreenPosition(float x, float y) const {
    for (int q = -4; q <= 4; ++q) {
        for (int r = -4; r <= 4; ++r) {
            const Position pos(q, r);
            if (!pos.isValid()) {
                continue;
            }

            const Vector2 center = toScreen(pos);
            if (pointInHex(x, y, center.x, center.y, boardRadius())) {
                return pos;
            }
        }
    }
    return std::nullopt;
}

bool GameApp2D::pointInHex(float px, float py, float cx, float cy, float radius) {
    const float dx = std::fabs(px - cx);
    const float dy = std::fabs(py - cy);
    if (dx > radius || dy > std::sqrt(3.0f) * radius / 2.0f) {
        return false;
    }
    return std::sqrt(3.0f) * dx + dy <= std::sqrt(3.0f) * radius;
}

void GameApp2D::drawHexagon(float centerX, float centerY, float radius, Color color) {
    DrawPoly({centerX, centerY}, 6, radius, 30.0f, color);
}

void GameApp2D::drawDirectionButtons() const {
    const float buttonY = static_cast<float>(screenHeight_ - 130);
    const float buttonStartX = 70.0f;
    const float buttonSize = 72.0f;

    for (int i = 0; i < static_cast<int>(kDirections.size()); ++i) {
        const Direction direction = kDirections[static_cast<size_t>(i)];
        const bool enabled = std::find(validDirections_.begin(), validDirections_.end(), direction) != validDirections_.end();
        const Rectangle button = {
            buttonStartX + i * (buttonSize + 10.0f),
            buttonY,
            buttonSize,
            buttonSize
        };

        DrawRectangleRounded(button, 0.18f, 8, enabled ? kAccent : kButton);
        DrawText(
            TextFormat("%d", i + 1),
            static_cast<int>(button.x + 10.0f),
            static_cast<int>(button.y + 10.0f),
            18,
            enabled ? RAYWHITE : DARKGRAY
        );

        const char* label = "?";
        switch (direction) {
            case Direction::WEST: label = "W"; break;
            case Direction::NORTH_WEST: label = "NW"; break;
            case Direction::NORTH_EAST: label = "NE"; break;
            case Direction::SOUTH_WEST: label = "SW"; break;
            case Direction::SOUTH_EAST: label = "SE"; break;
            case Direction::EAST: label = "E"; break;
        }
        DrawText(
            label,
            static_cast<int>(button.x + 20.0f),
            static_cast<int>(button.y + 30.0f),
            24,
            enabled ? RAYWHITE : BLACK
        );
    }
}

float GameApp2D::boardRadius() {
    return 42.0f;
}

float GameApp2D::marbleRadius() {
    return 24.0f;
}

float GameApp2D::hexSpacing() {
    return boardRadius() * 1.85f;
}

Vector2 GameApp2D::boardOrigin() {
    return {420.0f, 370.0f};
}

Vector2 GameApp2D::toScreen(const Position& pos) {
    const float size = hexSpacing() / std::sqrt(3.0f);
    const float x = boardOrigin().x + size * std::sqrt(3.0f) * (static_cast<float>(pos.q()) + static_cast<float>(pos.r()) / 2.0f);
    const float y = boardOrigin().y + size * 1.5f * static_cast<float>(pos.r());
    return {x, y};
}

const char* GameApp2D::controllerLabel(ControllerType type) {
    return type == ControllerType::Human ? "Joueur" : "IA";
}

const char* GameApp2D::playerLabel(Player player) {
    return player == Player::Black ? "Noir" : "Blanc";
}

void GameApp2D::drawDropdown(Rectangle bounds, const char* label, bool open) {
    DrawRectangleRounded(bounds, 0.14f, 8, kButton);
    DrawRectangleLinesEx(bounds, 1.0f, kBoardOutline);
    DrawText(label, static_cast<int>(bounds.x + 16.0f), static_cast<int>(bounds.y + 12.0f), 24, BLACK);
    DrawText(open ? "^" : "v", static_cast<int>(bounds.x + bounds.width - 28.0f), static_cast<int>(bounds.y + 10.0f), 24, BLACK);
}