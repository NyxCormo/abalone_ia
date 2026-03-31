#ifndef ABALONE_GAMEAPP2D_H
#define ABALONE_GAMEAPP2D_H

#include <optional>
#include <vector>

#include <raylib.h>

#include "../game/GameState.h"
#include "../game/InputHandler.h"

enum class ControllerType {
    Human
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

    static constexpr int screenWidth_ = 1280;
    static constexpr int screenHeight_ = 900;

    GameState state_;
    InputHandler input_;
    ControllerType blackController_;
    ControllerType whiteController_;
    ScreenState screenState_;
    std::vector<Direction> validDirections_;
    std::optional<Position> hoveredHex_;
    bool blackDropdownOpen_;
    bool whiteDropdownOpen_;

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
    void tryPlayDirection(Direction direction);

    [[nodiscard]] std::optional<Position> getHexAtScreenPosition(float x, float y) const;
    [[nodiscard]] static bool pointInHex(float px, float py, float cx, float cy, float radius);
    static void drawHexagon(float centerX, float centerY, float radius, Color color);
    void drawDirectionButtons() const;
    static void drawDropdown(Rectangle bounds, const char* label, bool open);

    [[nodiscard]] static float boardRadius();
    [[nodiscard]] static float marbleRadius();
    [[nodiscard]] static float hexSpacing();
    [[nodiscard]] static Vector2 boardOrigin();
    [[nodiscard]] static Vector2 toScreen(const Position& pos);
    [[nodiscard]] static const char* controllerLabel(ControllerType type);
    [[nodiscard]] static const char* playerLabel(Player player);
};

#endif // ABALONE_GAMEAPP2D_H