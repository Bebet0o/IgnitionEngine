#pragma once

class ViewportPanel
{
public:
    ViewportPanel();
    ~ViewportPanel();

    void Render();

    unsigned int GetFramebufferTexture() const { return m_FramebufferTexture; }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    void CreateFramebuffer(int width, int height);
    void DestroyFramebuffer();

    unsigned int m_Framebuffer = 0;
    unsigned int m_FramebufferTexture = 0;
    unsigned int m_Renderbuffer = 0;

    int m_Width = 0;
    int m_Height = 0;
    bool m_NeedsResize = true;
};
