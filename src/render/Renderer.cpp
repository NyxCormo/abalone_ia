#include "Renderer.h"
#include <cmath>

#include "MeshGenerator.h"

Renderer::Renderer()
    : camera{},
      boardModel{},
      hexModel{},
      marbleWhite{},
      marbleBlack{},
      whiteMarbleTex{},
      blackMarbleTex{},
      hexWood{},
      tableTexture{}
{
    camera.position = {0.0f, 0.0f, 0.0f};
    camera.target = {0.0f, 0.0f, 0.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    whiteMarbleTex = LoadTexture("../assets/textures/whiteMarble.png");
    blackMarbleTex = LoadTexture("../assets/textures/blackMarble.png");
    hexWood = LoadTexture("../assets/textures/hexWood.png");
    tableTexture = LoadTexture("../assets/textures/tableTexture.png");

    Mesh baseMesh = GenMeshCylinderWithUVs(10.0f, 0.2f, 50);
    boardModel = LoadModelFromMesh(baseMesh);
    boardModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
    boardModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tableTexture;

    Mesh hexMesh = GenMeshCylinderWithUVs(1.0f, 0.2f, 6);
    hexModel = LoadModelFromMesh(hexMesh);
    hexModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
    hexModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = hexWood;

    Mesh marbleMesh = GenMeshSphere(0.5f, 32, 32);

    marbleWhite = LoadModelFromMesh(marbleMesh);
    marbleWhite.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = whiteMarbleTex;
    marbleWhite.materials[0].maps[MATERIAL_MAP_SPECULAR].color = WHITE;

    marbleBlack = LoadModelFromMesh(marbleMesh);
    marbleBlack.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = blackMarbleTex;
    marbleBlack.materials[0].maps[MATERIAL_MAP_SPECULAR].color = WHITE;
}

Renderer::~Renderer() {
    UnloadModel(boardModel);
    UnloadModel(hexModel);
    UnloadModel(marbleWhite);
    UnloadModel(marbleBlack);
    UnloadTexture(whiteMarbleTex);
    UnloadTexture(blackMarbleTex);
    UnloadTexture(hexWood);
    UnloadTexture(tableTexture);
}

Vector3 Renderer::hexToWorld(int q, int r) {
    constexpr float HEX_SIZE = 1.2f;
    float x = HEX_SIZE * std::sqrt(3.0f) * (static_cast<float>(q) + static_cast<float>(r)/2.0f);
    float z = HEX_SIZE * 1.5f * static_cast<float>(r);
    return {x, 0.0f, z};
}

Vector2 Renderer::worldToScreen(Vector3 worldPos, const Camera3D &cam) {
    return GetWorldToScreen(worldPos, cam);
}

bool Renderer::isPointInHex(Vector2 point, Vector2 hexCenter, float radius) {
    float dx = point.x - hexCenter.x;
    float dy = point.y - hexCenter.y;
    return (dx*dx + dy*dy) <= (radius * radius);
}

void Renderer::draw(const Board& board) {
    constexpr float CAMERA_DISTANCE = 20.0f;
    constexpr float CAMERA_ANGLE_SPEED = 0.6f;

    static float cameraAngleH = 0.0f;
    static float cameraAngleV = 0.7f;

    if (IsKeyDown(KEY_LEFT)) {
        cameraAngleH += CAMERA_ANGLE_SPEED * GetFrameTime();
    }
    if (IsKeyDown(KEY_RIGHT)) {
        cameraAngleH -= CAMERA_ANGLE_SPEED * GetFrameTime();
    }

    if (IsKeyDown(KEY_DOWN)) {
        cameraAngleV += CAMERA_ANGLE_SPEED * GetFrameTime();
    }
    if (IsKeyDown(KEY_UP)) {
        cameraAngleV -= CAMERA_ANGLE_SPEED * GetFrameTime();
    }

    constexpr float MIN_VERTICAL = 0.1f;
    constexpr float MAX_VERTICAL = 3.04f;
    if (cameraAngleV < MIN_VERTICAL) cameraAngleV = MIN_VERTICAL;
    if (cameraAngleV > MAX_VERTICAL) cameraAngleV = MAX_VERTICAL;

    camera.position.x = CAMERA_DISTANCE * std::sin(cameraAngleV) * std::sin(cameraAngleH);
    camera.position.y = CAMERA_DISTANCE * std::cos(cameraAngleV);
    camera.position.z = CAMERA_DISTANCE * std::sin(cameraAngleV) * std::cos(cameraAngleH);

    camera.target = {0.0f, 0.0f, 0.0f};

    BeginMode3D(camera);

    DrawModel(boardModel, {0.0f, -0.1f, 0.0f}, 1.0f, WHITE);

    for(int q = -4; q <= 4; q++) {
        for(int r = -4; r <= 4; r++) {
            Position pos(q, r);
            if(!pos.isValid()) continue;

            Vector3 worldPos = hexToWorld(q, r);
            DrawModel(hexModel, worldPos, 1.0f, {200, 180, 140, 255});
        }
    }

    for(int q = -4; q <= 4; q++) {
        for(int r = -4; r <= 4; r++) {
            Position pos(q, r);
            if(!pos.isValid()) continue;

            Cell cell = board.get(pos);
            if(cell == Cell::Empty) continue;

            Vector3 worldPos = hexToWorld(q, r);
            worldPos.y = 0.6f;

            if(cell == Cell::Black) {
                DrawModel(marbleBlack, worldPos, 1.0f, WHITE);
            } else if(cell == Cell::White) {
                DrawModel(marbleWhite, worldPos, 1.0f, WHITE);
            }
        }
    }

    EndMode3D();

    drawGlobalDirectionRose();
}

std::optional<Position> Renderer::getClickedHex(Vector2 mousePos) const {
    float minDist = 1000.0f;
    std::optional<Position> closest = std::nullopt;

    for(int q = -4; q <= 4; q++) {
        for(int r = -4; r <= 4; r++) {
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

void Renderer::drawSelectionWheel(Position center, const std::vector<Direction>& directions) const {
    Vector3 worldPos = hexToWorld(center.q(), center.r());
    worldPos.y = 1.5f;

    Vector2 screenPos = worldToScreen(worldPos, camera);

    float wheelRadius = 80.0f;

    DrawCircle(
        static_cast<int>(screenPos.x),
        static_cast<int>(screenPos.y),
        wheelRadius + 5.0f,
        {0,0,0,100}
    );

    DrawCircle(
        static_cast<int>(screenPos.x),
        static_cast<int>(screenPos.y),
        wheelRadius,
        {255,255,255,200}
    );

    for(Direction dir : directions) {
        int dq = 0, dr = 0;
        switch(dir) {
            case Direction::EAST:       dq = 1; dr = 0; break;
            case Direction::NORTH_EAST: dq = 1; dr = -1; break;
            case Direction::NORTH_WEST: dq = 0; dr = -1; break;
            case Direction::WEST:       dq = -1; dr = 0; break;
            case Direction::SOUTH_WEST: dq = -1; dr = 1; break;
            case Direction::SOUTH_EAST: dq = 0; dr = 1; break;
        }

        Vector3 neighbor = hexToWorld(center.q() + dq, center.r() + dr);
        Vector3 worldDir;
        worldDir.x = neighbor.x - worldPos.x;
        worldDir.y = neighbor.y - worldPos.y;
        worldDir.z = neighbor.z - worldPos.z;

        Vector3 worldTarget;
        worldTarget.x = worldPos.x + worldDir.x;
        worldTarget.y = worldPos.y + worldDir.y;
        worldTarget.z = worldPos.z + worldDir.z;

        Vector2 screenDir = worldToScreen(worldTarget, camera);
        float angle = std::atan2(screenDir.y - screenPos.y, screenDir.x - screenPos.x);

        float buttonX = screenPos.x + std::cos(angle) * (wheelRadius - 20.0f);
        float buttonY = screenPos.y + std::sin(angle) * (wheelRadius - 20.0f);

        DrawCircle(
            static_cast<int>(buttonX),
            static_cast<int>(buttonY),
            25.0f,
            {100,200,100,255}
        );

        DrawCircleLines(
            static_cast<int>(buttonX),
            static_cast<int>(buttonY),
            25.0f,
            BLACK
        );

        const char* label = "";
        switch(dir) {
            case Direction::EAST: label = "E"; break;
            case Direction::NORTH_EAST: label = "NE"; break;
            case Direction::NORTH_WEST: label = "NW"; break;
            case Direction::WEST: label = "W"; break;
            case Direction::SOUTH_WEST: label = "SW"; break;
            case Direction::SOUTH_EAST: label = "SE"; break;
        }

        DrawText(
            label,
            static_cast<int>(buttonX) - 10,
            static_cast<int>(buttonY) - 8,
            16,
            BLACK
        );
    }
}

std::optional<Direction> Renderer::getClickedDirection(Position hexPos, Vector2 mousePos) const {
    Vector3 worldPos = hexToWorld(hexPos.q(), hexPos.r());
    worldPos.y = 1.5f;

    Vector2 screenPos = worldToScreen(worldPos, camera);

    for(int i = 0; i < 6; i++) {
        float wheelRadius = 80.0f;
        auto dir = static_cast<Direction>(i);

        int dq = 0, dr = 0;
        switch(dir) {
            case Direction::EAST:       dq = 1; dr = 0; break;
            case Direction::NORTH_EAST: dq = 1; dr = -1; break;
            case Direction::NORTH_WEST: dq = 0; dr = -1; break;
            case Direction::WEST:       dq = -1; dr = 0; break;
            case Direction::SOUTH_WEST: dq = -1; dr = 1; break;
            case Direction::SOUTH_EAST: dq = 0; dr = 1; break;
        }

        Vector3 neighbor = hexToWorld(hexPos.q() + dq, hexPos.r() + dr);
        Vector3 worldDir;
        worldDir.x = neighbor.x - worldPos.x;
        worldDir.y = neighbor.y - worldPos.y;
        worldDir.z = neighbor.z - worldPos.z;

        Vector3 worldTarget;
        worldTarget.x = worldPos.x + worldDir.x;
        worldTarget.y = worldPos.y + worldDir.y;
        worldTarget.z = worldPos.z + worldDir.z;

        Vector2 screenDir = worldToScreen(worldTarget, camera);
        float angle = std::atan2(screenDir.y - screenPos.y, screenDir.x - screenPos.x);

        float buttonX = screenPos.x + std::cos(angle) * (wheelRadius - 20.0f);
        float buttonY = screenPos.y + std::sin(angle) * (wheelRadius - 20.0f);

        float dx = mousePos.x - buttonX;
        float dy = mousePos.y - buttonY;
        float dist = std::sqrt(dx*dx + dy*dy);

        if(dist <= 25.0f) {
            return dir;
        }
    }

    return std::nullopt;
}

void Renderer::drawGlobalDirectionRose() const {
    int screenWidth = GetScreenWidth();

    Vector2 centerScreen = {static_cast<float>(screenWidth - 100), 100.0f};
    float radius = 60.0f;

    DrawCircle(static_cast<int>(centerScreen.x), static_cast<int>(centerScreen.y), radius + 5, {0,0,0,150});
    DrawCircle(static_cast<int>(centerScreen.x), static_cast<int>(centerScreen.y), radius, {200,200,200,200});

    Vector3 centerWorld = hexToWorld(0, 0);
    centerWorld.y = 0.0f;

    for(int i = 0; i < 6; i++) {
        const char* labels[6] = {"E","NE","NW","W","SW","SE"};
        constexpr int dq[6] = {1, 1, 0, -1, -1, 0};
        constexpr int dr[6] = {0, -1, -1, 0, 1, 1};

        Vector3 neighborWorld = hexToWorld(dq[i], dr[i]);
        neighborWorld.y = 0.0f;

        Vector2 neighborScreen = worldToScreen(neighborWorld, camera);
        Vector2 centerScreenWorld = worldToScreen(centerWorld, camera);

        float angle = std::atan2(neighborScreen.y - centerScreenWorld.y,
                                 neighborScreen.x - centerScreenWorld.x);

        float buttonX = centerScreen.x + std::cos(angle) * (radius - 15.0f);
        float buttonY = centerScreen.y + std::sin(angle) * (radius - 15.0f);

        DrawLine(static_cast<int>(centerScreen.x), static_cast<int>(centerScreen.y),
                 static_cast<int>(buttonX), static_cast<int>(buttonY),
                 {100,100,255,200});

        DrawCircle(static_cast<int>(buttonX), static_cast<int>(buttonY), 10.0f, {50,200,50,220});

        DrawText(labels[i], static_cast<int>(buttonX)-6, static_cast<int>(buttonY)-6, 14, BLACK);
    }
}