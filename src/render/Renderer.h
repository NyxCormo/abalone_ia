#pragma once

#include "raylib.h"
#include "../core/Board.h"
#include <cmath>

class Renderer
{
public:
    Renderer();
    void draw(Board& board);

private:
    Camera3D camera;

    Model boardModel;
    Model hexModel;
    Model marbleWhite;
    Model marbleBlack;

    Texture2D whiteMarbleTex;
    Texture2D blackMarbleTex;
    Texture2D hexWood;
    Texture2D tableWood;

    static Vector3 hexToWorld(int q, int r)
    {
        float size = 1.2f;
        float x = size * sqrt(3) * (q + r/2.0f);
        float z = size * 1.5f * r;
        return {x, 0, z};
    }
};