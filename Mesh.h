#pragma once
#include <vector>
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
    bool LoadEBO(const tinygltf::Model& model, const tinygltf::Primitive& primitive);
    void CreateVAO();
    void Draw(unsigned int program, const std::vector<unsigned int>& textures);

    void Destroy();

    unsigned int GetNumTriangles() { return _indexCount / 3; }
    unsigned int GetNumVertices() const { return _numVertices; }

private:
    bool LoadAttribute(const tinygltf::Model& model, const tinygltf::Primitive& primitive,const std::string& attributeName, unsigned int& vbo, size_t elementSize);

    unsigned int _vbo,_vbo_UV,_ebo, _vao;
    size_t _indexCount;
    unsigned int _numVertices;
    unsigned int _materialIndex;
};

