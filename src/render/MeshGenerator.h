#ifndef ABALONE_MESHGENERATOR_H
#define ABALONE_MESHGENERATOR_H


#pragma once
#include "raylib.h"

Mesh GenMeshCylinderWithUVs(float radius, float height, int sides);

Mesh GenMeshSphereWithUVs(float radius, int slices, int stacks);


#endif //ABALONE_MESHGENERATOR_H