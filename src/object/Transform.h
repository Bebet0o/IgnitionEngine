#pragma once
#include "Component.h"
#include "Vector3.h"

struct Transform : public Component {
    Vector3 position{0,0,0};
    Vector3 rotation{0,0,0};
    Vector3 scale{1,1,1};

    std::string GetName() const override { return "Transform"; }
    bool AllowMultiple() const override { return false; }
    static bool AllowMultiple_Static() { return false; }
};
