#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Component.h"
#include "Transform.h"
#include "RendererComponent.h"

class GameObject {
public:
    GameObject(const std::string& name = "GameObject");
    ~GameObject();

    template<typename T, typename... Args>
    T* AddComponent(Args&&... args)
    {
        if (!T::AllowMultiple_Static()) {
            for (const auto& comp : m_Components) {
                if (dynamic_cast<T*>(comp.get()) != nullptr)
                    return nullptr;
            }
        }

        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        comp->gameObject = this;
        T* ptr = comp.get();
        m_Components.push_back(std::move(comp));
        return ptr;
    }


    template<typename T>
    T* GetComponent() const {
        for (const auto& comp : m_Components) {
            T* c = dynamic_cast<T*>(comp.get());
            if (c) return c;
        }
        return nullptr;
    }

    const std::vector<std::unique_ptr<GameObject>>& GetChildren() const {
        return m_Children;
    }

    const std::vector<std::unique_ptr<Component>>& GetAllComponents() const { return m_Components; }


    void AddChild(std::unique_ptr<GameObject> child);
    void RemoveChild(GameObject* child);

    std::unique_ptr<GameObject> ExtractChild(GameObject* child);

    std::vector<std::unique_ptr<GameObject>>& GetChildren();
    GameObject* GetParent() const;
    void SetParent(GameObject* parent);

    std::string name;

private:
    GameObject* m_Parent = nullptr;
    std::vector<std::unique_ptr<GameObject>> m_Children;
    std::vector<std::unique_ptr<Component>> m_Components;
};
