#pragma once

#include "raylib.h"
#include "../game/Board.h"
#include "../geometry/Position.h"
#include <vector>
#include <optional>

class Renderer
{
public:

    Camera3D camera;

    Renderer();
    ~Renderer();

    [[nodiscard]] std::optional<Position> getClickedHex(Vector2 mousePos) const;
    [[nodiscard]] std::optional<Direction> getClickedDirection(Position hexPos, Vector2 mousePos) const;

    void draw(const Board& board);
    void drawSelectionWheel(Position center, const std::vector<Direction>& directions) const;
    void drawGlobalDirectionRose() const;

private:

    Model boardModel;
    Model hexModel;
    Model marbleWhite;
    Model marbleBlack;

    Texture2D whiteMarbleTex;
    Texture2D blackMarbleTex;
    Texture2D hexWood;
    Texture2D tableTexture;

    Shader marbleShader;

    int lightPosLoc;
    int lightColorLoc;
    int lightIntensityLoc;
    int shininessLoc;
    int specularStrengthLoc;
    int ambientStrengthLoc;
    int viewPosLoc;

    Vector3 lightPosition;

    float marbleShininess;
    float marbleSpecularStrength;
    float marbleAmbientStrength;

    static Vector3 hexToWorld(int q, int r);
    static Vector2 worldToScreen(Vector3 worldPos, const Camera3D &cam);
    static bool isPointInHex(Vector2 point, Vector2 hexCenter, float radius);

    void updateShaderLighting();
};