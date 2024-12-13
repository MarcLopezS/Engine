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
}

bool Model::LoadModel(const std::string& fileName) 
{
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
            mesh->LoadMesh(model, srcMesh, primitive);
        }
    }

    return true;
}

void Model::Draw() const
{
    //TODO: Pending on implementing Mesh class
}

void Model::Destroy()
{
    //TODO: Pending on implementing Mesh class
}

