#include "InspectorPanel.h"
#include <imgui.h>
#include "../object/Transform.h"

InspectorPanel::InspectorPanel() {}

void InspectorPanel::SetTarget(GameObject* target)
{
    m_Target = target;
}

void InspectorPanel::Render()
{
    ImGui::Begin("Inspector");

    if (!m_Target)
    {
        ImGui::Text("No GameObject selected.");
    }
    else
    {
        static char nameBuffer[128];
        strncpy(nameBuffer, m_Target->name.c_str(), sizeof(nameBuffer));
        nameBuffer[sizeof(nameBuffer) - 1] = 0;

        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
        {
            m_Target->name = nameBuffer;
        }

        ImGui::Separator();

        auto* transform = m_Target->GetComponent<Transform>();
        if (transform)
        {
            ImGui::Text("Transform");
            ImGui::DragFloat3("Position", (float*)&(transform->position), 0.1f);
            ImGui::DragFloat3("Rotation", (float*)&(transform->rotation), 0.1f);
            ImGui::DragFloat3("Scale", (float*)&(transform->scale), 0.1f);
        }

    }

    ImGui::End();
}
