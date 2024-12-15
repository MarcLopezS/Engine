#include "Model.h"
#include "Globals.h"
#include "Mesh.h"
#include "Application.h"
#include "ModuleTexture.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"


Model::Model():_totalTriangles(0), _totalVertices(0), _diagonalModel(0.0f)
{
    _minBounds = float3(FLT_MAX, FLT_MAX, FLT_MAX);
    _maxBounds = float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
}

Model::~Model()
{
    Destroy();
}

bool Model::LoadModel(const std::string& fileName) 
{
    LOG("Loading Model...");
    tinygltf::TinyGLTF gltfContext;
    tinygltf::Model model;
    std::string error, warning;

    bool ret = gltfContext.LoadASCIIFromFile(&model, &error, &warning, fileName);

    if (!ret) {
        LOG("Error loading glTF file: %s", fileName);
        return false;
    }

    if (!warning.empty())
        LOG("Warning: %s", warning);

    for (const auto& srcMesh : model.meshes) {
        for (const auto& primitive : srcMesh.primitives) {
            Mesh* mesh = new Mesh;
            if (mesh->LoadMesh(model, srcMesh, primitive))
            {
                _mesh_list.push_back(mesh);
            }
            else {
                LOG("Error: Failed to load mesh.");
                delete mesh;
            }
        }
    }

    LoadMaterials(model);
    CalcNumVerticesTriangles();
    CalculateBoundingBox();

    LOG("Model Loaded Successfully");
    return true;
}

void Model::LoadMaterials(const tinygltf::Model& srcModel)
{
    LOG("Loading Materials...")
    for (const auto& srcMaterial : srcModel.materials)
    {
        unsigned int textureId = 0;

        if (srcMaterial.pbrMetallicRoughness.baseColorTexture.index >= 0)
        {
            const tinygltf::Texture& texture = srcModel.textures[srcMaterial.pbrMetallicRoughness.baseColorTexture.index];
            const tinygltf::Image& image = srcModel.images[texture.source];

            if (!App->GetModuleTexture()->LoadTexture(image.uri)) 
            {
                LOG("Error: Could not the texture be loaded.");
            }
            else 
            {
                LOG("Texture loaded successfully");

            }

            textureId = App->GetModuleTexture()->GetTextureID();

            if(textureId == 0)
                LOG("Warning: Failed to load texture for material.");
            
        }

        _textures.push_back(textureId);
    }

    LOG("Loaded Material Successfully")
}

void Model::CalculateBoundingBox() {

    for (const auto& mesh : _mesh_list) {
        float3 meshMin = mesh->GetMinBounds();
        float3 meshMax = mesh->GetMaxBounds();

        _minBounds.x = min(_minBounds.x, meshMin.x);
        _minBounds.y = min(_minBounds.y, meshMin.y);
        _minBounds.z = min(_minBounds.z, meshMin.z);

        _maxBounds.x = max(_maxBounds.x, meshMax.x);
        _maxBounds.y = max(_maxBounds.y, meshMax.y);
        _maxBounds.z = max(_maxBounds.z, meshMax.z);
    }

    CalculateDiagonal();
}

void Model::CalculateDiagonal()
{
    _diagonalModel = (_maxBounds - _minBounds).Length();
}

float3 Model::GetModelCenter() const
{
    return (_minBounds + _maxBounds) * 0.5f;
}

void Model::Draw(unsigned int program) const
{
    for (size_t i = 0; i < _mesh_list.size(); ++i) {
        _mesh_list[i]->Draw(program, _textures);
    }
}

void Model::CalcNumVerticesTriangles()
{
    for (auto& mesh : _mesh_list)
    {
        _totalTriangles += mesh->GetNumTriangles();
        _totalVertices += mesh->GetNumVertices();
    }
}

void Model::Destroy()
{
    for (auto& mesh : _mesh_list) {
        if (mesh) {
            mesh->Destroy();
            delete mesh;
        }
    }
    _mesh_list.clear();
    _textures.clear();
}

