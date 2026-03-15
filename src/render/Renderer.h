#pragma once

#include "raylib.h"
#include "../core/Board.h"
#include "../core/Position.h"
#include <cmath>
#include <vector>
#include <optional>

class Renderer
{
public:

    Camera3D camera;

    Renderer();
    ~Renderer();
    void draw(const Board& board);
    std::optional<Position> getClickedHex(Vector2 mousePos);
    void drawSelectionWheel(Position center, const std::vector<Direction>& directions);
    std::optional<Direction> getClickedDirection(Position hexPos, Vector2 mousePos);

private:

    Model boardModel;
    Model hexModel;
    Model marbleWhite;
    Model marbleBlack;

    Texture2D whiteMarbleTex;
    Texture2D blackMarbleTex;
    Texture2D hexWood;
    Texture2D tableWood;

    static Vector3 hexToWorld(int q, int r);
    static Vector2 worldToScreen(Vector3 worldPos, Camera3D cam);
    static bool isPointInHex(Vector2 point, Vector2 hexCenter, float radius);
};