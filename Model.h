#pragma once
#include <string>
#include <vector>

class Mesh;

class Model {
public:
    Model();
    ~Model();

    bool LoadModel(const std::string& fileName);

    void Draw() const;

    void Destroy();

private:
    std::vector<Mesh*> _mesh_list;
};
