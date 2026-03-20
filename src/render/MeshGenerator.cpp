#include "MeshGenerator.h"
#include <cmath>

Mesh GenMeshCylinderWithUVs(float radius, float height, int sides) {
    Mesh mesh = {0};
    mesh.triangleCount = sides * 2 + sides + sides;
    mesh.vertexCount = mesh.triangleCount * 3;

    mesh.vertices = static_cast<float *>(MemAlloc(mesh.vertexCount * 3 * sizeof(float)));
    mesh.texcoords = static_cast<float *>(MemAlloc(mesh.vertexCount * 2 * sizeof(float)));
    mesh.normals = static_cast<float *>(MemAlloc(mesh.vertexCount * 3 * sizeof(float)));

    int vIdx = 0;
    int uvIdx = 0;
    int nIdx = 0;
    float halfHeight = height / 2.0f;

    for (int i = 0; i < sides; i++) {
        float angle1 = (2.0f * PI * static_cast<float>(i)) / static_cast<float>(sides);
        float angle2 = (2.0f * PI * static_cast<float>(i + 1)) / static_cast<float>(sides);

        float x1 = radius * cosf(angle1);
        float z1 = radius * sinf(angle1);
        float x2 = radius * cosf(angle2);
        float z2 = radius * sinf(angle2);

        float u1 = static_cast<float>(i) / static_cast<float>(sides);
        float u2 = static_cast<float>(i + 1) / static_cast<float>(sides);


        mesh.vertices[vIdx++] = x1; mesh.vertices[vIdx++] = -halfHeight; mesh.vertices[vIdx++] = z1;
        mesh.texcoords[uvIdx++] = u1; mesh.texcoords[uvIdx++] = 1.0f;
        mesh.normals[nIdx++] = x1 / radius; mesh.normals[nIdx++] = 0.0f; mesh.normals[nIdx++] = z1 / radius;

        mesh.vertices[vIdx++] = x1; mesh.vertices[vIdx++] = halfHeight; mesh.vertices[vIdx++] = z1;
        mesh.texcoords[uvIdx++] = u1; mesh.texcoords[uvIdx++] = 0.0f;
        mesh.normals[nIdx++] = x1 / radius; mesh.normals[nIdx++] = 0.0f; mesh.normals[nIdx++] = z1 / radius;

        mesh.vertices[vIdx++] = x2; mesh.vertices[vIdx++] = halfHeight; mesh.vertices[vIdx++] = z2;
        mesh.texcoords[uvIdx++] = u2; mesh.texcoords[uvIdx++] = 0.0f;
        mesh.normals[nIdx++] = x2 / radius; mesh.normals[nIdx++] = 0.0f; mesh.normals[nIdx++] = z2 / radius;

        mesh.vertices[vIdx++] = x1; mesh.vertices[vIdx++] = -halfHeight; mesh.vertices[vIdx++] = z1;
        mesh.texcoords[uvIdx++] = u1; mesh.texcoords[uvIdx++] = 1.0f;
        mesh.normals[nIdx++] = x1 / radius; mesh.normals[nIdx++] = 0.0f; mesh.normals[nIdx++] = z1 / radius;

        mesh.vertices[vIdx++] = x2; mesh.vertices[vIdx++] = halfHeight; mesh.vertices[vIdx++] = z2;
        mesh.texcoords[uvIdx++] = u2; mesh.texcoords[uvIdx++] = 0.0f;
        mesh.normals[nIdx++] = x2 / radius; mesh.normals[nIdx++] = 0.0f; mesh.normals[nIdx++] = z2 / radius;

        mesh.vertices[vIdx++] = x2; mesh.vertices[vIdx++] = -halfHeight; mesh.vertices[vIdx++] = z2;
        mesh.texcoords[uvIdx++] = u2; mesh.texcoords[uvIdx++] = 1.0f;
        mesh.normals[nIdx++] = x2 / radius; mesh.normals[nIdx++] = 0.0f; mesh.normals[nIdx++] = z2 / radius;
    }

    for (int i = 0; i < sides; i++) {
        float angle1 = (2.0f * PI * static_cast<float>(i)) / static_cast<float>(sides);
        float angle2 = (2.0f * PI * static_cast<float>(i + 1)) / static_cast<float>(sides);
        float x1 = radius * cosf(angle1);
        float z1 = radius * sinf(angle1);
        float x2 = radius * cosf(angle2);
        float z2 = radius * sinf(angle2);

        mesh.vertices[vIdx++] = 0.0f;
        mesh.vertices[vIdx++] = halfHeight;
        mesh.vertices[vIdx++] = 0.0f;
        mesh.vertices[vIdx++] = x2;
        mesh.vertices[vIdx++] = halfHeight;
        mesh.vertices[vIdx++] = z2;
        mesh.vertices[vIdx++] = x1;
        mesh.vertices[vIdx++] = halfHeight;
        mesh.vertices[vIdx++] = z1;

        mesh.texcoords[uvIdx++] = 0.5f;
        mesh.texcoords[uvIdx++] = 0.5f;
        mesh.texcoords[uvIdx++] = 0.5f + 0.5f * cosf(angle2);
        mesh.texcoords[uvIdx++] = 0.5f + 0.5f * sinf(angle2);
        mesh.texcoords[uvIdx++] = 0.5f + 0.5f * cosf(angle1);
        mesh.texcoords[uvIdx++] = 0.5f + 0.5f * sinf(angle1);

        for(int n = 0 ; n < 3 ; n++){
            mesh.normals[nIdx++] = 0.0f;
            mesh.normals[nIdx++] = 1.0f;
            mesh.normals[nIdx++] = 0.0f;
        }
    }

    for (int i = 0; i < sides; i++) {
        float angle1 = (2.0f * PI * static_cast<float>(i)) / static_cast<float>(sides);
        float angle2 = (2.0f * PI * static_cast<float>(i + 1) / static_cast<float>(sides));
        float x1 = radius * cosf(angle1);
        float z1 = radius * sinf(angle1);
        float x2 = radius * cosf(angle2);
        float z2 = radius * sinf(angle2);

        mesh.vertices[vIdx++] = 0.0f; mesh.vertices[vIdx++] = -halfHeight; mesh.vertices[vIdx++] = 0.0f;
        mesh.texcoords[uvIdx++] = 0.5f; mesh.texcoords[uvIdx++] = 0.5f;
        mesh.normals[nIdx++] = 0.0f; mesh.normals[nIdx++] = -1.0f; mesh.normals[nIdx++] = 0.0f;

        mesh.vertices[vIdx++] = x2; mesh.vertices[vIdx++] = -halfHeight; mesh.vertices[vIdx++] = z2;
        mesh.texcoords[uvIdx++] = 0.5f + 0.5f*cosf(angle2); mesh.texcoords[uvIdx++] = 0.5f + 0.5f*sinf(angle2);
        mesh.normals[nIdx++] = 0.0f; mesh.normals[nIdx++] = -1.0f; mesh.normals[nIdx++] = 0.0f;

        mesh.vertices[vIdx++] = x1; mesh.vertices[vIdx++] = -halfHeight; mesh.vertices[vIdx++] = z1;
        mesh.texcoords[uvIdx++] = 0.5f + 0.5f*cosf(angle1); mesh.texcoords[uvIdx++] = 0.5f + 0.5f*sinf(angle1);
        mesh.normals[nIdx++] = 0.0f; mesh.normals[nIdx++] = -1.0f; mesh.normals[nIdx++] = 0.0f;
    }

    UploadMesh(&mesh, false);
    return mesh;
}