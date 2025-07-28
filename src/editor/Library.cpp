#include "Library.h"
#include <imgui.h>
#include <filesystem>

namespace fs = std::filesystem;

LibraryPanel::LibraryPanel(const std::string& assetsDir)
    : m_assetsDir(assetsDir)
{
    Refresh();
}

void LibraryPanel::Refresh()
{
    m_files.clear();
    if (!fs::exists(m_assetsDir)) return;

    for (const auto& entry : fs::directory_iterator(m_assetsDir))
    {
        LibraryFile file;
        file.name = entry.path().filename().string();
        file.path = entry.path().string();
        file.isDirectory = entry.is_directory();
        m_files.push_back(file);
    }
}

void LibraryPanel::Render()
{
    ImGui::Begin("Library");

    if (ImGui::Button("Refresh"))
        Refresh();

    ImGui::Separator();

    for (int i = 0; i < m_files.size(); ++i)
    {
        const auto& file = m_files[i];
        std::string displayName = (file.isDirectory ? "[DIR] " : "") + file.name;

        ImGui::PushID(i);
        if (ImGui::Selectable(displayName.c_str(), m_selected == i))
        {
            m_selected = i;
        }
        ImGui::PopID();
    }

    if (m_selected != -1)
    {
        ImGui::Separator();
        ImGui::Text("Selected: %s", m_files[m_selected].name.c_str());
    }

    ImGui::End();
}
