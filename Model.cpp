#include "Model.h"
#include "Globals.h"
#include "Mesh.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"


Model::Model()
{
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

    return true;
}

void Model::Draw() const
{
    for (auto& mesh : _mesh_list) {
        mesh->Draw();
    }
}

void Model::Destroy()
{
    for (auto& mesh : _mesh_list) {
        mesh->Destroy();
    }
    _mesh_list.clear();
}

