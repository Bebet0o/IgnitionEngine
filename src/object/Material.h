#pragma once
#include <glad/glad.h>
#include <string>

class Material {
public:
    Material(GLuint shader, GLuint texture = 0)
        : m_Shader(shader), m_Texture(texture) {}
    GLuint GetShader() const { return m_Shader; }
    GLuint GetTexture() const { return m_Texture; }
private:
    GLuint m_Shader;
    GLuint m_Texture;
};
