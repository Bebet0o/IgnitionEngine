#pragma once
#include "object/Scene.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class ViewportPanel
{
public:
    ViewportPanel();
    ~ViewportPanel();

    void Render(Scene* scene);

    unsigned int GetFramebufferTexture() const { return m_FramebufferTexture; }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    void CreateFramebuffer(int width, int height);
    void DestroyFramebuffer();
    void ViewportPanel::DrawGameObjectRecursive(GameObject* obj, const glm::mat4& view, const glm::mat4& proj);

    unsigned int m_Framebuffer = 0;
    unsigned int m_FramebufferTexture = 0;
    unsigned int m_Renderbuffer = 0;

    Scene* m_Scene = nullptr;
    int m_Width = 0;
    int m_Height = 0;
    bool m_NeedsResize = true;
};
