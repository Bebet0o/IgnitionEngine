#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Material.h"

class RendererComponent : public Component {
public:
    RendererComponent(Mesh* mesh, Material* material)
        : m_Mesh(mesh), m_Material(material) {}

    std::string GetName() const override { return "Renderer"; }

    Mesh* GetMesh() const { return m_Mesh; }
    Material* GetMaterial() const { return m_Material; }
    bool AllowMultiple() const override { return false; }
    static bool AllowMultiple_Static() { return false; }
private:
    Mesh* m_Mesh;
    Material* m_Material;
};
