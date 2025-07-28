#include "HierarchyPanel.h"
#include <imgui.h>

// Pour la suppression différée (safe remove)
static GameObject* s_ObjectToDelete = nullptr;
struct DragDropMoveRequest {
    GameObject* source = nullptr; // Le node qu'on veut déplacer
    GameObject* target = nullptr; // Le nouveau parent (ou nullptr si racine)
};
static DragDropMoveRequest s_PendingMove;

HierarchyPanel::HierarchyPanel() {}

void HierarchyPanel::SetScene(Scene* scene)
{
    m_Scene = scene;
    m_Selected = nullptr;
}

void HierarchyPanel::Render()
{
    ImGui::Begin("Hierarchy");

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_PTR"))
        {
            GameObject* dropped = *(GameObject**)payload->Data;
            // Déjà racine ? On ne fait rien
            if (dropped->GetParent() != nullptr)
            {
                std::unique_ptr<GameObject> moving_ptr = dropped->GetParent()->ExtractChild(dropped);
                if (moving_ptr)
                    m_Scene->AddRootGameObject(std::move(moving_ptr));
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (m_Scene)
    {
        ImGui::Text("Scene: %s", m_Scene->name.c_str());
    }
    else
    {
        ImGui::Text("No Scene");
    }

    ImGui::Separator();

    if (m_Scene)
    {
        auto& roots = m_Scene->GetRootGameObjects();
        for (auto& obj : roots)
            DrawGameObjectNode(obj.get());
    }

    ImVec2 min = ImGui::GetWindowPos();
    ImVec2 max = ImVec2(min.x + ImGui::GetWindowWidth(), min.y + ImGui::GetWindowHeight());
    ImGui::SetCursorScreenPos(min);
    ImGui::InvisibleButton("RootDropZone", ImVec2(max.x - min.x, max.y - min.y - ImGui::GetCursorPosY()));

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_PTR"))
        {
            GameObject* dropped = *(GameObject**)payload->Data;
            if (dropped->GetParent() != nullptr)
            {
                s_PendingMove.source = dropped;
                s_PendingMove.target = nullptr; // Racine
            }
        }
        ImGui::EndDragDropTarget();
    }

    // Menu contextuel sur la fenêtre (pour créer un root)
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Create Empty GameObject")) {
            if (m_Scene) m_Scene->CreateGameObject("GameObject");
        }
        ImGui::EndPopup();
    }

    if (s_PendingMove.source) {
        std::unique_ptr<GameObject> moving_ptr = nullptr;
        if (s_PendingMove.source->GetParent())
            moving_ptr = s_PendingMove.source->GetParent()->ExtractChild(s_PendingMove.source);
        else if (m_Scene)
            moving_ptr = m_Scene->ExtractRootGameObject(s_PendingMove.source);

        if (moving_ptr) {
            if (s_PendingMove.target)
                s_PendingMove.target->AddChild(std::move(moving_ptr));
            else
                m_Scene->AddRootGameObject(std::move(moving_ptr));
        }
        s_PendingMove.source = nullptr;
        s_PendingMove.target = nullptr;
    }

    ImGui::End();

    // Suppression différée (à faire après la frame !)
    if (s_ObjectToDelete) {
        if (s_ObjectToDelete->GetParent())
            s_ObjectToDelete->GetParent()->RemoveChild(s_ObjectToDelete);
        else if (m_Scene)
            m_Scene->RemoveRootGameObject(s_ObjectToDelete);
        s_ObjectToDelete = nullptr;
    }
}

bool IsDescendant(GameObject* obj, GameObject* target)
{
    if (!target) return false;
    for (auto& child : target->GetChildren())
    {
        if (child.get() == obj) return true;
        if (IsDescendant(obj, child.get())) return true;
    }
    return false;
}

void HierarchyPanel::DrawGameObjectNode(GameObject* object)
{
    ImGuiTreeNodeFlags flags = object->GetChildren().empty() ? ImGuiTreeNodeFlags_Leaf : 0;
    if (object == m_Selected)
        flags |= ImGuiTreeNodeFlags_Selected;

    bool nodeOpen = ImGui::TreeNodeEx((void*)object, flags, "%s", object->name.c_str());

    // Clic gauche : sélection (stable)
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        m_Selected = object;

    // Clic droit (menu contextuel GameObject)
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Create Child")) {
            object->AddChild(std::make_unique<GameObject>("Child"));
        }
        if (ImGui::MenuItem("Delete")) {
            s_ObjectToDelete = object;
        }
        ImGui::EndPopup();
    }

    // Drag & drop source
    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("GAMEOBJECT_PTR", &object, sizeof(GameObject*));
        ImGui::Text("Déplacer %s", object->name.c_str());
        ImGui::EndDragDropSource();
    }

    // Drag & drop target
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_PTR"))
        {
            GameObject* dropped = *(GameObject**)payload->Data;
            if (dropped != object && dropped->GetParent() != object && !IsDescendant(object, dropped))
            {
                s_PendingMove.source = dropped;
                s_PendingMove.target = object;
            }
        }
        ImGui::EndDragDropTarget();
    }


    // Affichage récursif enfants
    if (nodeOpen) {
        for (auto& child : object->GetChildren())
            DrawGameObjectNode(child.get());
        ImGui::TreePop();
    }
}

GameObject* HierarchyPanel::GetSelectedObject() const
{
    return m_Selected;
}
