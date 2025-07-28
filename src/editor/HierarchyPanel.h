#pragma once
#include "../object/Scene.h"

class HierarchyPanel
{
public:
    HierarchyPanel();
    void SetScene(Scene* scene);
    void Render();
    GameObject* GetSelectedObject() const;

private:
    void DrawGameObjectNode(GameObject* object);

    Scene* m_Scene = nullptr;
    GameObject* m_Selected = nullptr;
    GameObject* m_ContextTarget = nullptr;
};
