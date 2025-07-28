#pragma once
#include <vector>
#include <memory>
#include <string>
#include "GameObject.h"
#include "Transform.h"
#include "json.hpp"
#include <fstream>

class Scene {
public:
    Scene(const std::string& name = "Scene");

    GameObject* CreateGameObject(const std::string& name = "GameObject", GameObject* parent = nullptr);

    void Clear();
    bool SaveToFile(const std::string& path) const;
    bool LoadFromFile(const std::string& path);

    void RemoveGameObject(GameObject* object);
    void RemoveRootGameObject(GameObject* obj);
    void AddRootGameObject(std::unique_ptr<GameObject> obj);

    const std::vector<std::unique_ptr<GameObject>>& GetRootGameObjects() const;

    std::unique_ptr<GameObject> ExtractRootGameObject(GameObject* obj);

    std::string name;

private:
    std::vector<std::unique_ptr<GameObject>> m_RootGameObjects;
};
