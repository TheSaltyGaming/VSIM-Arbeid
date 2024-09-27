﻿#include "Math.h"

Math::Math()
{
    
}

/// \brief Moves a mesh towards a target mesh
/// \param mesh the mesh that will be moved
/// \param target destination mesh
/// \param speed movement speed
void Math::moveObject(Mesh* mesh, Mesh* target, float speed)
{
    
    // Get the current target point
    glm::vec3 readposition = mesh->globalPosition;


    glm::vec3 targetPoint = target->globalPosition;

    // Check if the NPC is close enough to the target point
    float distanceToTarget = glm::distance(mesh->globalPosition, targetPoint);
    if (distanceToTarget < 0.1f) {
        
        return;
    }


    // Calculate the direction vector from the NPC to the target point
    glm::vec3 direction = glm::normalize(targetPoint - readposition);

    // Move NPC towards target
    glm::vec3 tempposition = mesh->globalPosition;
    tempposition += direction * speed;
    mesh->globalPosition = tempposition;
    
    }

void Math::moveObject(Mesh* mesh, std::vector<glm::vec3> pointList, float speed)
{
    // Get the current target point
    glm::vec3 readposition = mesh->globalPosition;


    glm::vec3 targetPoint = pointList[pointIndex+1];


    // Calculate the direction vector from the NPC to the target point
    glm::vec3 direction = glm::normalize(targetPoint - readposition);

    // Move NPC towards target
    glm::vec3 tempposition = mesh->globalPosition;
    tempposition += direction * speed;
    mesh->globalPosition = tempposition;

    // Check if the NPC is close enough to the target point
    float distanceToTarget = glm::distance(mesh->globalPosition, targetPoint);
    if (distanceToTarget < 0.1f) { // Threshold value
        // Move to the next point
        pointIndex++;

        // Loop back to start
        if (pointIndex >= pointList.size()-1) {
            pointIndex = 0;
        }
    }
}

glm::vec3 Math::barycentricCoordinates(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 P)
{
    glm::vec3 v0 = B - A, v1 = C - A, v2 = P - A;
    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return glm::vec3(u, v, w);
}

float Math::calculateHeightUsingBarycentric2(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C,
    const glm::vec3& P)
{
    // Calculate barycentric coordinates for P within the triangle ABC.
    glm::vec3 baryCoords = barycentricCoordinates(A, B, C, P);

    // Interpolate Y cood at P.
    float height = A.y * baryCoords.x + B.y * baryCoords.y + C.y * baryCoords.z;
    return height;
}

bool Math::isPointAboveTriangle(const TriangleStruct& triangle, const glm::vec3& point)
{
    // Calculate barycentric coordinates for the point with respect to the triangle
    glm::vec3 baryCoords = barycentricCoordinates(triangle.v0, triangle.v1, triangle.v2, point);

    // Check if the point lies within the triangle
    if (baryCoords.x >= 0 && baryCoords.x <= 1 &&
        baryCoords.y >= 0 && baryCoords.y <= 1 &&
        baryCoords.z >= 0 && baryCoords.z <= 1)
    {
        // Check if the point is above the triangle
        if (point.y > triangle.v0.y && point.y > triangle.v1.y && point.y > triangle.v2.y)
        {
            return true;
        }
    }

    return false;
}

void Math::MapPlayerToSurface(Surface* surface, Mesh& MainMesh, float deltaTime)
{
    
    float x = MainMesh.globalPosition.x;
    float z = MainMesh.globalPosition.z;

    
    for (const TriangleStruct& triangle : surface->triangles)
    {
        // Check if the camera's position is above the triangle
        if (isPointAboveTriangle(triangle, glm::vec3(x, MainMesh.globalPosition.y, z)))
        {
            // Calculate the barycentric coordinates of the camera's position within the triangle
            glm::vec3 barycentricCoords = barycentricCoordinates(triangle.v0, triangle.v1, triangle.v2, glm::vec3(x, MainMesh.globalPosition.y, z));

            // Calculate the height at the camera's position by interpolating the heights of the triangle's vertices
            float height = calculateHeightUsingBarycentric2(triangle.v0, triangle.v1, triangle.v2, glm::vec3(x, MainMesh.globalPosition.y, z));

            float interpolationSpeed = 6.f* deltaTime;
            MainMesh.globalPosition.y += ((height - MainMesh.globalPosition.y+1.f) * interpolationSpeed);

            
            break;
        }
    }
}

glm::vec3 Math::RandomVec3(float min, float max)
{
    float x = min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
    float y = min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
    float z = min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
    return glm::vec3(x, y, z);
}

glm::vec3 Math::deCasteljau(std::vector<glm::vec3> points, float t)
{
    for (int i = 0; i < points.size() - 1; i++)
    {
        for (int j = 0; j < points.size() - i - 1; j++)
        {
            points[j] = points[j] * (1 - t)  + points[j + 1] * t;
        }
    }
    return points[0];
}



