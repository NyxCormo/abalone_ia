#include "GameApp2D.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <initializer_list>

#include <raylib.h>

#include "../game/GameEngine.h"

namespace {
constexpr Color kBackground = {239, 232, 220, 255};
constexpr Color kBoardFill = {210, 187, 145, 255};
constexpr Color kBoardOutline = {96, 68, 43, 255};
constexpr Color kBlackMarble = {38, 41, 46, 255};
constexpr Color kWhiteMarble = {245, 242, 233, 255};
constexpr Color kSelection = {255, 199, 0, 255};
constexpr Color kPanel = {252, 248, 241, 255};
constexpr Color kAccent = {166, 104, 65, 255};
constexpr Color kButton = {232, 217, 197, 255};
constexpr Color kInputFill = {244, 238, 228, 255};

constexpr std::array<Direction, 6> kDirections = {
    Direction::WEST,
    Direction::NORTH_WEST,
    Direction::NORTH_EAST,
    Direction::SOUTH_WEST,
    Direction::SOUTH_EAST,
    Direction::EAST
};

struct SetupLayout {
    Rectangle panel = {40.0f, 170.0f, 1200.0f, 660.0f};
    Rectangle playerCard = {70.0f, 220.0f, 500.0f, 580.0f};
    float playerColumnGap = 580.0f;
    Vector2 playerTitle = {105.0f, 250.0f};
    Rectangle dropdown = {105.0f, 300.0f, 220.0f, 48.0f};
    float dropdownItemHeight = 44.0f;
    float controlSpacing = 90.0f;
    Rectangle editor = {105.0f, 410.0f, 220.0f, 48.0f};
    Rectangle startButton = {480.0f, 75.0f, 270.0f, 64.0f};
    Vector2 modesText = {800.0f, 82.0f};
};

struct SidebarLayout {
    Rectangle panel = {880.0f, 40.0f, 340.0f, 820.0f};
    int left = 920;
    int titleY = 80;
    int gameSectionY = 140;
    int controllerSectionY = 240;
    int scoreSectionY = 390;
    int controlsSectionY = 640;
    int lineHeight = 35;
};

struct DirectionButtonLayout {
    float startX = 70.0f;
    float bottomOffset = 130.0f;
    float size = 72.0f;
    float gap = 10.0f;
};

struct BoardLayout {
    float hexRadius = 42.0f;
    float marbleRadius = 24.0f;
    Vector2 origin = {420.0f, 370.0f};
};

constexpr SetupLayout kSetupLayout{};
constexpr SidebarLayout kSidebarLayout{};
constexpr DirectionButtonLayout kDirectionLayout{};
constexpr BoardLayout kBoardLayout{};

const char* directionLabel(Direction direction) {
    switch (direction) {
        case Direction::WEST: return "W";
        case Direction::NORTH_WEST: return "NW";
        case Direction::NORTH_EAST: return "NE";
        case Direction::SOUTH_WEST: return "SW";
        case Direction::SOUTH_EAST: return "SE";
        case Direction::EAST: return "E";
    }
    return "?";
}

Rectangle offsetRectangle(Rectangle base, float dx, float dy) {
    return {base.x + dx, base.y + dy, base.width, base.height};
}

Rectangle editorValueBounds(Rectangle bounds) {
    return {bounds.x + 60.0f, bounds.y, bounds.width - 120.0f, bounds.height};
}
}

GameApp2D::GameApp2D()
    : state_{},
      input_{},
      playerConfigs_{
          PlayerConfig{ControllerType::Minimax, MinimaxAI{}, false},
          PlayerConfig{ControllerType::Minimax, MinimaxAI{}, false}
      },
      screenState_{ScreenState::Setup},
      validDirections_{},
      hoveredHex_{std::nullopt},
      aiMoveCooldown_{0.0f},
      activeSetupPlayer_{std::nullopt},
      activeSetupField_{std::nullopt},
      setupInputBuffer_{} {
    state_.board.setup();
}

void GameApp2D::run() {
    InitWindow(screenWidth_, screenHeight_, "Abalone 2D");
    SetTargetFPS(200);

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
    aiMoveCooldown_ = 0.15f;
    for (PlayerConfig& config : playerConfigs_) {
        config.dropdownOpen = false;
    }
    clearSetupFieldEdit();
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
                for (PlayerConfig& config : playerConfigs_) {
                    config.dropdownOpen = false;
                }
                clearSetupFieldEdit();
            }
            break;
    }
}

