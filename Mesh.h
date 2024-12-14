#pragma once

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
    bool LoadEBO(const tinygltf::Model& model, const tinygltf::Primitive& primitive);
    void CreateVAO();
    void Draw() const;

    void Destroy();

private:
    unsigned int _vbo,_ebo, _vao;
    size_t _indexCount;
};

