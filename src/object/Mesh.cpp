#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    m_IndexCount = indices.size();

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    // texcoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void Mesh::Bind() const { glBindVertexArray(m_VAO); }
void Mesh::Unbind() const { glBindVertexArray(0); }
void Mesh::Draw() const { glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0); }

// Un cube simple (8 sommets, 36 indices)
Mesh* Mesh::CreateCube() {
    std::vector<Vertex> vertices = {
        // Face avant
        {{-0.5f,-0.5f, 0.5f},{0,0,1},{0,0}}, {{ 0.5f,-0.5f, 0.5f},{0,0,1},{1,0}},
        {{ 0.5f, 0.5f, 0.5f},{0,0,1},{1,1}}, {{-0.5f, 0.5f, 0.5f},{0,0,1},{0,1}},
        // Face arrière
        {{-0.5f,-0.5f,-0.5f},{0,0,-1},{1,0}},{{ 0.5f,-0.5f,-0.5f},{0,0,-1},{0,0}},
        {{ 0.5f, 0.5f,-0.5f},{0,0,-1},{0,1}},{{-0.5f, 0.5f,-0.5f},{0,0,-1},{1,1}}
    };
    std::vector<unsigned int> indices = {
        0,1,2, 2,3,0, // avant
        4,5,6, 6,7,4, // arrière
        0,4,7, 7,3,0, // gauche
        1,5,6, 6,2,1, // droite
        3,2,6, 6,7,3, // haut
        0,1,5, 5,4,0  // bas
    };
    return new Mesh(vertices, indices);
}
