cmake_minimum_required(VERSION 4.1)
project(Abalone)

set(CMAKE_CXX_STANDARD 20)

set(CORE
        src/core/Position.cpp
        src/core/Board.cpp
)

add_executable(Abalone src/main.cpp
        ${CORE}
)
