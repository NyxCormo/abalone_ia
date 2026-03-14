#include <iostream>
#include <raylib.h>

#include "core/Board.h"
#include "render/Renderer.h"

int main() {
    std::cout << "=== Test Board ===" << std::endl;

    Board board;
    board.setup();

    std::cout << board.toString() << std::endl;

    std::cout << "Billes noires: " << board.countMarbles(Player::Black) << std::endl;
    std::cout << "Billes blanches: " << board.countMarbles(Player::White) << std::endl;


    InitWindow(1200, 800, "Abalone");

    SetTargetFPS(60);

    Renderer renderer;

    while(!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        renderer.draw(board);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}