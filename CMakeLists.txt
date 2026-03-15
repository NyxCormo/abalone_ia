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
        src/core/Position.cpp
        src/core/Board.cpp
        src/core/Move.cpp
        src/core/MoveGenerator.cpp
        src/core/Game_Rules.cpp
        src/core/InputHandler.cpp
)

set(RENDER_SOURCES
        src/render/Renderer.cpp
)

add_executable(Abalone
        src/main.cpp
        ${CORE_SOURCES}
        ${RENDER_SOURCES}
)

target_include_directories(Abalone PRIVATE src)
target_link_libraries(Abalone PRIVATE raylib)