#include "Renderer.h"
#include <cmath>
#include <raymath.h>

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

    marbleBlack = LoadModelFromMesh(marbleMesh);
    marbleBlack.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = blackMarbleTex;
    marbleBlack.materials[0].maps[MATERIAL_MAP_SPECULAR].color = WHITE;
}

Renderer::~Renderer()
{
    UnloadTexture(whiteMarbleTex);
    UnloadTexture(blackMarbleTex);
    UnloadTexture(hexWood);
    UnloadTexture(tableWood);

    UnloadModel(boardModel);
    UnloadModel(hexModel);
    UnloadModel(marbleWhite);
    UnloadModel(marbleBlack);
}

Vector3 Renderer::hexToWorld(int q, int r)
{
    float size = 1.2f;
    float x = size * std::sqrt(3.0f) * (q + r/2.0f);
    float z = size * 1.5f * r;
    return {x, 0, z};
}

Vector2 Renderer::worldToScreen(Vector3 worldPos, Camera3D cam)
{
    return GetWorldToScreen(worldPos, cam);
}

bool Renderer::isPointInHex(Vector2 point, Vector2 hexCenter, float radius)
{
    float dx = point.x - hexCenter.x;
    float dy = point.y - hexCenter.y;
    float dist = std::sqrt(dx*dx + dy*dy);
    return dist <= radius;
}

void Renderer::draw(const Board& board)
{
    camera.position = {0, 20, -10};
    camera.target = {0, 0, 0};


    BeginMode3D(camera);

    DrawPlane({0, 0, 0}, {40, 40}, {90, 60, 40, 255});
    DrawModel(boardModel, {0, -0.1f, 0}, 1.0f, WHITE);

    for(int q = -4; q <= 4; q++)
    {
        for(int r = -4; r <= 4; r++)
        {
            Position p(q, r);
            if(!p.isValid()) continue;

            Vector3 pos = hexToWorld(q, r);
            DrawModel(hexModel, pos, 1.0f, {150, 100, 50, 255});

            Cell marble = board.get(p);
            if(marble == Cell::White) {
                Vector3 marblePos = pos;
                marblePos.y = 0.6f;
                DrawModel(marbleWhite, marblePos, 1.0f, WHITE);
            }
            if(marble == Cell::Black) {
                Vector3 marblePos = pos;
                marblePos.y = 0.6f;
                DrawModel(marbleBlack, marblePos, 1.0f, {30, 30, 30, 255});
            }
        }
    }

    EndMode3D();
}

std::optional<Position> Renderer::getClickedHex(Vector2 mousePos)
{
    float minDist = 1000.0f;
    std::optional<Position> closest = std::nullopt;

    for(int q = -4; q <= 4; q++)
    {
        for(int r = -4; r <= 4; r++)
        {
            Position p(q, r);
            if(!p.isValid()) continue;

            Vector3 worldPos = hexToWorld(q, r);
            Vector2 screenPos = worldToScreen(worldPos, camera);

            float dx = mousePos.x - screenPos.x;
            float dy = mousePos.y - screenPos.y;
            float dist = std::sqrt(dx*dx + dy*dy);

            if(dist < 40.0f && dist < minDist) {
                minDist = dist;
                closest = p;
            }
        }
    }

    return closest;
}

void Renderer::drawSelectionWheel(Position center, const std::vector<Direction>& directions)
{
    Vector3 worldPos = hexToWorld(center.q(), center.r());
    worldPos.y = 1.5f;
    Vector2 screenPos = worldToScreen(worldPos, camera);

    float wheelRadius = 80.0f;
    DrawCircle(screenPos.x, screenPos.y, wheelRadius + 5, {0, 0, 0, 100});
    DrawCircle(screenPos.x, screenPos.y, wheelRadius, {255, 255, 255, 200});

    for(size_t i = 0; i < directions.size(); i++)
    {
        Direction dir = directions[i];
        float angle = static_cast<int>(dir) * 60.0f * DEG2RAD;

        float buttonX = screenPos.x + std::cos(angle) * (wheelRadius - 20);
        float buttonY = screenPos.y + std::sin(angle) * (wheelRadius - 20);

        DrawCircle(buttonX, buttonY, 25, {100, 200, 100, 255});
        DrawCircleLines(buttonX, buttonY, 25, BLACK);

        const char* label = "";
        switch(dir) {
            case Direction::EAST: label = "E"; break;
            case Direction::NORTH_EAST: label = "NE"; break;
            case Direction::NORTH_WEST: label = "NW"; break;
            case Direction::WEST: label = "W"; break;
            case Direction::SOUTH_WEST: label = "SW"; break;
            case Direction::SOUTH_EAST: label = "SE"; break;
        }
        DrawText(label, buttonX - 10, buttonY - 8, 16, BLACK);
    }
}

std::optional<Direction> Renderer::getClickedDirection(Position hexPos, Vector2 mousePos)
{
    Vector3 worldPos = hexToWorld(hexPos.q(), hexPos.r());
    worldPos.y = 1.5f;
    Vector2 screenPos = worldToScreen(worldPos, camera);

    float wheelRadius = 80.0f;

    for(int i = 0; i < 6; i++)
    {
        Direction dir = static_cast<Direction>(i);
        float angle = i * 60.0f * DEG2RAD;

        float buttonX = screenPos.x + std::cos(angle) * (wheelRadius - 20);
        float buttonY = screenPos.y + std::sin(angle) * (wheelRadius - 20);

        float dx = mousePos.x - buttonX;
        float dy = mousePos.y - buttonY;
        float dist = std::sqrt(dx*dx + dy*dy);

        if(dist <= 25.0f) {
            return dir;
        }
    }

    return std::nullopt;
}