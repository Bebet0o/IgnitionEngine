#include "Scene.h"

using json = nlohmann::json;

void Scene::Clear()
{
    m_RootGameObjects.clear();
}

// Sérialisation simple
static json SerializeGameObject(const GameObject* obj)
{
    json j;
    j["name"] = obj->name;
    Transform* t = obj->GetComponent<Transform>();
    if (t) {
        j["position"] = { t->position.x, t->position.y, t->position.z };
        j["rotation"] = { t->rotation.x, t->rotation.y, t->rotation.z };
        j["scale"]    = { t->scale.x, t->scale.y, t->scale.z };
    }
    j["children"] = json::array();
    for (const auto& child : obj->GetChildren())
        j["children"].push_back(SerializeGameObject(child.get()));
    return j;
}

bool Scene::SaveToFile(const std::string& path) const
{
    json j;
    for (const auto& root : m_RootGameObjects)
        j["objects"].push_back(SerializeGameObject(root.get()));
    std::ofstream f(path);
    if (!f.is_open()) return false;
    f << j.dump(2);
    return true;
}

// Désérialisation
static std::unique_ptr<GameObject> DeserializeGameObject(const json& j)
{
    auto obj = std::make_unique<GameObject>(j.value("name", "GameObject"));
    auto* t = obj->GetComponent<Transform>();
    if (t && j.contains("position"))
    {
        auto pos = j["position"];
        t->position = { pos[0], pos[1], pos[2] };
        auto rot = j["rotation"];
        t->rotation = { rot[0], rot[1], rot[2] };
        auto scl = j["scale"];
        t->scale = { scl[0], scl[1], scl[2] };
    }
    if (j.contains("children")) {
        for (const auto& c : j["children"]) {
            obj->AddChild(DeserializeGameObject(c));
        }
    }
    return obj;
}

bool Scene::LoadFromFile(const std::string& path)
{
    std::ifstream f(path);
    if (!f.is_open()) return false;
    json j; f >> j;
    m_RootGameObjects.clear();
    for (const auto& root : j["objects"]) {
        m_RootGameObjects.push_back(DeserializeGameObject(root));
    }
    name = std::filesystem::path(path).stem().string();
    return true;
}

Scene::Scene(const std::string& name)
    : name(name)
{
}

GameObject* Scene::CreateGameObject(const std::string& name, GameObject* parent)
{
    auto obj = std::make_unique<GameObject>(name);

    if (parent == nullptr) {
        // Ajoute comme racine
        m_RootGameObjects.push_back(std::move(obj));
        return m_RootGameObjects.back().get();
    } else {
        // Ajoute comme enfant du parent
        parent->AddChild(std::move(obj));
        return parent->GetChildren().back().get();
    }
}

void Scene::AddRootGameObject(std::unique_ptr<GameObject> obj)
{
    obj->SetParent(nullptr);
    m_RootGameObjects.push_back(std::move(obj));
}

std::unique_ptr<GameObject> Scene::ExtractRootGameObject(GameObject* obj)
{
    auto it = std::find_if(m_RootGameObjects.begin(), m_RootGameObjects.end(),
        [obj](const std::unique_ptr<GameObject>& ptr) { return ptr.get() == obj; });
    if (it != m_RootGameObjects.end()) {
        std::unique_ptr<GameObject> result = std::move(*it);
        m_RootGameObjects.erase(it);
        return result;
    }
    return nullptr;
}

void Scene::RemoveRootGameObject(GameObject* obj) {
    m_RootGameObjects.erase(
        std::remove_if(m_RootGameObjects.begin(), m_RootGameObjects.end(),
            [obj](const std::unique_ptr<GameObject>& ptr) { return ptr.get() == obj; }),
        m_RootGameObjects.end());
}

void Scene::RemoveGameObject(GameObject* object)
{
    // Supprime seulement dans les racines (suppression récursive à améliorer si tu veux)
    m_RootGameObjects.erase(
        std::remove_if(m_RootGameObjects.begin(), m_RootGameObjects.end(),
            [object](const std::unique_ptr<GameObject>& obj) { return obj.get() == object; }),
        m_RootGameObjects.end());
}

const std::vector<std::unique_ptr<GameObject>>& Scene::GetRootGameObjects() const
{
    return m_RootGameObjects;
}
