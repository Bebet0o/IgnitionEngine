#include <glad/glad.h>
#include <imgui.h>

#include "ViewportPanel.h"
#include "object/GameObject.h"
#include "object/Scene.h"

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

void ViewportPanel::DrawGameObjectRecursive(GameObject* obj, const glm::mat4& view, const glm::mat4& proj)
{
    auto* renderer = obj->GetComponent<RendererComponent>();
    auto* transform = obj->GetComponent<Transform>();
    if (renderer && transform)
    {
        auto* material = renderer->GetMaterial();
        glUseProgram(material->GetShader());

        // 1. Matrices caméra (caméra de base)
        glm::mat4 view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)m_Width/(float)m_Height, 0.1f, 100.0f);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(
            transform->position.x, transform->position.y, transform->position.z
        ));
        model = glm::rotate(model, glm::radians(transform->rotation.z), glm::vec3(0,0,1));
        model = glm::rotate(model, glm::radians(transform->rotation.x), glm::vec3(0,1,0));
        model = glm::rotate(model, glm::radians(transform->rotation.y), glm::vec3(1,0,0));
        model = glm::scale(model, glm::vec3(transform->scale.x, transform->scale.y, transform->scale.z));

        // 2. Passe les uniforms
        GLint modelLoc = glGetUniformLocation(material->GetShader(), "u_Model");
        GLint viewLoc  = glGetUniformLocation(material->GetShader(), "u_View");
        GLint projLoc  = glGetUniformLocation(material->GetShader(), "u_Projection");
        if (modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        if (viewLoc  != -1) glUniformMatrix4fv(viewLoc,  1, GL_FALSE, glm::value_ptr(view));
        if (projLoc  != -1) glUniformMatrix4fv(projLoc,  1, GL_FALSE, glm::value_ptr(proj));

        // 3. Texture
        if (material->GetTexture()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material->GetTexture());
            GLint texLoc = glGetUniformLocation(material->GetShader(), "u_Texture");
            if (texLoc != -1) glUniform1i(texLoc, 0);
        }

        // 4. Assure toi que le test de profondeur est actif
        glEnable(GL_DEPTH_TEST);

        renderer->GetMesh()->Bind();
        renderer->GetMesh()->Draw();
        renderer->GetMesh()->Unbind();
    }

}

void ViewportPanel::Render(Scene* scene)
{
    m_Scene = scene;
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

    // Matrices caméra de base
    glm::mat4 view = glm::lookAt(
        glm::vec3(0, 0, 3), // position caméra
        glm::vec3(0, 0, 0), // regarde le centre
        glm::vec3(0, 1, 0)  // up
    );
    glm::mat4 proj = glm::perspective(
        glm::radians(45.0f),
        (float)m_Width / (float)m_Height,
        0.1f, 100.0f
    );

    glEnable(GL_DEPTH_TEST);

    for (auto& obj : m_Scene->GetRootGameObjects()) {
        DrawGameObjectRecursive(obj.get(), view, proj);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // -------------------------------------------------------

    // Affiche la texture du framebuffer dans la fenêtre ImGui (viewport)
    ImGui::Image((void*)(intptr_t)m_FramebufferTexture, viewportPanelSize, ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();
}
