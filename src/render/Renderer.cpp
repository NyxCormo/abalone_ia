#include "Renderer.h"
#include <cmath>

Renderer::Renderer()
{
    camera.position = {10, 12, 10};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};
    camera.fovy = 45;
    camera.projection = CAMERA_PERSPECTIVE;

    whiteMarbleTex = LoadTexture("../assets/textures/whiteMarble.png");
    blackMarbleTex = LoadTexture("../assets/textures/blackMarble.png");
    hexWood = LoadTexture("../assets/textures/hexWood.png");
    tableWood = LoadTexture("../assets/textures/tableWood.png");

    float plateauRadius = 10.0f;
    float plateauHeight = 0.2f;
    Mesh baseMesh = GenMeshCylinder(plateauRadius, plateauHeight, 50);
    boardModel = LoadModelFromMesh(baseMesh);
    boardModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = {120, 80, 40, 255};
    boardModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tableWood;

    Mesh hexMesh = GenMeshCylinder(1.0f, 0.2f, 6);
    hexModel = LoadModelFromMesh(hexMesh);
    hexModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = {120, 80, 40, 255};
    hexModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = hexWood;

    Mesh marbleMesh = GenMeshSphere(0.5f, 32, 32);

    marbleWhite = LoadModelFromMesh(marbleMesh);
    marbleWhite.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = whiteMarbleTex;
    marbleWhite.materials[0].maps[MATERIAL_MAP_SPECULAR].color = WHITE;
    marbleWhite.materials[0].maps[MATERIAL_MAP_SPECULAR].texture = whiteMarbleTex;

    marbleBlack = LoadModelFromMesh(marbleMesh);
    marbleBlack.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = blackMarbleTex;
    marbleBlack.materials[0].maps[MATERIAL_MAP_SPECULAR].color = WHITE;
    marbleBlack.materials[0].maps[MATERIAL_MAP_SPECULAR].texture = blackMarbleTex;
}

void Renderer::draw(Board& board)
{
    UpdateCamera(&camera, CAMERA_ORBITAL);

    BeginMode3D(camera);

    DrawPlane({0, 0, 0}, {40, 40}, {90, 60, 40, 255});
    DrawModel(boardModel, {0, -0.1f, 0}, 1.0f, WHITE);

    for(int q = -4; q <= 4; q++)
    {
        for(int r = -4; r <= 4; r++)
        {
            int s = -q-r;
            if(abs(q) > 4 || abs(r) > 4 || abs(s) > 4) continue;

            Vector3 pos = hexToWorld(q, r);

            DrawModel(hexModel, pos, 1.0f, BLACK);

            Position p(q, r);
            auto marble = board.get(p);
            if(marble == Cell::White) {
                Vector3 marblePos = pos;
                marblePos.y = 0.6f;
                DrawModel(marbleWhite, marblePos, 1.0f, WHITE);
            }
            if(marble == Cell::Black) {
                Vector3 marblePos = pos;
                marblePos.y = 0.6f;
                DrawModel(marbleBlack, marblePos, 1.0f, GRAY);
            }
        }
    }

    EndMode3D();
}