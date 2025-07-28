#include "GameObject.h"

GameObject::GameObject(const std::string& n)
    : name(n)
{
    AddComponent<Transform>();
}

GameObject::~GameObject() {
    for (auto& comp : m_Components)
        comp->OnDestroy();
}


template Transform* GameObject::AddComponent<Transform>();

void GameObject::AddChild(std::unique_ptr<GameObject> child)
{
    child->m_Parent = this;
    m_Children.push_back(std::move(child));
}

std::unique_ptr<GameObject> GameObject::ExtractChild(GameObject* child)
{
    auto it = std::find_if(m_Children.begin(), m_Children.end(),
        [child](const std::unique_ptr<GameObject>& ptr) { return ptr.get() == child; });
    if (it != m_Children.end()) {
        std::unique_ptr<GameObject> result = std::move(*it);
        m_Children.erase(it);
        return result;
    }
    return nullptr;
}


void GameObject::RemoveChild(GameObject* child)
{
    m_Children.erase(
        std::remove_if(m_Children.begin(), m_Children.end(),
            [child](const std::unique_ptr<GameObject>& ptr) { return ptr.get() == child; }),
        m_Children.end());
}

std::vector<std::unique_ptr<GameObject>>& GameObject::GetChildren()
{
    return m_Children;
}

GameObject* GameObject::GetParent() const
{
    return m_Parent;
}

void GameObject::SetParent(GameObject* parent)
{
    m_Parent = parent;
}