void GameApp2D::handleSetupInput() {
    const Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        bool clickedKnownControl = false;

        for (Player player : {Player::Black, Player::White}) {
            PlayerConfig& config = configFor(player);
            const std::vector<SetupControl> controls = setupControlsFor(player);
            const Rectangle dropdown = controls.front().bounds;

            if (CheckCollisionPointRec(mouse, dropdown)) {
                commitSetupFieldEdit();
                config.dropdownOpen = !config.dropdownOpen;
                configFor(opponent(player)).dropdownOpen = false;
                clickedKnownControl = true;
                return;
            }

            if (config.dropdownOpen) {
                const Rectangle humanItem = offsetRectangle(dropdown, 0.0f, dropdown.height);
                const Rectangle minimaxItem = offsetRectangle(humanItem, 0.0f, kSetupLayout.dropdownItemHeight);
                if (CheckCollisionPointRec(mouse, humanItem)) {
                    commitSetupFieldEdit();
                    setController(player, ControllerType::Human);
                    clickedKnownControl = true;
                    return;
                }
                if (CheckCollisionPointRec(mouse, minimaxItem)) {
                    commitSetupFieldEdit();
                    setController(player, ControllerType::Minimax);
                    clickedKnownControl = true;
                    return;
                }
            }

            for (const SetupControl& control : controls) {
                if (control.type != SetupControl::Type::IntegerEditor) {
                    continue;
                }

                const Rectangle minus = {control.bounds.x, control.bounds.y, 48.0f, control.bounds.height};
                const Rectangle valueBox = editorValueBounds(control.bounds);
                const Rectangle plus = {
                    control.bounds.x + control.bounds.width - 48.0f,
                    control.bounds.y,
                    48.0f,
                    control.bounds.height
                };

                if (CheckCollisionPointRec(mouse, valueBox)) {
                    beginSetupFieldEdit(player, control.field);
                    clickedKnownControl = true;
                    return;
                }

                if (!CheckCollisionPointRec(mouse, minus) && !CheckCollisionPointRec(mouse, plus)) {
                    continue;
                }

                commitSetupFieldEdit();
                const int delta = CheckCollisionPointRec(mouse, minus) ? -1 : 1;
                MinimaxAI& ai = config.ai;
                if (control.field == SetupControl::FieldId::Depth) {
                    ai.setDepth(ai.depth() + delta);
                } else if (control.field == SetupControl::FieldId::MarbleWeight) {
                    ai.setMarbleWeight(ai.marbleWeight() + delta * 10);
                } else if (control.field == SetupControl::FieldId::EjectionWeight) {
                    ai.setEjectionWeight(ai.ejectionWeight() + delta * 10);
                } else if (control.field == SetupControl::FieldId::CenterWeight) {
                    ai.setCenterWeight(ai.centerWeight() + delta);
                }
                clickedKnownControl = true;
                return;
            }
        }

        const float buttonCenterX = (screenWidth_ - startButtonBounds().width) / 2.0f;
        const Rectangle centeredButton = {
            buttonCenterX,
            startButtonBounds().y,
            startButtonBounds().width,
            startButtonBounds().height
        };
        if (CheckCollisionPointRec(mouse, centeredButton)) {
            commitSetupFieldEdit();
            resetGame();
            return;
        }

        if (!clickedKnownControl) {
            commitSetupFieldEdit();
        }
        for (PlayerConfig& config : playerConfigs_) {
            config.dropdownOpen = false;
        }
    }

    handleSetupTextInput();

    if (IsKeyPressed(KEY_ENTER)) {
        if (activeSetupField_.has_value()) {
            commitSetupFieldEdit();
        } else {
            resetGame();
        }
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
    const float buttonY = static_cast<float>(screenHeight_) - kDirectionLayout.bottomOffset;

    for (int i = 0; i < static_cast<int>(kDirections.size()); ++i) {
        const Rectangle button = {
            kDirectionLayout.startX + i * (kDirectionLayout.size + kDirectionLayout.gap),
            buttonY,
            kDirectionLayout.size,
            kDirectionLayout.size
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

    aiMoveCooldown_ -= GetFrameTime();
    if (aiMoveCooldown_ <= 0.0f) {
        performAIMove();
        aiMoveCooldown_ = 0.2f;
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

    DrawRectangleRounded(kSetupLayout.panel, 0.06f, 12, kPanel);
    DrawText("Modes possibles", static_cast<int>(kSetupLayout.modesText.x), static_cast<int>(kSetupLayout.modesText.y), 28, BLACK);
    DrawText("- Humain", static_cast<int>(kSetupLayout.modesText.x), static_cast<int>(kSetupLayout.modesText.y + 34.0f), 22, DARKGRAY);
    DrawText("- MinMax", static_cast<int>(kSetupLayout.modesText.x + 120.0f), static_cast<int>(kSetupLayout.modesText.y + 34.0f), 22, DARKGRAY);

    for (Player player : {Player::Black, Player::White}) {
        const PlayerConfig& config = configFor(player);
        const std::vector<SetupControl> controls = setupControlsFor(player);
        const float columnOffset = player == Player::Black ? 0.0f : kSetupLayout.playerColumnGap;
        const Rectangle card = offsetRectangle(kSetupLayout.playerCard, columnOffset, 0.0f);
        const int titleX = static_cast<int>(kSetupLayout.playerTitle.x + columnOffset);
        const int titleY = static_cast<int>(kSetupLayout.playerTitle.y);

        DrawRectangleRounded(card, 0.05f, 10, Fade(RAYWHITE, 0.45f));
        DrawText(player == Player::Black ? "Joueur noir" : "Joueur blanc", titleX, titleY, 30, BLACK);
        drawDropdown(controls.front().bounds, controllerLabel(config.controller), config.dropdownOpen);

        for (size_t i = 1; i < controls.size(); ++i) {
            const SetupControl& control = controls[i];
            const bool active = isEditingField(player, control.field);
            const char* textOverride = active ? setupInputBuffer_.c_str() : nullptr;

            if (control.field == SetupControl::FieldId::Depth) {
                drawIntegerEditor(control.bounds, control.label.c_str(), config.ai.depth(), active, textOverride);
            } else if (control.field == SetupControl::FieldId::MarbleWeight) {
                drawIntegerEditor(control.bounds, control.label.c_str(), config.ai.marbleWeight(), active, textOverride);
            } else if (control.field == SetupControl::FieldId::EjectionWeight) {
                drawIntegerEditor(control.bounds, control.label.c_str(), config.ai.ejectionWeight(), active, textOverride);
            } else if (control.field == SetupControl::FieldId::CenterWeight) {
                drawIntegerEditor(control.bounds, control.label.c_str(), config.ai.centerWeight(), active, textOverride);
            }
        }
    }

    for (Player player : {Player::Black, Player::White}) {
        const PlayerConfig& config = configFor(player);
        if (!config.dropdownOpen) {
            continue;
        }

        const std::vector<SetupControl> controls = setupControlsFor(player);
        const Rectangle humanItem = offsetRectangle(controls.front().bounds, 0.0f, controls.front().bounds.height);
        const Rectangle minimaxItem = offsetRectangle(humanItem, 0.0f, kSetupLayout.dropdownItemHeight);

        DrawRectangleRounded(offsetRectangle(humanItem, 2.0f, 2.0f), 0.12f, 6, Fade(BLACK, 0.2f));
        DrawRectangleRounded(offsetRectangle(minimaxItem, 2.0f, 2.0f), 0.12f, 6, Fade(BLACK, 0.2f));

        DrawRectangleRounded(humanItem, 0.12f, 6, kButton);
        DrawRectangleRounded(minimaxItem, 0.12f, 6, kButton);
        DrawRectangleLinesEx(humanItem, 1.0f, kBoardOutline);
        DrawRectangleLinesEx(minimaxItem, 1.0f, kBoardOutline);
        DrawText("Joueur", static_cast<int>(humanItem.x + 22.0f), static_cast<int>(humanItem.y + 11.0f), 22, BLACK);
        DrawText("MinMax", static_cast<int>(minimaxItem.x + 22.0f), static_cast<int>(minimaxItem.y + 11.0f), 22, BLACK);
    }

    const float buttonCenterX = (screenWidth_ - startButtonBounds().width) / 2.0f;
    const Rectangle centeredButton = {
        buttonCenterX,
        startButtonBounds().y,
        startButtonBounds().width,
        startButtonBounds().height
    };
    DrawRectangleRounded(centeredButton, 0.2f, 8, kAccent);
    DrawText("Lancer la partie", static_cast<int>(centeredButton.x + 28.0f), static_cast<int>(centeredButton.y + 18.0f), 28, RAYWHITE);
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
    DrawRectangleRounded(kSidebarLayout.panel, 0.05f, 12, kPanel);

    DrawText("Partie", kSidebarLayout.left, kSidebarLayout.titleY, 34, BLACK);
    DrawText(TextFormat("Tour : %d", state_.moveCount + 1), kSidebarLayout.left, kSidebarLayout.gameSectionY, 24, DARKGRAY);
    DrawText(
        TextFormat("Actuel : %s", playerLabel(state_.currentPlayer)),
        kSidebarLayout.left,
        kSidebarLayout.gameSectionY + kSidebarLayout.lineHeight,
        24,
        state_.currentPlayer == Player::Black ? BLACK : GRAY
    );

    const PlayerConfig& black = configFor(Player::Black);
    const PlayerConfig& white = configFor(Player::White);
    DrawText(TextFormat("Noir : %s", controllerLabel(black.controller)), kSidebarLayout.left, kSidebarLayout.controllerSectionY, 24, BLACK);
    DrawText(TextFormat("Blanc : %s", controllerLabel(white.controller)), kSidebarLayout.left, kSidebarLayout.controllerSectionY + kSidebarLayout.lineHeight, 24, BLACK);

    int aiInfoY = kSidebarLayout.controllerSectionY + 2 * kSidebarLayout.lineHeight;
    for (Player player : {Player::Black, Player::White}) {
        const PlayerConfig& config = configFor(player);
        if (config.controller != ControllerType::Minimax) {
            continue;
        }

        DrawText(
            TextFormat(
                "%s IA d=%d b=%d e=%d c=%d",
                playerLabel(player),
                config.ai.depth(),
                config.ai.marbleWeight(),
                config.ai.ejectionWeight(),
                config.ai.centerWeight()
            ),
            kSidebarLayout.left,
            aiInfoY,
            22,
            kAccent
        );
        aiInfoY += 30;
    }

    DrawText("Score", kSidebarLayout.left, kSidebarLayout.scoreSectionY, 30, BLACK);
    DrawText(TextFormat("Billes noires : %d", state_.board.countMarbles(Player::Black)), kSidebarLayout.left, kSidebarLayout.scoreSectionY + 50, 22, BLACK);
    DrawText(TextFormat("Billes blanches : %d", state_.board.countMarbles(Player::White)), kSidebarLayout.left, kSidebarLayout.scoreSectionY + 80, 22, BLACK);
    DrawText(TextFormat("Noires ejectees : %d", state_.board.blackEjected()), kSidebarLayout.left, kSidebarLayout.scoreSectionY + 130, 22, BLACK);
    DrawText(TextFormat("Blanches ejectees : %d", state_.board.whiteEjected()), kSidebarLayout.left, kSidebarLayout.scoreSectionY + 160, 22, BLACK);

    DrawText("Controles", kSidebarLayout.left, kSidebarLayout.controlsSectionY, 30, BLACK);
    DrawText("Clic gauche : selection", kSidebarLayout.left, kSidebarLayout.controlsSectionY + 50, 20, DARKGRAY);
    DrawText("Clic droit : annuler", kSidebarLayout.left, kSidebarLayout.controlsSectionY + 80, 20, DARKGRAY);
    DrawText("1..6 ou boutons : jouer", kSidebarLayout.left, kSidebarLayout.controlsSectionY + 110, 20, DARKGRAY);
    DrawText("R : retour menu", kSidebarLayout.left, kSidebarLayout.controlsSectionY + 140, 20, DARKGRAY);
}

void GameApp2D::renderGameOver() const {
    DrawRectangle(0, 0, screenWidth_, screenHeight_, Fade(BLACK, 0.35f));
    DrawRectangleRounded({350.0f, 260.0f, 580.0f, 220.0f}, 0.08f, 12, kPanel);
    DrawText("PARTIE TERMINEE", 425, 300, 40, BLACK);
    DrawText(TextFormat("Vainqueur : %s", playerLabel(state_.winner.value())), 435, 360, 30, kAccent);
    DrawText("ENTREE ou R pour revenir au menu", 395, 420, 24, DARKGRAY);
}

bool GameApp2D::currentPlayerIsHuman() const {
    return configFor(state_.currentPlayer).controller == ControllerType::Human;
}

void GameApp2D::performAIMove() {
    const MinimaxAI& ai = configFor(state_.currentPlayer).ai;
    const std::optional<Move> move = ai.chooseMove(state_);

    if (!move.has_value()) {
        state_.winner = opponent(state_.currentPlayer);
        screenState_ = ScreenState::GameOver;
        return;
    }

    state_ = GameEngine::applyMove(state_, move.value());
    input_.clearSelection();
    validDirections_.clear();

    if (GameEngine::isGameOver(state_)) {
        screenState_ = ScreenState::GameOver;
    }
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

void GameApp2D::setController(Player player, ControllerType controller) {
    commitSetupFieldEdit();
    PlayerConfig& config = configFor(player);
    config.controller = controller;
    config.dropdownOpen = false;
}

GameApp2D::PlayerConfig& GameApp2D::configFor(Player player) {
    return playerConfigs_[playerIndex(player)];
}

const GameApp2D::PlayerConfig& GameApp2D::configFor(Player player) const {
    return playerConfigs_[playerIndex(player)];
}

int GameApp2D::playerIndex(Player player) {
    return player == Player::Black ? 0 : 1;
}

std::vector<GameApp2D::SetupControl> GameApp2D::setupControlsFor(Player player) const {
    const float offsetX = player == Player::Black ? 0.0f : kSetupLayout.playerColumnGap;
    std::vector<SetupControl> controls;
    controls.push_back({"", SetupControl::Type::Dropdown, SetupControl::FieldId::Controller, offsetRectangle(kSetupLayout.dropdown, offsetX, 0.0f)});

    if (configFor(player).controller != ControllerType::Minimax) {
        return controls;
    }

    const std::array<SetupControl, 4> editorTemplates = {{
        {"Profondeur MinMax", SetupControl::Type::IntegerEditor, SetupControl::FieldId::Depth, {}},
        {"Poids billes", SetupControl::Type::IntegerEditor, SetupControl::FieldId::MarbleWeight, {}},
        {"Poids ejection", SetupControl::Type::IntegerEditor, SetupControl::FieldId::EjectionWeight, {}},
        {"Poids centre", SetupControl::Type::IntegerEditor, SetupControl::FieldId::CenterWeight, {}}
    }};

    for (size_t i = 0; i < editorTemplates.size(); ++i) {
        SetupControl control = editorTemplates[i];
        control.bounds = offsetRectangle(kSetupLayout.editor, offsetX, static_cast<float>(i) * kSetupLayout.controlSpacing);
        controls.push_back(control);
    }
    return controls;
}

void GameApp2D::beginSetupFieldEdit(Player player, SetupControl::FieldId field) {
    if (isEditingField(player, field)) {
        return;
    }

    commitSetupFieldEdit();
    activeSetupPlayer_ = player;
    activeSetupField_ = field;
    setupInputBuffer_ = std::to_string(setupFieldValue(player, field));
}

void GameApp2D::commitSetupFieldEdit() {
    if (!activeSetupPlayer_.has_value() || !activeSetupField_.has_value()) {
        return;
    }

    if (!setupInputBuffer_.empty()) {
        setSetupFieldValue(activeSetupPlayer_.value(), activeSetupField_.value(), std::atoi(setupInputBuffer_.c_str()));
    }
    clearSetupFieldEdit();
}

void GameApp2D::clearSetupFieldEdit() {
    activeSetupPlayer_.reset();
    activeSetupField_.reset();
    setupInputBuffer_.clear();
}

void GameApp2D::handleSetupTextInput() {
    if (!activeSetupField_.has_value()) {
        return;
    }

    int key = GetCharPressed();
    while (key > 0) {
        if (key >= '0' && key <= '9' && setupInputBuffer_.size() < 4) {
            if (setupInputBuffer_ == "0") {
                setupInputBuffer_.clear();
            }
            setupInputBuffer_.push_back(static_cast<char>(key));
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !setupInputBuffer_.empty()) {
        setupInputBuffer_.pop_back();
    }

    if (setupInputBuffer_.empty()) {
        setupInputBuffer_ = "0";
    }
}

int GameApp2D::setupFieldValue(Player player, SetupControl::FieldId field) const {
    const MinimaxAI& ai = configFor(player).ai;
    switch (field) {
        case SetupControl::FieldId::Depth: return ai.depth();
        case SetupControl::FieldId::MarbleWeight: return ai.marbleWeight();
        case SetupControl::FieldId::EjectionWeight: return ai.ejectionWeight();
        case SetupControl::FieldId::CenterWeight: return ai.centerWeight();
        case SetupControl::FieldId::Controller: break;
    }
    return 0;
}

void GameApp2D::setSetupFieldValue(Player player, SetupControl::FieldId field, int value) {
    MinimaxAI& ai = configFor(player).ai;
    switch (field) {
        case SetupControl::FieldId::Depth:
            ai.setDepth(value);
            break;
        case SetupControl::FieldId::MarbleWeight:
            ai.setMarbleWeight(value);
            break;
        case SetupControl::FieldId::EjectionWeight:
            ai.setEjectionWeight(value);
            break;
        case SetupControl::FieldId::CenterWeight:
            ai.setCenterWeight(value);
            break;
        case SetupControl::FieldId::Controller:
            break;
    }
}

bool GameApp2D::isEditingField(Player player, SetupControl::FieldId field) const {
    return activeSetupPlayer_.has_value()
        && activeSetupField_.has_value()
        && activeSetupPlayer_.value() == player
        && activeSetupField_.value() == field;
}

Rectangle GameApp2D::startButtonBounds() const {
    return kSetupLayout.startButton;
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
    const float buttonY = static_cast<float>(screenHeight_) - kDirectionLayout.bottomOffset;

    for (int i = 0; i < static_cast<int>(kDirections.size()); ++i) {
        const Direction direction = kDirections[static_cast<size_t>(i)];
        const bool enabled = std::find(validDirections_.begin(), validDirections_.end(), direction) != validDirections_.end();
        const Rectangle button = {
            kDirectionLayout.startX + i * (kDirectionLayout.size + kDirectionLayout.gap),
            buttonY,
            kDirectionLayout.size,
            kDirectionLayout.size
        };

        DrawRectangleRounded(button, 0.18f, 8, enabled ? kAccent : kButton);
        DrawText(
            TextFormat("%d", i + 1),
            static_cast<int>(button.x + 10.0f),
            static_cast<int>(button.y + 10.0f),
            18,
            enabled ? RAYWHITE : DARKGRAY
        );
        DrawText(
            directionLabel(direction),
            static_cast<int>(button.x + 20.0f),
            static_cast<int>(button.y + 30.0f),
            24,
            enabled ? RAYWHITE : BLACK
        );
    }
}

float GameApp2D::boardRadius() {
    return kBoardLayout.hexRadius;
}

float GameApp2D::marbleRadius() {
    return kBoardLayout.marbleRadius;
}

float GameApp2D::hexSpacing() {
    return boardRadius() * 1.85f;
}

Vector2 GameApp2D::boardOrigin() {
    return kBoardLayout.origin;
}

Vector2 GameApp2D::toScreen(const Position& pos) {
    const float size = hexSpacing() / std::sqrt(3.0f);
    const float x = boardOrigin().x + size * std::sqrt(3.0f) * (static_cast<float>(pos.q()) + static_cast<float>(pos.r()) / 2.0f);
    const float y = boardOrigin().y + size * 1.5f * static_cast<float>(pos.r());
    return {x, y};
}

const char* GameApp2D::controllerLabel(ControllerType type) {
    return type == ControllerType::Human ? "Joueur" : "MinMax";
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

void GameApp2D::drawDepthEditor(Rectangle bounds, const char* label, int depth) {
    drawIntegerEditor(bounds, label, depth, false);
}

void GameApp2D::drawIntegerEditor(Rectangle bounds, const char* label, int value, bool active, const char* textOverride) {
    DrawText(label, static_cast<int>(bounds.x), static_cast<int>(bounds.y - 32.0f), 22, BLACK);

    const Rectangle minus = {bounds.x, bounds.y, 48.0f, bounds.height};
    const Rectangle valueBox = editorValueBounds(bounds);
    const Rectangle plus = {bounds.x + bounds.width - 48.0f, bounds.y, 48.0f, bounds.height};

    DrawRectangleRounded(minus, 0.16f, 6, kButton);
    DrawRectangleRounded(valueBox, 0.16f, 6, kInputFill);
    DrawRectangleRounded(plus, 0.16f, 6, kButton);
    DrawRectangleLinesEx(valueBox, active ? 2.0f : 1.0f, active ? kAccent : Fade(kBoardOutline, 0.5f));

    DrawText("-", static_cast<int>(minus.x + 18.0f), static_cast<int>(minus.y + 8.0f), 28, BLACK);
    const char* displayedText = textOverride != nullptr ? textOverride : TextFormat("%d", value);
    const int textWidth = MeasureText(displayedText, 24);
    DrawText(displayedText, static_cast<int>(valueBox.x + (valueBox.width - static_cast<float>(textWidth)) / 2.0f), static_cast<int>(valueBox.y + 10.0f), 24, BLACK);
    DrawText("+", static_cast<int>(plus.x + 15.0f), static_cast<int>(plus.y + 8.0f), 28, BLACK);
}