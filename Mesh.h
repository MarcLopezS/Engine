#pragma once
#include <string>

namespace tinygltf {
    class Model;
    class Mesh;
    class Primitive;
}

class Mesh {
public:
    Mesh();
    ~Mesh();

    bool LoadMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive);

    void Draw() const;

    void Destroy();

private:
    unsigned int vbo,ebo;

    int materialIndex;
};

