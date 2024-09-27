#pragma once
#include <vector>
#include "glm/vec3.hpp"
#include "../Vertex.h"

struct Vertex;

class Surface
{
public:
    Surface();
    Surface(int size, glm::vec3 color);

    void Setup();
    void Draw(unsigned int shaderProgram);

    glm::vec3 RandomColor();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    std::vector<TriangleStruct> triangles;
    int size = 10;

    unsigned int VBO, VAO, EBO;

    glm::vec3 globalPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    
    
};
