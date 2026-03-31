#ifndef ABALONE_GAMEAPP2D_H
#define ABALONE_GAMEAPP2D_H

#include <optional>
#include <string>
#include <vector>

#include <raylib.h>

#include "../ai/MinimaxAI.h"
#include "../game/GameState.h"
#include "../game/InputHandler.h"

enum class ControllerType {
    Human,
    Minimax
};

class GameApp2D {
public:
    GameApp2D();
    void run();

private:
    enum class ScreenState {
        Setup,
        Playing,
        GameOver
    };

    struct PlayerConfig {
        ControllerType controller = ControllerType::Human;
        MinimaxAI ai;
        bool dropdownOpen = false;
    };

    struct SetupControl {
        enum class FieldId {
            Controller,
            Depth,
            ThreadCount,
            MarbleWeight,
            EjectionWeight,
            CenterWeight
        };

        std::string label;
        enum class Type {
            Dropdown,
            IntegerEditor
        } type = Type::Dropdown;
        FieldId field = FieldId::Controller;
        Rectangle bounds{};
    };

    static constexpr int screenWidth_ = 1280;
    static constexpr int screenHeight_ = 900;

    GameState state_;
    InputHandler input_;
    PlayerConfig playerConfigs_[2];
    ScreenState screenState_;
    std::vector<Direction> validDirections_;
    std::optional<Position> hoveredHex_;
    float aiMoveCooldown_;
    std::optional<Player> activeSetupPlayer_;
    std::optional<SetupControl::FieldId> activeSetupField_;
    std::string setupInputBuffer_;

    void resetGame();
    void handleInput();
    void handleSetupInput();
    void handlePlayingInput();
    void update();
    void render() const;
    void renderSetup() const;
    void renderBoard() const;
    void renderSidebar() const;
    void renderGameOver() const;

    [[nodiscard]] bool currentPlayerIsHuman() const;
    void performAIMove();
    void tryPlayDirection(Direction direction);
    void setController(Player player, ControllerType controller);
    [[nodiscard]] PlayerConfig& configFor(Player player);
    [[nodiscard]] const PlayerConfig& configFor(Player player) const;
    [[nodiscard]] static int playerIndex(Player player);
    [[nodiscard]] std::vector<SetupControl> setupControlsFor(Player player) const;
    [[nodiscard]] Rectangle startButtonBounds() const;
    void beginSetupFieldEdit(Player player, SetupControl::FieldId field);
    void commitSetupFieldEdit();
    void clearSetupFieldEdit();
    void handleSetupTextInput();
    [[nodiscard]] int setupFieldValue(Player player, SetupControl::FieldId field) const;
    void setSetupFieldValue(Player player, SetupControl::FieldId field, int value);
    [[nodiscard]] bool isEditingField(Player player, SetupControl::FieldId field) const;

    [[nodiscard]] std::optional<Position> getHexAtScreenPosition(float x, float y) const;
    [[nodiscard]] static bool pointInHex(float px, float py, float cx, float cy, float radius);
    static void drawHexagon(float centerX, float centerY, float radius, Color color);
    void drawDirectionButtons() const;
    static void drawDropdown(Rectangle bounds, const char* label, bool open);
    static void drawDepthEditor(Rectangle bounds, const char* label, int depth);
    static void drawIntegerEditor(Rectangle bounds, const char* label, int value, bool active, const char* textOverride = nullptr);

    [[nodiscard]] static float boardRadius();
    [[nodiscard]] static float marbleRadius();
    [[nodiscard]] static float hexSpacing();
    [[nodiscard]] static Vector2 boardOrigin();
    [[nodiscard]] static Vector2 toScreen(const Position& pos);
    [[nodiscard]] static const char* controllerLabel(ControllerType type);
    [[nodiscard]] static const char* playerLabel(Player player);
};

#endif // ABALONE_GAMEAPP2D_H
