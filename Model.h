#pragma once
#include <string>
#include <vector>

class Mesh;

namespace tinygltf {
    class Model;
}

class Model {
public:
    Model();
    ~Model();

    bool LoadModel(const std::string& fileName);
    void LoadMaterials(const tinygltf::Model& srcModel);
    void Draw(unsigned int program) const;

    void Destroy();

private:
    std::vector<Mesh*> _mesh_list;
    std::vector<unsigned int> _textures;
};
