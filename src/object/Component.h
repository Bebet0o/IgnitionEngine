#pragma once
#include <string>
#include <memory>

class GameObject;

class Component {
public:
    GameObject* gameObject = nullptr;
    virtual ~Component() = default;

    virtual std::string GetName() const { return "Component"; }
    virtual void OnAwake() {}
    virtual void OnStart() {}
    virtual void OnUpdate(float dt) {}
    virtual void OnDestroy() {}

    virtual bool AllowMultiple() const { return false; }
    static bool AllowMultiple_Static() { return false; }
};
