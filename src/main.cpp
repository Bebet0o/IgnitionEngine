#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>

#include "tinyfiledialogs.h"

#include "editor/HierarchyPanel.h"
#include "editor/Library.h"
#include "editor/InspectorPanel.h"
#include "editor/ViewportPanel.h"

#include "object/Scene.h"

// Taille initiale de la fenêtre
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;

static Scene g_Scene("No Scene");
static HierarchyPanel g_HierarchyPanel;
static LibraryPanel libraryPanel("assets");
static InspectorPanel g_InspectorPanel;

static std::string g_LastScenePath = "";

extern GLuint g_DefaultTexture;
extern GLuint g_DefaultShader;

void CreateDefaultTexture()
{
    unsigned char white[4] = { 255, 255, 255, 255 };
    glGenTextures(1, &g_DefaultTexture);
    glBindTexture(GL_TEXTURE_2D, g_DefaultTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint CompileShader(const char* vertexSrc, const char* fragmentSrc)
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "[Vertex Shader] Compile error:\n" << infoLog << std::endl;
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "[Fragment Shader] Compile error:\n" << infoLog << std::endl;
    }

    // Link program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "[Shader Program] Link error:\n" << infoLog << std::endl;
    }

    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Exemple de shader ultra simple
const char* defaultVertexShader = R"(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aNormal;
    layout(location = 2) in vec2 aUV;
    uniform mat4 u_Model;
    uniform mat4 u_View;
    uniform mat4 u_Projection;
    out vec2 vUV;
    void main() {
        vUV = aUV;
        gl_Position = u_Projection * u_View * u_Model * vec4(aPos, 1.0);
    }
)";
const char* defaultFragmentShader = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 vUV;
    uniform sampler2D u_Texture;
    void main() {
        FragColor = texture(u_Texture, vUV);
    }
)";

void ShowMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
                g_Scene.Clear();
                g_HierarchyPanel.SetScene(&g_Scene);
                g_Scene.name = "Untitled";
                g_LastScenePath = "";
            }
            if (ImGui::MenuItem("Open Scene...", "Ctrl+O")) {
                const char* filterPatterns[] = { "*.json" };
                const char* path = tinyfd_openFileDialog(
                    "Open Scene",
                    "assets/",
                    1, filterPatterns, NULL, 0);

                if (path)
                {
                    g_Scene.LoadFromFile(path);
                    g_HierarchyPanel.SetScene(&g_Scene);
                    g_LastScenePath = path;
                }
            }
             if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
                const char* path = g_LastScenePath.c_str();
                if (path)
                {
                    g_Scene.SaveToFile(path);
                }
            }
            if (ImGui::MenuItem("Save Scene As", "Ctrl+Shift+S")) {
                const char* filterPatterns[] = { "*.json" };
                const char* path = tinyfd_saveFileDialog(
                    "Save Scene",
                    "assets/scene.json",
                    1, filterPatterns, NULL);

                if (path)
                {
                    g_Scene.SaveToFile(path);
                    g_LastScenePath = path;
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                // TODO: Quitter proprement
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "Ctrl+X")) {}
            if (ImGui::MenuItem("Copy", "Ctrl+C")) {}
            if (ImGui::MenuItem("Paste", "Ctrl+V")) {}
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Library")) {
                // TODO: Afficher/cacher le panneau Library
            }
            if (ImGui::MenuItem("Hierarchy")) {
                // TODO: Afficher/cacher le panneau Hierarchy
            }
            if (ImGui::MenuItem("Inspector")) {
                // TODO: Afficher/cacher le panneau Inspector
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About")) {
                // TODO: Afficher la fenêtre "About"
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImGuiIO& io = ImGui::GetIO();

    // Ctrl+S → Save
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) && io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false))
    {
        const char* path = g_LastScenePath.c_str();
            if (path)
            {
                g_Scene.SaveToFile(path);
            }
    }

    // Ctrl+Shift+S → Save as
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) && io.KeyCtrl && io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_S, false))
    {
        const char* filterPatterns[] = { "*.json" };
        const char* path = tinyfd_saveFileDialog(
            "Save Scene",
            "assets/scene.json",
            1, filterPatterns, NULL);

        if (path)
        {
            g_Scene.SaveToFile(path);
            g_LastScenePath = path;
        }
    }

    // Ctrl+O → Open
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) && io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_O, false))
    {
        const char* filterPatterns[] = { "*.json" };
        const char* path = tinyfd_openFileDialog(
            "Open Scene",
            "assets/",
            1, filterPatterns, NULL, 0);

        if (path)
        {
            g_Scene.LoadFromFile(path);
            g_HierarchyPanel.SetScene(&g_Scene);
            g_LastScenePath = path;
        }
    }

    // Ctrl+N → New
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) && io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_N, false))
    {
        g_Scene.Clear();
        g_HierarchyPanel.SetScene(&g_Scene);
        g_Scene.name = "Untitled";
        g_LastScenePath = "";
    }

}

void ShowMainDockSpace()
{
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // On désactive le background pour que la couleur OpenGL soit visible
    window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::Begin("IgnitionEngineDockSpace", nullptr, window_flags);

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Important : fait le dockspace sur toute la fenêtre principale
    ImGuiID dockspace_id = ImGui::GetID("IgnitionEngineDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    ImGui::End();
}

int main() {
    // 1. Initialisation GLFW
    if (!glfwInit()) {
        std::cerr << "Erreur: Impossible d'initialiser GLFW\n";
        return -1;
    }

    // 2. Contexte OpenGL 3.3+
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 3. Création fenêtre
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Ignition Editor", nullptr, nullptr);
    if (!window) {
        std::cerr << "Erreur: Impossible de créer la fenêtre GLFW\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 4. Chargement GLAD (OpenGL loader)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erreur: Impossible d'initialiser GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    CreateDefaultTexture();
    g_DefaultShader = CompileShader(defaultVertexShader, defaultFragmentShader);

    // 5. Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    static ViewportPanel g_ViewportPanel;
    
    // 7. Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll GLFW events
        glfwPollEvents();

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ShowMainDockSpace();
        ShowMainMenuBar();

        // -- PANELS DE L'EDITEUR --
        libraryPanel.Render();
        g_HierarchyPanel.Render();
        g_ViewportPanel.Render(&g_Scene);

        GameObject* selected = g_HierarchyPanel.GetSelectedObject();
        g_InspectorPanel.SetTarget(selected);
        g_InspectorPanel.Render();

        // -- RENDER IMGUI --
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.13f, 0.14f, 0.16f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // 8. Cleanup ImGui/GLFW
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
