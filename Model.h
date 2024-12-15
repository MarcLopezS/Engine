#pragma once
#include <string>
#include <vector>
#include "Math/float3.h"

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

    void CalculateBoundingBox();
    void CalculateDiagonal();
    float3 GetModelCenter() const;

    void CalcNumVerticesTriangles();
    unsigned int GetTotalTriangles() const { return _totalTriangles; }
    unsigned int GetTotalVertices() const { return _totalVertices; }
    float3 GetMinBounds() { return _minBounds; }
    float3 GetMaxBounds() { return _maxBounds; }
    float GetDiagonalModel() { return _diagonalModel; }

    void Destroy();

private:
    std::vector<Mesh*> _mesh_list;
    std::vector<unsigned int> _textures;

    unsigned int _totalTriangles, _totalVertices;

    float3 _minBounds;
    float3 _maxBounds;
    float _diagonalModel;
};
