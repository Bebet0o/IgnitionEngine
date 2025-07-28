#pragma once
#include "../object/GameObject.h"

class InspectorPanel
{
public:
    InspectorPanel();
    void SetTarget(GameObject* target);
    void Render();

private:
    GameObject* m_Target = nullptr;
};
