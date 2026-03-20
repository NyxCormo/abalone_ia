cmake_minimum_required(VERSION 3.20)
project(Abalone)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib 5.0 QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            raylib
            URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
    )
    FetchContent_MakeAvailable(raylib)
endif()

set(CORE_SOURCES
        src/geometry/Position.cpp
        src/game/Board.cpp
        src/game/Move.cpp
        src/game/MoveGenerator.cpp
        src/game/Game_Rules.cpp
        src/game/InputHandler.cpp
        src/game/GameState.h
        src/game/GameEngine.cpp
        src/game/GameEngine.h
)

set(RENDER_SOURCES
        src/render/Renderer.cpp
        src/render/MeshGenerator.cpp
        src/render/MeshGenerator.h
        src/render/UI.cpp
        src/render/UI.h
        src/render/Skybox.cpp
        src/render/Skybox.h
)

add_executable(AbaloneApp
        src/main.cpp
        ${CORE_SOURCES}
        ${RENDER_SOURCES}
        src/app/GameApp.cpp
        src/app/GameApp.h
)

target_include_directories(AbaloneApp PRIVATE src)
target_link_libraries(AbaloneApp PRIVATE raylib)