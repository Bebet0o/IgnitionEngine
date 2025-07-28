#include <glad/glad.h>
#include "ViewportPanel.h"
#include <imgui.h>

ViewportPanel::ViewportPanel()
{
    CreateFramebuffer(1280, 720);
}

ViewportPanel::~ViewportPanel()
{
    DestroyFramebuffer();
}

void ViewportPanel::CreateFramebuffer(int width, int height)
{
    DestroyFramebuffer();

    m_Width = width;
    m_Height = height;

    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

    // Texture couleur
    glGenTextures(1, &m_FramebufferTexture);
    glBindTexture(GL_TEXTURE_2D, m_FramebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FramebufferTexture, 0);

    // Renderbuffer profondeur
    glGenRenderbuffers(1, &m_Renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_Renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Renderbuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ViewportPanel::DestroyFramebuffer()
{
    if (m_Renderbuffer) {
        glDeleteRenderbuffers(1, &m_Renderbuffer);
        m_Renderbuffer = 0;
    }
    if (m_FramebufferTexture) {
        glDeleteTextures(1, &m_FramebufferTexture);
        m_FramebufferTexture = 0;
    }
    if (m_Framebuffer) {
        glDeleteFramebuffers(1, &m_Framebuffer);
        m_Framebuffer = 0;
    }
}

void ViewportPanel::Render()
{
    ImGui::Begin("Viewport");

    // Récupère la taille ImGui
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    int newWidth  = (int)viewportPanelSize.x;
    int newHeight = (int)viewportPanelSize.y;

    if ((newWidth != m_Width || newHeight != m_Height) && newWidth > 0 && newHeight > 0)
    {
        CreateFramebuffer(newWidth, newHeight);
    }

    // RENDU DE LA SCENE DANS LE FRAMEBUFFER ----------------
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    glViewport(0, 0, m_Width, m_Height);
    glClearColor(0.1f, 0.15f, 0.23f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ... ici tu dessines ta scène, tes objets, etc.

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // -------------------------------------------------------

    // Affiche la texture du framebuffer dans la fenêtre ImGui (viewport)
    ImGui::Image((void*)(intptr_t)m_FramebufferTexture, viewportPanelSize, ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();
}
