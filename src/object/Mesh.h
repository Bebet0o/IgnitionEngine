#pragma once
#include <vector>
#include <glad/glad.h>

struct Vertex {
    float position[3];
    float normal[3];
    float texcoord[2];
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    void Bind() const;
    void Unbind() const;
    void Draw() const;

    static Mesh* CreateCube();

private:
    GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;
    unsigned int m_IndexCount = 0;
};
