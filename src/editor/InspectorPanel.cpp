#include "InspectorPanel.h"
#include <imgui.h>
#include "object/Transform.h"
#include "object/RendererComponent.h"
#include "object/Mesh.h"
#include <functional>
#include <vector>
#include <string.h>

struct ComponentTypeInfo {
    std::string name;
    std::function<void(GameObject*)> addToGameObject;
    std::function<bool(GameObject*)> canAddToGameObject;
};

GLuint g_DefaultShader = 0;
GLuint g_DefaultTexture = 0;

inline std::vector<ComponentTypeInfo> GetAllComponentTypes() {
    return {
        {
            "Renderer",
            [](GameObject* obj) {
                Mesh* mesh = Mesh::CreateCube();
                Material* material = new Material(g_DefaultShader, g_DefaultTexture);
                obj->AddComponent<RendererComponent>(mesh, material);
            },
            [](GameObject* obj) -> bool {
                return obj->GetComponent<RendererComponent>() == nullptr;
            }
        },
    };
}

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

        for (const auto& compPtr : m_Target->GetAllComponents())
        {
            Component* comp = compPtr.get();
            std::string type = comp->GetName();

            if (type == "Transform")
            {
                auto* transform = dynamic_cast<Transform*>(comp);
                if (transform)
                {
                    ImGui::Separator();
                    ImGui::Text("Transform");
                    ImGui::DragFloat3("Position", (float*)&(transform->position), 0.1f);
                    ImGui::DragFloat3("Rotation", (float*)&(transform->rotation), 0.1f);
                    ImGui::DragFloat3("Scale",    (float*)&(transform->scale),    0.1f);
                }
            }
            else if (type == "Renderer")
            {
                auto* renderer = dynamic_cast<RendererComponent*>(comp);
                if (renderer)
                {
                    ImGui::Separator();
                    ImGui::Text("Renderer");
                    ImGui::Text("Mesh: %p", renderer->GetMesh());
                    ImGui::Text("Material: %p", renderer->GetMaterial());
                }
            }
        }


        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponentPopup");

        if (ImGui::BeginPopup("AddComponentPopup"))
        {
            auto allTypes = GetAllComponentTypes();
            for (const auto& info : allTypes)
            {
                if (info.canAddToGameObject(m_Target))
                {
                    if (ImGui::MenuItem(info.name.c_str()))
                    {
                        info.addToGameObject(m_Target);
                        ImGui::CloseCurrentPopup();
                    }
                }
            }
            ImGui::EndPopup();
        }
    }

    ImGui::End();
}
